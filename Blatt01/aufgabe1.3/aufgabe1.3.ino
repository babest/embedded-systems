void setup() {
  // put your setup code here, to run once:
  pinMode(3, INPUT);
  pinMode(7, OUTPUT);
  digitalWrite(7, LOW);
  attachInterrupt(3, updateLight, CHANGE);
}

void loop() {
  // put your main code here, to run repeatedly:
  delay(2000);
}

void updateLight() {
  digitalWrite(7, digitalRead(3));
}
