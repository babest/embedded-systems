void setup() {
  // put your setup code here, to run once:
  pinMode(3, INPUT);
  pinMode(7, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  digitalWrite(7, digitalRead(3));
  delay(1000);
}
