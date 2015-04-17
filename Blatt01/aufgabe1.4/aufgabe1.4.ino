void setup() {
  // put your setup code here, to run once:
  pinMode(3, INPUT);
  pinMode(7, OUTPUT);
  digitalWrite(7, LOW);

  Serial.begin(9600);
}

int counter = 0;

void loop() {
  // put your main code here, to run repeatedly:
  analogWrite(7, counter);
  Serial.println(counter);
  
  counter = (++counter)%256;
  delay(50);
}
