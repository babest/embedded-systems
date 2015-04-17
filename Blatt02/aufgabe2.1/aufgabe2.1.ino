// These are the clock frequencies available to the timers /2,/8,/32,/128
// 84Mhz/2 = 42.000 MHz     (TC_CMR_TCCLKS_TIMER_CLOCK1)
// 84Mhz/8 = 10.500 MHz     (TC_CMR_TCCLKS_TIMER_CLOCK2)
// 84Mhz/32 = 2.625 MHz     (TC_CMR_TCCLKS_TIMER_CLOCK3)
// 84Mhz/128 = 656.250 KHz  (TC_CMR_TCCLKS_TIMER_CLOCK4)

// Counter for fun
int count = 0;


void setup() {
  pmc_set_writeprotect(false);
  pmc_enable_periph_clk(ID_TC0);
  
  //>> Hier erfolgt die Konfiguration des Timers
  TC_Configure(TC0, 0, TC_CMR_WAVE | TC_CMR_WAVSEL_UP_RC | TC_CMR_TCCLKS_TIMER_CLOCK4);
  
  TC_SetRC(TC0, 0, 6562.5); // 656250 Hz / 656.25/ 10 = 100 Hz
  TC_Start(TC0, 0);
  
  TC0->TC_CHANNEL[0].TC_IER = TC_IER_CPCS;
  TC0->TC_CHANNEL[0].TC_IDR = ~TC_IER_CPCS;
  
  NVIC_ClearPendingIRQ(TC0_IRQn);
  NVIC_EnableIRQ(TC0_IRQn);
  
  //>> Hier wird der konfigurierte Timer gestartet
  
  pinMode(3, INPUT);
  pinMode(5, INPUT);
  pinMode(7, OUTPUT);
  pinMode(9, OUTPUT);
  digitalWrite(7, HIGH);
  digitalWrite(9, HIGH);
  
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  Serial.println(count);
}

void TC0_Handler() {
  if(digitalRead(3) == LOW)
  {
    count++;
  }
  // We need to get the status to clear it and allow the interrupt to fire again
  TC_GetStatus(TC0, 0);
}




void backup() {
  TC0->TC_CHANNEL[0].TC_IER = TC_IER_CPCS;
  
  const uint32_t TC = 1000/2;
  
  TC_SetRA(TC0, 0, TC / 2);
  TC_SetRC(TC0, 0, TC);
}
