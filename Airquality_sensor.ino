#include "MQ135.h"
#include "Servo.h"
//Pin setup 
#define PIN_MQ135 A2
#define SERVOPIN 6 //PWM PIN 6
#define LEDPIN 3

MQ135 mq135_sensor = MQ135(PIN_MQ135);
Servo myservo;

int ServoAngle = 0;
float temperature = 25.0;
float humidity = 60.0;
 
void setup() {
  pinMode (LEDPIN, OUTPUT);
  myservo.attach(SERVOPIN); //Attach sẻvo to pin 5
  Serial.begin(9600);
}
 
void loop() {
  //Setup variable
  float rzero = mq135_sensor.getRZero();
  float correctedRZero = mq135_sensor.getCorrectedRZero(temperature, humidity);
  float resistance = mq135_sensor.getResistance();
  float ppm = mq135_sensor.getPPM();
  float correctedPPM = mq135_sensor.getCorrectedPPM(temperature, humidity);

 //Print information onserial monitor
  Serial.print("MQ135 RZero: ");
  Serial.print(rzero);
  Serial.print("\t Corrected RZero: ");
  Serial.print(correctedRZero);
  Serial.print("\t Resistance: ");
  Serial.print(resistance);
  Serial.print("\t PPM: ");
  Serial.print(ppm);
  Serial.print("\t Airquality  (Corrected PPM) : ");
  Serial.print(correctedPPM);
  Serial.println("ppm");

  //Controll LED & Servo
  if(correctedPPM >= 1) { // Air is polluted
      digitalWrite (LEDPIN, HIGH);
      for (ServoAngle = 0; ServoAngle < 180; ServoAngle ++) {
          myservo.write(ServoAngle);
         delay (15);
      }
     
  }
  
  if (correctedPPM < 1) { //Clean air
      digitalWrite (LEDPIN, LOW);
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
  delay(500);
}
