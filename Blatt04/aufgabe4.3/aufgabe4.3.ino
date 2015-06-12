#include <Wire.h>

// Examples:
// http://www.arduino.cc/en/Tutorial/MasterWriter
// http://www.arduino.cc/en/Tutorial/MasterReader



// Init Vars

// I^2C
int slaveAddress = 42;
const int requestedMessageSize = 15;
char requestedMessage[requestedMessageSize];

// Gyro
int zAxisInputPin = A0;
int vrefInputPin = A2;
int azOutputPin = 2;
int gyroThreshold = 5;
float currentZAxisValue = 0;

// Communication
int currentAngle = 90;


// Programm Setup


void setup() {
  
  // Gyro Init
  pinMode(azOutputPin, OUTPUT);
  
  // Gyro reset Threshold
  digitalWrite(azOutputPin, LOW);
  delay(200);
  digitalWrite(azOutputPin, HIGH);
  delay(200);
  digitalWrite(azOutputPin, LOW);
  
  // init console output
  Serial.begin(9600);
  // Init i^2C
  Wire.begin();
  
  startTimer(TC0, 1, TC1_IRQn, 10);
  
}



// Programm Loop

void loop() {
  
  sendCurrentAngle();
  delay(100);  
}



// Angle control

void sendCurrentAngle() {
  char message[100];
  sprintf(message, "MOVE_TO(%d)\n", currentAngle);
  
  flushMessage(message);
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



// Gyro

void readGyroValues() {
  
  //                  (Gyro Output Voltage - Gyro Zero-Rate Out) / Sensitivity;
  currentZAxisValue = (int)analogRead(zAxisInputPin) - (int)analogRead(vrefInputPin);
  currentZAxisValue = currentZAxisValue * (5000 / 1024) / 9.1;
  
  currentZAxisValue = (gyroThreshold < fabs(currentZAxisValue)) ? currentZAxisValue : 0;
}

void updateAngle() {
  currentAngle += currentZAxisValue / 10;
  if(currentAngle<0) {
    currentAngle=0;
  } else if(180 < currentAngle) {
    currentAngle=180;
  }
}



// Timer handling

// https://android.googlesource.com/platform/external/arduino-ide/+/f876b2abdebd02acfa4ba21e607327be4f9668d4/hardware/arduino/sam/cores/arduino/wiring_analog.c
// http://forum.arduino.cc/index.php?topic=130423.15
void startTimer(Tc *tc, uint32_t channel, IRQn_Type irq, uint32_t frequency) {
        pmc_set_writeprotect(false);
        pmc_enable_periph_clk((uint32_t)irq);
        
        //>> Hier erfolgt die Konfiguration des Timers
        TC_Configure(tc, channel, TC_CMR_WAVE | TC_CMR_WAVSEL_UP_RC | TC_CMR_TCCLKS_TIMER_CLOCK4);
        
        uint32_t rc = VARIANT_MCK/128/frequency; //128 because we selected TIMER_CLOCK4 above
        //TC_SetRA(tc, channel, rc/2); //50% high, 50% low
        TC_SetRC(tc, channel, rc);
        TC_Start(tc, channel);
        
        tc->TC_CHANNEL[channel].TC_IER=TC_IER_CPCS;
        tc->TC_CHANNEL[channel].TC_IDR=~TC_IER_CPCS;
        
        NVIC_ClearPendingIRQ(irq);
        NVIC_EnableIRQ(irq);
}

void TC1_Handler() {
  
  readGyroValues();
  updateAngle();
  
  // We need to get the status to clear it and allow the interrupt to fire again
  TC_GetStatus(TC0, 1);
}

