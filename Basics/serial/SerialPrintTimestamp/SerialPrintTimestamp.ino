/*
 * Shows how to print out variables using multiple print statements.
 * Specifically, prints out the time (in milliseconds) since the Arduino was
 * turned on.
 * 
 * For more on this, see:
 * https://makeabilitylab.github.io/physcomp/arduino/inside-arduino.html
 * 
 * References
 * - https://www.arduino.cc/en/reference/serial
 * 
 * By Jon E. Froehlich
 * @jonfroehlich
 * http://makeabilitylab.io
 * 
 */
void setup() {
  Serial.begin(9600); // opens serial port, sets data rate to 9600 bps
}

void loop() {
  // Get the current time since the Arduino started our program (in ms)
  unsigned long currentTimestampMs = millis();

  // Use multiple print statements to format output
  Serial.print("Time since Arduino started: ");
  Serial.print(currentTimestampMs);
  Serial.println(" ms");
  delay(500);
}
