

int zAxisInputPin = A0;
int xAxisInputPin = A1;
int vrefInputPin = A2;
int azOutputPin = 2;

int currentZAxisValue = 0;


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
  
  pinMode(zAxisInputPin, INPUT);
  pinMode(xAxisInputPin, INPUT);
  pinMode(vrefInputPin, INPUT);
  pinMode(azOutputPin, OUTPUT);
  
  digitalWrite(azOutputPin, LOW);
  delay(200);
  digitalWrite(azOutputPin, HIGH);
  delay(200);
  digitalWrite(azOutputPin, LOW);
  
  startTimer(TC0, 0, TC0_IRQn, 10);
}

void loop() {
  // put your main code here, to run repeatedly:
  
  //Serial.println(currentZAxisValue);
  char data[10];
  sprintf(data, "%d", currentZAxisValue);
  Serial.println(data);
  Serial.println(currentZAxisValue);
}



void TC0_Handler() {
  
  //                  (Gyro Output Voltage - Gyro Zero-Rate Out) / Sensitivity;
  currentZAxisValue = (double)(analogRead(zAxisInputPin) - analogRead(vrefInputPin)) * 10 / 91;
  
  // We need to get the status to clear it and allow the interrupt to fire again
  TC_GetStatus(TC0, 0);
}
