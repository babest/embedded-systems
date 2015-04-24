// These are the clock frequencies available to the timers /2,/8,/32,/128
// 84Mhz/2 = 42.000 MHz     (TC_CMR_TCCLKS_TIMER_CLOCK1)
// 84Mhz/8 = 10.500 MHz     (TC_CMR_TCCLKS_TIMER_CLOCK2)
// 84Mhz/32 = 2.625 MHz     (TC_CMR_TCCLKS_TIMER_CLOCK3)
// 84Mhz/128 = 656.250 KHz  (TC_CMR_TCCLKS_TIMER_CLOCK4)

// Counter
int counterPin3High = 0;
int counterPin3Low= 0;
int counterPin5High = 0;
int counterPin5Low = 0;

// store laste state of pins
int pin3LastState = HIGH;
int pin5LastState = HIGH;
int LED7Level = 0;

//https://android.googlesource.com/platform/external/arduino-ide/+/f876b2abdebd02acfa4ba21e607327be4f9668d4/hardware/arduino/sam/cores/arduino/wiring_analog.c
//http://forum.arduino.cc/index.php?topic=130423.15
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
  
  startTimer(TC0, 0, TC0_IRQn, 1000);
  startTimer(TC0, 1, TC1_IRQn, 1000);
  
  //>> Hier wird der konfigurierte Timer gestartet
  
  pinMode(3, INPUT);
  pinMode(5, INPUT);
  pinMode(7, OUTPUT);
  pinMode(9, OUTPUT);
  digitalWrite(7, LOW);
  digitalWrite(9, LOW);
  
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  //Serial.println(LED7Level);
}

void TC0_Handler() {
  // increase counter based on current pin input value
  (digitalRead(3) == HIGH) ? counterPin3High++ : counterPin3Low++;
  
  if (counterPin3High + counterPin3Low > 30) {
    
    if (counterPin3High > counterPin3Low && pin3LastState == LOW) {
      pin3High();
      pin3LastState = HIGH;
    }
    else if (counterPin3High < counterPin3Low && pin3LastState == HIGH) {
      pin3Low();
      pin3LastState = LOW;
    }
    
    counterPin3High = counterPin3Low = 0;
  }
  
  // We need to get the status to clear it and allow the interrupt to fire again
  TC_GetStatus(TC0, 0);
}

void TC1_Handler() {
  // increase counter based on current pin input value
  (digitalRead(5) == HIGH) ? counterPin5High++ : counterPin5Low++;
  
  if (counterPin5High + counterPin5Low > 30) {
    
    if (counterPin5High > counterPin5Low && pin5LastState == LOW) {
      pin5High();
      pin5LastState = HIGH;
    }
    else if (counterPin5High < counterPin5Low && pin5LastState == HIGH) {
      pin5Low();
      pin5LastState = LOW;
    }
    
    counterPin5High = counterPin5Low = 0;
  }
  
  // We need to get the status to clear it and allow the interrupt to fire again
  TC_GetStatus(TC0, 1);
}

void pin3High() {
  //Serial.println("Pin 3 High");
}

void pin3Low() {
  Serial.println("Pin 3 Low");
  
  LED7Level -= 10;
  if (LED7Level < 0) LED7Level = 0;
  
  analogWrite(7, LED7Level);
  Serial.println(LED7Level);
}

void pin5High() {
  //Serial.println("Pin 5 High");
}

void pin5Low() {
  Serial.println("Pin 5 Low");
  
  LED7Level += 10;
  if (LED7Level > 255) LED7Level = 255;
  
  analogWrite(7, LED7Level);
  Serial.println(LED7Level);
}




void backup() {
  TC0->TC_CHANNEL[0].TC_IER = TC_IER_CPCS;
  
  const uint32_t TC = 1000/2;
  
  TC_SetRA(TC0, 0, TC / 2);
  TC_SetRC(TC0, 0, TC);
}
