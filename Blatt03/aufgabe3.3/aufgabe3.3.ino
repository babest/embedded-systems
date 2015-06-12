#include <Servo.h>

int zAxisInputPin = A0;
int xAxisInputPin = A1;
int vrefInputPin = A2;
int azOutputPin = 2;
int gyroThreshold = 5;

int servoOutputPin = 3;
Servo servo;
int servoPos = 0;
int servoMax = 180;
int servoAdjust = 23; // 23 - 157°

int LEDPin = 13;

float currentXAxisValue = 0;
float currentZAxisValue = 0;

char data[100];

int LEDCurrentValue = LOW;
int LEDLeftBlink = 0;
int LEDLeftCount = 0;


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



void setup() {
  // put your setup code here, to run once:
  
  Serial.begin(9600);
  
  servo.attach(servoOutputPin);
  pinMode(LEDPin, OUTPUT);
  
  /*pinMode(zAxisInputPin, INPUT);
  pinMode(xAxisInputPin, INPUT);
  pinMode(vrefInputPin, INPUT);*/
  pinMode(azOutputPin, OUTPUT);
  
  digitalWrite(azOutputPin, LOW);
  delay(200);
  digitalWrite(azOutputPin, HIGH);
  delay(200);
  digitalWrite(azOutputPin, LOW);
  
  startTimer(TC0, 1, TC1_IRQn, 10);
}

void loop() {
  // put your main code here, to run repeatedly:
  
  Serial.println(data);
  //Serial.println(currentZAxisValue);
  
  if (servoPos < 23) {
    servoPos = 23;
    SetLEDBlink(3);
  } else if(servoPos > 157) {
    servoPos = 157;
    SetLEDBlink(3);
  }
  
  servo.write(servoPos);
  LEDUpdate();
}



void SetLEDBlink(int count) {
  LEDLeftBlink = count;
}

void LEDUpdate() {
  
  if (LEDLeftCount > 0) {
    LEDLeftCount--;
  }
  else {
    if (LEDCurrentValue == HIGH) {
      LEDCurrentValue = LOW;
      LEDLeftCount = 1;
    }
    else if (LEDCurrentValue == LOW && LEDLeftBlink > 0) {
      LEDCurrentValue = HIGH;
      LEDLeftBlink--;
      LEDLeftCount = 1;
    }
  }
  
  digitalWrite(LEDPin, LEDCurrentValue);
}



void TC1_Handler() {
  
  //                  (Gyro Output Voltage - Gyro Zero-Rate Out) / Sensitivity;
  currentXAxisValue = (int)analogRead(xAxisInputPin) - (int)analogRead(vrefInputPin);
  currentXAxisValue = currentXAxisValue * (5000 / 1024) / 9.1;
  
  currentZAxisValue = (int)analogRead(zAxisInputPin) - (int)analogRead(vrefInputPin);
  currentZAxisValue = currentZAxisValue * (5000 / 1024) / 9.1;
  
  //currentXAxisValue = (gyroThreshold < fabs(currentXAxisValue)) ? ((0 < currentXAxisValue) ? currentXAxisValue - gyroThreshold : currentXAxisValue + gyroThreshold) : 0;
  //currentZAxisValue = (gyroThreshold < fabs(currentZAxisValue)) ? ((0 < currentZAxisValue) ? currentZAxisValue - gyroThreshold : currentZAxisValue + gyroThreshold) : 0;
  currentXAxisValue = (gyroThreshold < fabs(currentXAxisValue)) ? currentXAxisValue : 0;
  currentZAxisValue = (gyroThreshold < fabs(currentZAxisValue)) ? currentZAxisValue : 0;
  
  sprintf(data, "x: %6d %6d % 3.3f   |   z: %6d %6d % 3.3f", //TODO: nicer display with spacing
    analogRead(xAxisInputPin), ((int)analogRead(xAxisInputPin) - (int)analogRead(vrefInputPin)), currentXAxisValue,
    analogRead(zAxisInputPin), ((int)analogRead(zAxisInputPin) - (int)analogRead(vrefInputPin)), currentZAxisValue);
  
  servoPos += currentZAxisValue / 10;
  
  // We need to get the status to clear it and allow the interrupt to fire again
  TC_GetStatus(TC0, 1);
}
