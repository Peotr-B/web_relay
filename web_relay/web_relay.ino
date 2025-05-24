 
/* 16мая25
https://robotchip.ru/obzor-modulya-rele-dlya-esp8266-01s-esp-01s/?ysclid=m5a1xykmc4321151192
Обзор модуля реле для ESP8266-01S

23мая25
https://projectalt.ru/publ/arduino_esp8266_i_esp32/programmirovanie/esp8266_nodemcu_otpravka_soobshhenij_v_whatsapp/11-1-0-26
ESP8266 NodeMCU: отправка сообщений в WhatsApp

*/

#include <ESP8266WiFi.h>                      // Подключаем библиотеку ESP8266WiFi
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#include <UrlEncode.h>

const char* ssid = "*****";     // Название Вашей WiFi сети
const char* password = "*****";// Пароль от Вашей WiFi сети

// +international_country_code + phone number
// Portugal +351, example: +351912345678
String phoneNumber = "+7*****";
String apiKey = "*****";

#define RELAY 0                               // Пин к которому подключен датчик
WiFiServer server(80);                        // Указываем порт Web-сервера

//Подпрограмма отправки сообщения в Ватсапп
void sendMessage(String message){

 // Data to send with HTTP POST
 String url = "http://api.callmebot.com/whatsapp.php?phone=" + phoneNumber + "&apikey=" + apiKey + "&text=" + urlEncode(message);
 WiFiClient client; 
 HTTPClient http;
 http.begin(client, url);

 // Specify content-type header
 //Отправка запроса HTTP post и сохранение кода ответа
 http.addHeader("Content-Type", "application/x-www-form-urlencoded");
 
 // Send HTTP POST request
 //Если код ответа равен 200, это означает, что запрос post прошел успешно. 
 //В противном случае что-то пошло не так.
 int httpResponseCode = http.POST(url);
 if (httpResponseCode == 200){
 Serial.print("Message sent successfully");
 }
 else{
 Serial.println("Error sending the message");
 Serial.print("HTTP response code: ");
 Serial.println(httpResponseCode);
 }

 // Free resources
 //Освобождение ресурсов
 http.end();
}

void setup(){
  delay(2200);      
  Serial.begin(115200);                       // Скорость передачи 115200 
//  Serial.begin(748800);
  pinMode(RELAY,OUTPUT);                      // Указываем вывод RELAY как выход
  digitalWrite(RELAY, LOW);                   // Устанавливаем RELAY в LOW (0В)
  Serial.println();                           // Печать пустой строки 
  Serial.print("Connecting to ");             // Печать "Подключение к:"
  Serial.println(ssid);                       // Печать "Название Вашей WiFi сети"
 
  WiFi.begin(ssid, password);                 // Подключение к WiFi Сети
 
  while (WiFi.status() != WL_CONNECTED)       // Проверка подключения к WiFi сети
  { 
    delay(500);                               // Пауза 500 мкс
    Serial.print(".");                        // Печать "."
  }
  Serial.println("");                         // Печать пустой строки  
 // Serial.println("WiFi connected");           // Печать "WiFi connected"

  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());

 // Send Message to WhatsAPP
 //Отправка контрольного сообщения на WhatsAPP
 sendMessage("Hello from ESP8266!");
 
  server.begin();                             // Запуск сервера
  Serial.println("Server started");           // Печать "Server starte"
  Serial.print("Use this URL to connect: ");  // Печать "Use this URL to connect:" 
  Serial.print(WiFi.localIP());               // Печать выданого IP адресса          
}
 
void loop(){
   WiFiClient client = server.available();    // Получаем данные, посылаемые клиентом 
  if (!client)                                
  {
    return;
  }
  Serial.println("new client");               // Отправка "new client"
  while(!client.available())                  // Пока есть соединение с клиентом 
  {
    delay(1);                                 // пауза 1 мс
  }
 
  String request = client.readStringUntil('\r');
  Serial.println(request);
  client.flush();
 
  int value = LOW;
  if (request.indexOf("/RELAY=ON") != -1)  
  {
    Serial.println("RELAY=ON");
    digitalWrite(RELAY,LOW);
    value = LOW;
    sendMessage("RELAY=ON!");
  }
  if (request.indexOf("/RELAY=OFF") != -1)  
  {
    Serial.println("RELAY=OFF");
    digitalWrite(RELAY,HIGH);
    value = HIGH;
    sendMessage("RELAY=OFF!");
  }
 
  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: text/html");
  client.println(""); 
  client.println("<!DOCTYPE HTML>");
  client.println("<html>");
  client.println("<head><title>ESP8266 RELAY Control</title></head>");
  client.print("Relay is now: ");
 
  if(value == HIGH) 
  {
    client.print("OFF");
  } 
  else 
  {
    client.print("ON");
  }
  client.println("<br><br>");
  client.println("Turn <a href=\"/RELAY=OFF\">OFF</a> RELAY<br>");
  client.println("Turn <a href=\"/RELAY=ON\">ON</a> RELAY<br>");
    client.println("</html>");
 
  delay(1);
  Serial.println("Client disonnected");
  Serial.println("");
}
