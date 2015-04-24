
// Counter
int counterPin3High = 0;
int counterPin3Low= 0;
int counterPin5High = 0;
int counterPin5Low = 0;

// store laste state of pins
int pin3LastState = HIGH;
int pin5LastState = HIGH;

const int pwmPin = 10;        // switch input
const int motorA1Pin = 11;    // H-bridge leg 1 (pin 2, 1A)
const int motorA2Pin = 12;    // H-bridge leg 2 (pin 7, 2A)
const int enablePin = 13;     // H-bridge enable pin

const int CLOCKWISE = 0;
const int CCLOCKWISE = 1;

const int MOTOR_STATE_NORMAL = 0;
const int MOTOR_STATE_CHANGING_SPEED = 1;
const int MOTOR_STATE_CHANGING_DIRECTION = 2;

int motorCurrentState = MOTOR_STATE_NORMAL;

int motorCurrentSpeed = 0;
int motorDestinationSpeed = motorCurrentSpeed;
int motorCurrentRotation = CLOCKWISE;
int motorDestinationRotation = motorCurrentRotation;
int motorChangeDuration = 0;




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
  
  pinMode(3, INPUT);
  pinMode(5, INPUT);
  
  //>> Hier wird der konfigurierte Timer gestartet
  startTimer(TC0, 0, TC0_IRQn, 1000);
  startTimer(TC0, 1, TC1_IRQn, 1000);
  
  //enable H-bridge
  pinMode(enablePin, OUTPUT);
  
  // set all the other pins you're using as outputs:
  pinMode(pwmPin, OUTPUT);
  pinMode(motorA1Pin, OUTPUT);
  pinMode(motorA2Pin, OUTPUT);

  // set enablePin high so that motor can turn on:
  digitalWrite(enablePin, HIGH);
  digitalWrite(motorA1Pin, LOW);
  digitalWrite(motorA2Pin, HIGH);
    
  Serial.begin(9600);
  
  setMotorRotation(CLOCKWISE);
  setMotorSpeed(255, 1000);
}




void loop() {
  
  calculateMotorChange();
  
}




void calculateMotorChange() {
  
  // The motor wants to change the speed
  if (motorCurrentState == MOTOR_STATE_CHANGING_SPEED) {
    
    // The motor has not yet reached its destination Speed
    if (motorCurrentSpeed != motorDestinationSpeed) {
      
      if (motorDestinationSpeed > motorCurrentSpeed) {
        motorCurrentSpeed++;
      }
      else {
        motorCurrentSpeed--;
      }
      
      analogWrite(pwmPin, motorCurrentSpeed);
      
      delay(motorChangeDuration);
      
    }
    else {
       // we are done.
       motorCurrentState = MOTOR_STATE_NORMAL;
    }
    
  }
  // The motor wants to change the direction
  else if (motorCurrentState == MOTOR_STATE_CHANGING_DIRECTION) {
    
    // If the direction is reached
    if (motorCurrentRotation == motorDestinationRotation) {
      
      // If the final speed is not reached yet
      if (motorCurrentSpeed != motorDestinationSpeed) {
        motorCurrentSpeed++;
        analogWrite(pwmPin, motorCurrentSpeed);
        delay(motorChangeDuration);
      }
      // If the final speed has been reached
      else {
        // we are done.
        motorCurrentState = MOTOR_STATE_NORMAL;
      }
      
    }
    // If the direction is still wrong
    else {
      
      // If the motor is already stopped
      if(motorCurrentSpeed == 0) {
        // change the direction
        motorCurrentRotation = motorDestinationRotation;
        
        if (motorCurrentRotation == CLOCKWISE) {
          digitalWrite(motorA1Pin, LOW);
          digitalWrite(motorA2Pin, HIGH);
        }
        else if (motorCurrentRotation == CCLOCKWISE) {
          digitalWrite(motorA1Pin, HIGH);
          digitalWrite(motorA2Pin, LOW);
        }
      }
      // If the motor needs to be stopped first
      else {
        motorCurrentSpeed--;
        analogWrite(pwmPin, motorCurrentSpeed);
        delay(motorChangeDuration);
      }
      
    }
  }
  
}




void setMotorSpeed(int speed, int duration) {
  
  if (motorCurrentState != MOTOR_STATE_NORMAL) return;
  
  if (motorDestinationSpeed == speed) return;
  
  motorCurrentState = MOTOR_STATE_CHANGING_SPEED;
  
  motorDestinationSpeed = speed;
  motorChangeDuration = duration / abs(motorCurrentSpeed - motorDestinationSpeed);
}

void setMotorRotation(int rotation) {
  
  if (motorCurrentState != MOTOR_STATE_NORMAL) return;
  
  if (motorCurrentRotation == rotation) return;
  
  motorCurrentState = MOTOR_STATE_CHANGING_DIRECTION;
  
  motorDestinationRotation = rotation;
  motorDestinationSpeed = motorCurrentSpeed;
  motorChangeDuration = 1000 / motorCurrentSpeed;
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
  
  int destinationRotation = (motorCurrentRotation == CLOCKWISE) ? CCLOCKWISE : CLOCKWISE;
  setMotorRotation(destinationRotation);
}




void pin5High() {
  //Serial.println("Pin 5 High");
}

void pin5Low() {
  Serial.println("Pin 5 Low");
  
  if (motorCurrentSpeed == 0) {
    setMotorSpeed(255, 1000);
  }
  else {
    setMotorSpeed(0, 1000);
  }
}




