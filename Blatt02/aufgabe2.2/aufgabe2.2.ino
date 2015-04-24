
const int pwmPin = 10;    // switch input
const int motorA1Pin = 11;    // H-bridge leg 1 (pin 2, 1A)
const int motorA2Pin = 12;    // H-bridge leg 2 (pin 7, 2A)
const int enablePin = 13;    // H-bridge enable pin

const int CLOCKWISE = 0;
const int CCLOCKWISE = 1;
int motor1speed = 0;

void setup() {
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
    analogWrite(pwmPin, 200);
}

void loop() {
    motorPowerUp(CLOCKWISE, 255, 5000);
    motorStop(CLOCKWISE);
    delay(1000);
    motorPowerUp(CCLOCKWISE, 255, 5000);
    motorStop(CCLOCKWISE);
    delay(1000);
}


void motorOperate(int direction, int speed) {
  if(direction == CLOCKWISE) {
    digitalWrite(motorA1Pin, LOW);
    digitalWrite(motorA2Pin, HIGH);
  } else {
    digitalWrite(motorA1Pin, HIGH);
    digitalWrite(motorA2Pin, LOW);
  }
  
  motor1speed=speed;
  analogWrite(pwmPin, motor1speed);
}

void motorPowerUp(int direction, int maxSpeed, int duration) {
  int delayMs = duration/(maxSpeed-motor1speed);
  
  while(motor1speed < maxSpeed) {
    motorOperate(direction, motor1speed+1);
    delay(delayMs);
  }
}

void motorPowerDown(int direction, int lowSpeed, int duration) {
  int delayMs = duration/(motor1speed-lowSpeed);
  
  while(lowSpeed < motor1speed) {
    motorOperate(direction, motor1speed-1);
    delay(delayMs);
  }
}

void motorStop(int direction) {
  motorPowerDown(direction, 0, 1000);
}
