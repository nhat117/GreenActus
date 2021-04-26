#include <SoftwareSerial.h>
#include <dht11.h>
#include <MQ135.h>
#define RX 2
#define TX 3
#define dht_apin 11 // Analog Pin sensor is connected to
dht11 dhtObject;
#define PIN_MQ135 A1
String AP = "G7";       // AP NAME
String PASS = "0988546133"; // AP PASSWORD
String API = "31KI0CZWA9A7U5U9";   // Write API KEY
String HOST = "api.thingspeak.com";
String PORT = "80";
int countTrueCommand;
int countTimeCommand; 
boolean found = false; 
int valSensor = 1;

float temperature;
float humid;

MQ135 mq135_sensor = MQ135(PIN_MQ135);
  
SoftwareSerial esp8266(RX,TX); 
  
void setup() {
  Serial.begin(9600);
  esp8266.begin(9600);
  sendCommand("AT",5,"OK");
  sendCommand("AT+CWMODE=1",5,"OK");
  sendCommand("AT+CWJAP=\""+ AP +"\",\""+ PASS +"\"",20,"OK");
}

void loop() {

  //Setup variable
  
 String getData = "GET /update?api_key="+ API +"&field1="+getTemperatureValue()+"&field2="+getHumidityValue() + "&field3=" + getAirQuality();
 sendCommand("AT+CIPMUX=1",5,"OK");
 sendCommand("AT+CIPSTART=0,\"TCP\",\""+ HOST +"\","+ PORT,15,"OK");
 sendCommand("AT+CIPSEND=0," +String(getData.length()+4),4,">");
 esp8266.println(getData);delay(1500);countTrueCommand++;
 sendCommand("AT+CIPCLOSE=0",5,"OK");
}


String getTemperatureValue(){

   dhtObject.read(dht_apin);
   Serial.print(" Temperature(C)= ");
   int temp = dhtObject.temperature;
   temperature = temp;
   Serial.println(temp); 
   delay(50);
   return String(temp); 
  
}


String getHumidityValue(){

   dhtObject.read(dht_apin);
   Serial.print(" Humidity in %= ");
   int humidity = dhtObject.humidity;
   humid = humidity;
   Serial.println(humidity);
   delay(50);
   return String(humidity); 
  
}

String getAirQuality(){
  Serial.print("Airquality in ppm = ");
  float rzero = mq135_sensor.getRZero();
  float correctedRZero = mq135_sensor.getCorrectedRZero(temperature, humid);
  float resistance = mq135_sensor.getResistance();
  float ppm = mq135_sensor.getPPM();
  float correctedPPM = mq135_sensor.getCorrectedPPM(temperature, humid);
  Serial.println(correctedPPM);
  delay (50);
  return String(correctedPPM);
}



void sendCommand(String command, int maxTime, char readReplay[]) {
  Serial.print(countTrueCommand);
  Serial.print(". at command => ");
  Serial.print(command);
  Serial.print(" ");
  while(countTimeCommand < (maxTime*1))
  {
    esp8266.println(command);//at+cipsend
    if(esp8266.find(readReplay))//ok
    {
      found = true;
      break;
    }
  
    countTimeCommand++;
  }
  
  if(found == true)
  {
    Serial.println("OYI");
    countTrueCommand++;
    countTimeCommand = 0;
  }
  
  if(found == false)
  {
    Serial.println("Fail");
    countTrueCommand = 0;
    countTimeCommand = 0;
  }
  
  found = false;
 }
