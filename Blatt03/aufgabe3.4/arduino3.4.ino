#include <Servo.h>

int servoOutputPin = 3;
Servo servo;
int servoPos = 0;
int servoMax = 180;
int servoAdjust = 23; // 23 - 157°

char serialData[100];

void setup() {
  // put your setup code here, to run once:
  
  Serial.begin(9600);
  
  servo.attach(servoOutputPin);
}


void loop() {
  // put your main code here, to run repeatedly:
  
  if(0 < Serial.available()) {
    Serial.readBytesUntil('\n', serialData, 100);
    
    if (strstr(serialData, "LED_on") != null) {
      LED_on();
    }
    else if (strstr(serialData, "LED_off") != null) {
      LED_off();
    }
    else if (strstr(serialData, "moveTo(") != null) {
      int value = atoi(serialData+strlen("moveTo("));
      moveTo(value);
      //TODO: weiterschreiben
    }
  }
  
  servo.write(servoPos);
}


void LED_on() {
  digitalWrite(LEDPin, HIGH);
}

void LED_off() {
  digitalWrite(LEDPin, LOW);
}
