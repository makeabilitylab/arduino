void setup() {
  Serial.begin(9600); // opens serial port, sets data rate to 9600 bps
}

void loop() {
  Serial.println("Hello world!");
  delay(500);
}
