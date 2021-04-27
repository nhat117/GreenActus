#include <ESP8266WiFi.h>
#include <DHT.h> 
#include "MQ135.h"
#include <Servo.h>
String apiKey = "31KI0CZWA9A7U5U9";     //  Enter your Write API key from ThingSpeak
 
const char *ssid =  "MINHTRI";     // replace with your wifi ssid and wpa2 key
const char *pass =  "0988546133";
const char* server = "api.thingspeak.com";
#define PIN_MQ135 A0  // 
#define DHTPIN 0          //pin D3
#define SERVOPIN D4
#define ENVIRONMENT 2
//Object Init
Servo myservo;
DHT dht(DHTPIN, DHT11);
MQ135 mq135_sensor = MQ135(PIN_MQ135); 
WiFiClient client;
 
void setup() 
{
       Serial.begin(115200);
       delay(10);
       dht.begin();
 
       Serial.println("Connecting to ");
       Serial.println(ssid);
 
 
       WiFi.begin(ssid, pass);
 
      while (WiFi.status() != WL_CONNECTED) 
     {
            delay(500);
            Serial.print(".");
     }
      Serial.println("");
      Serial.println("WiFi connected");
      myservo.attach(SERVOPIN);
}
 
void loop() 
{
      int ServoAngle = 0;
//      float h = dht.readHumidity();
      float h = 50;
//      float t = dht.readTemperature();
      float t =27;
      float rzero = mq135_sensor.getRZero();
      float correctedRZero = mq135_sensor.getCorrectedRZero(t, h);
      float resistance = mq135_sensor.getResistance();
      float ppm = mq135_sensor.getPPM();
      float correctedPPM = mq135_sensor.getCorrectedPPM(t, h);
                  if (isnan(h) || isnan(t)) 
                 {
                     Serial.println("Failed to read from DHT sensor!");
                      return;
                 }
 
                         if (client.connect(server,80))   //   "184.106.153.149" or api.thingspeak.com
                      {  
                            
                             String postStr = apiKey;
                             postStr +="&field1=";
                             postStr += String(t);
                             postStr +="&field2=";
                             postStr += String(h);
                             postStr +="&field3=";
                             postStr += String(correctedPPM);
                             postStr += "\r\n\r\n";
 
                             client.print("POST /update HTTP/1.1\n");
                             client.print("Host: api.thingspeak.com\n");
                             client.print("Connection: close\n");
                             client.print("X-THINGSPEAKAPIKEY: "+apiKey+"\n");
                             client.print("Content-Type: application/x-www-form-urlencoded\n");
                             client.print("Content-Length: ");
                             client.print(postStr.length());
                             client.print("\n\n");
                             client.print(postStr);
 
                             Serial.print("Temperature: ");
                             Serial.print(t);
                             Serial.print(" degrees Celcius, Humidity: ");
                             Serial.print(h);
                             Serial.print ("Air quality PPM: ");
                             Serial.print (correctedPPM);
                             Serial.println("%. Send to Thingspeak.");
                        }
                          if (correctedPPM < ENVIRONMENT) { //Clean air
                          //Control Servi
                              if (ServoAngle != 0) {
                                for (ServoAngle = 180; ServoAngle > 0; ServoAngle --) {
                                     myservo.write(ServoAngle);
                                     delay (15); 
                              }
                          } else {
                            ServoAngle = 0;
                          }
  }
          client.stop();
 
          Serial.println("Waiting...");
  
  // thingspeak needs minimum 15 sec delay between updates
  delay(500);
}
