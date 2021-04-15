/*
 * Simplest possible example to demonstrate use of serial protocol.
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
  // Initialize the serial port
  // https://www.arduino.cc/reference/en/language/functions/communication/serial/begin/
  Serial.begin(9600); // opens serial port, sets data rate to 9600 bps
}

void loop() {
  // Print out "Hello world!" 
  // https://www.arduino.cc/reference/en/language/functions/communication/serial/println/
  Serial.println("Hello world!");

  // Delay by 500ms
  delay(500);
}
