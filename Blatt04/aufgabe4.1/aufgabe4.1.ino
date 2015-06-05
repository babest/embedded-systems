

// Init Vars

int buttonPin = 2;
int buttonLastState;
int counterButtonPinHigh;
int counterButtonPinLow;

int counter = 0;

const int serialSendBufferSize = 100;
const int serialReceiveBufferSize = 100;
char serialSendBuffer[serialSendBufferSize];
char serialReceiveBuffer[serialReceiveBufferSize];

int ledPin = 13;



// Programm Setup

void setup() {
  
  // start Timer for Button
  startTimer(TC0, 0, TC0_IRQn, 1000);
  
  // Set the button pin mode to input
  pinMode(buttonPin, INPUT);
  pinMode(ledPin, OUTPUT);
  
  // Enable the serials
  Serial.begin(9600);
  Serial1.begin(9600);
}



// Programm Loop

void loop() {
  flushCommand();
  receiveCommand();
}



// Button Handler

void buttonHigh() {
  Serial.println("Pin 3 High");
}

void buttonLow() {
  Serial.println("Pin 3 Low");
  sendCommand("LED_PULSE");
}



// Sending and receiving commands

void receiveCommand() {
  // read a command from the serial
  
  if (Serial1.available()) {
    memset(serialReceiveBuffer, '\0', serialReceiveBufferSize);
    
    Serial1.readBytesUntil('\n', serialReceiveBuffer, serialReceiveBufferSize);
    
    Serial.print("Receive Command: ");
    Serial.println(serialReceiveBuffer);
    
    // following lines hurt in my heart </3
    digitalWrite(ledPin, HIGH);
    delay(200);
    digitalWrite(ledPin, LOW);
    delay(200);
    digitalWrite(ledPin, HIGH);
    delay(200);
    digitalWrite(ledPin, LOW);
    delay(200);
    digitalWrite(ledPin, HIGH);
    delay(200);
    digitalWrite(ledPin, LOW);
  }
}

void sendCommand(const char* message) {
  // Add the command to the sendBuffer
  
  if (0 == strlen(serialSendBuffer)) {
    Serial.print("Send Command: ");
    Serial.println(message);
    
    strncpy(serialSendBuffer, message, serialSendBufferSize);
  }
}

void flushCommand() {
  // Send buffer to other device
  
  if (0 < strlen(serialSendBuffer)) {
    Serial.print("Flush Command: ");
    Serial.println(serialSendBuffer);
    
    Serial1.println(serialSendBuffer);
    
    memset(serialSendBuffer, '\0', serialSendBufferSize);
  }
}



// Timer setup and handler

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

void TC0_Handler() {
  // increase counter based on current pin input value
  (digitalRead(buttonPin) == HIGH) ? counterButtonPinHigh++ : counterButtonPinLow++;
  
  if (counterButtonPinHigh + counterButtonPinLow > 30) {
    
    if (counterButtonPinHigh > counterButtonPinLow && buttonLastState == LOW) {
      buttonHigh();
      buttonLastState = HIGH;
    }
    else if (counterButtonPinHigh < counterButtonPinLow && buttonLastState == HIGH) {
      buttonLow();
      buttonLastState = LOW;
    }
    
    counterButtonPinHigh = counterButtonPinLow = 0;
  }
  
  // We need to get the status to clear it and allow the interrupt to fire again
  TC_GetStatus(TC0, 0);
}

