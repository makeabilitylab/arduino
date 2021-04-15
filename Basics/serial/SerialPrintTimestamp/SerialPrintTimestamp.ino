void setup() {
  Serial.begin(9600); // opens serial port, sets data rate to 9600 bps
}

void loop() {
  // Get the current time since the Arduino started our program (in ms)
  unsigned long currentTimestampMs = millis();

  Serial.print("Time since Arduino started: ");
  Serial.print(currentTimestampMs);
  Serial.println(" ms");
  delay(500);
}
