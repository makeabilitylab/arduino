/*
 * Reads in the x, y, and z accelerometer values from the ADXL335 and prints the following CSV
 * to serial: timestamp, x, y, z.
 * 
 * See wiring diagram here: 
 * https://learn.adafruit.com/adafruit-analog-accelerometer-breakouts/arduino-wiring
 * 
 * To calibrate the ADXL335, read this tutorial:
 * https://learn.adafruit.com/adafruit-analog-accelerometer-breakouts/calibration-and-programming
 * 
 * By Jon E. Froehlich
 * @jonfroehlich
 * http://makeabilitylab.io
 */

const int ACCELX_INPUT = A0; // accel x is hooked up to A0
const int ACCELY_INPUT = A1; // accel y is hooked up to A0
const int ACCELZ_INPUT = A2; // accel z is hooked up to A0

void setup() {
  Serial.begin(9600); // to print values to the screen
}

void loop() {

  // the timestamp is the number of milliseconds since bootup
  // it must be an unsigned long (or risk overflow)
  unsigned long arduinoTimestamp = millis(); // https://www.arduino.cc/reference/en/language/functions/time/millis/
  
  int accelX = analogRead(ACCELX_INPUT);
  delay(1); // recall that we want to delay by 1ms between consecutive analogReads
  int accelY = analogRead(ACCELY_INPUT);
  delay(1); // recall that we want to delay by 1ms between consecutive analogReads
  int accelZ = analogRead(ACCELZ_INPUT);

  // Print values to serial
  Serial.print(arduinoTimestamp);
  Serial.print(",");
  Serial.print(accelX);
  Serial.print(",");
  Serial.print(accelY);
  Serial.print(",");
  Serial.print(accelZ);
  Serial.println();

  delay(10);
}
