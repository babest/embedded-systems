#include <Wire.h>

// Examples:
// http://www.arduino.cc/en/Tutorial/MasterWriter
// http://www.arduino.cc/en/Tutorial/MasterReader



// Init Vars

int slaveAddress = 42;

const int requestedMessageSize = 15;
char requestedMessage[requestedMessageSize];



// Programm Setup


void setup() {
  // init console output
  Serial.begin(9600);
  
  Wire.begin();
}



// Programm Loop

void loop() {
  
  ledOn();
  requestMessage();
  delay(2000);
  ledOff();
  requestMessage();
  delay(2000);
  
}



// LED Control

void ledOn() {
  flushMessage("LED_ON");
}

void ledOff() {
  flushMessage("LED_OFF");
}



// Communication

void flushMessage(const char* message) {
  Serial.print("flushMessage: ");
  Serial.println(message);
  
  Wire.beginTransmission(slaveAddress);
  Wire.write(message);
  Wire.endTransmission();
}

void requestMessage() {
  int bytes = Wire.requestFrom(slaveAddress, requestedMessageSize); // request 6 bytes from slave device; 
  memset(requestedMessage, '\0', requestedMessageSize);
  
  for(int i=0; i < bytes; i++) {
    *(requestedMessage+i) = Wire.read();
  }
  
  Serial.print("Requested Message: ");
  Serial.println(requestedMessage);
}



