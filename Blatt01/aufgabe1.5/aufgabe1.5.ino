//#define USE_INTERRUPT


void setup() {
  // put your setup code here, to run once:
  pinMode(3, INPUT);
  pinMode(5, INPUT);
  pinMode(7, OUTPUT);
  digitalWrite(7, LOW);

  #ifdef USE_INTERRUPT
  attachInterrupt(3, increase, FALLING);
  attachInterrupt(5, decrease, FALLING);
  #endif

  Serial.begin(9600);
}

int counter = 0;

void loop() {
  // put your main code here, to run repeatedly:
  
  #ifndef USE_INTERRUPT
  // decrease
  if(digitalRead(5) == LOW)
  {
    decrease();
  }
  // increase
  if(digitalRead(3) == LOW)
  {
    increase();
  }
  #endif
  
  counter = counter%256;
  analogWrite(7, counter);
  
  Serial.println(counter);
  
  delay(10);
}

void increase() {
  if (counter < 255)
  {
    counter++;
  }
}
void decrease() {
  if (counter > 0)
  {
    counter--;
  }
}
