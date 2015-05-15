#include <Servo.h>

int servoOutputPin = 3;
Servo servo;
int servoPos = 0;
int servoMax = 180;
int servoAdjust = 23; // 23 - 157°


void setup() {
  // put your setup code here, to run once:
  
  Serial.begin(9600);
  
  servo.attach(servoOutputPin);
}


void loop() {
  // put your main code here, to run repeatedly:
  
  for(servoPos = servoAdjust; servoPos < servoMax - servoAdjust; servoPos += 1)  // goes from 0 degrees to 180 degrees
  {                                  // in steps of 1 degree
    servo.write(servoPos);              // tell servo to go to position in variable 'pos'
    Serial.println(servo.read());
    delay(400);                       // waits 15ms for the servo to reach the position
  }
  for(servoPos = servoMax-servoAdjust; servoPos>servoAdjust; servoPos-=1)     // goes from 180 degrees to 0 degrees
  {                                
    servo.write(servoPos);              // tell servo to go to position in variable 'pos'
    delay(15);                       // waits 15ms for the servo to reach the position
  }
}
