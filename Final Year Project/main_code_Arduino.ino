#include<Servo.h>
#include <Mouse.h>
const int trigpin= 6;
const int echopin= 5;
long duration;
int distance;
Servo s;
String gateStatus = "closed";
int smokeA0 = A0;
void setup() {
  s.attach(9);
  pinMode(trigpin,OUTPUT);
  pinMode(echopin,INPUT);
  pinMode(4,OUTPUT);
  pinMode(7,OUTPUT);
  pinMode(2,INPUT);
  pinMode(11,OUTPUT);
  pinMode(8,OUTPUT);
  pinMode(10,OUTPUT);
  Serial.begin(9600); // sets the serial port to 9600
  Serial.println("Gas sensor warming up!");
  delay(2000); // allow the MQ-2 to warm up
  digitalWrite(7,HIGH);
}


void loop() {

  //Smoke sensor code
  float smoke = analogRead(smokeA0);
  Serial.print("Smoke value: ");
  Serial.println(smoke);
  if(smoke > 200)
  {
    Serial.println(" | Smoke detected!");
    digitalWrite(10,HIGH);
    digitalWrite(11,HIGH);
    delay(200);
  }
  else
  {
     Serial.println(" | Smoke  not detected!");
     digitalWrite(10,LOW);
     digitalWrite(11,LOW);
  }
  
  //Temprature sensor code
  float temp = analogRead(A1);
  Serial.print("Temprature value: ");  
  Serial.println(temp);
  if(temp >= 120)
  {
    digitalWrite(8,HIGH);
    digitalWrite(11,HIGH);
    delay(1000);
  }
  else
  {
    digitalWrite(8,LOW);
    digitalWrite(11,LOW);
  }


  //Ultrasonic sensor code
  digitalWrite(trigpin,HIGH);
  delayMicroseconds(10);
  digitalWrite(trigpin,LOW);
  duration=pulseIn(echopin,HIGH);
  distance = duration*0.034/2;
  Serial.print("Ultrasonic sensor distance: ");
  Serial.println(distance);

  if(distance <=30)
  {
    digitalWrite(4,HIGH);
    digitalWrite(11,HIGH);
    delay(2000);
  }
  else
  {
    digitalWrite(4,LOW);
    digitalWrite(11,LOW);
  }

  
  //Gate state configuartion.
  if (gateStatus == "closed")
  {
    s.write(0);
  }
  else
  {
    s.write(180);
  }
  checkGateStatus();
  delay(500); // wait 500ms for next reading
}
  
void checkGateStatus()
{
  int value = digitalRead(2);
  if(value==HIGH)
  {
    gateStatus = "open";
  }
  else
  {
    gateStatus = "closed";
  }
}
