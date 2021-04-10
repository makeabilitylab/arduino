/*
 * Sends an x,y value string as 4-decimal floats between [0,1] over the serial port. Intended
 * to work with the Processing program ArduinoEtchASketch.pde and ArduinoPong.pde
 *   
 * By Jon Froehlich
 * http://makeabilitylab.io
 * 
 */

const int ANALOG_XCONTROL_PIN = A0;
const int ANALOG_YCONTROL_PIN = A1;
const int DELAY_INTERVAL = 100;
const int DECIMAL_PLACES = 4; // controls how many decimal places to send over serial

void setup() {

  // Turn on Serial so we can communicate with our Processing program
  Serial.begin(9600);   
}

void loop() {

  // read the potentiometers
  int xRawVal = analogRead(ANALOG_XCONTROL_PIN);
  int yRawVal = analogRead(ANALOG_YCONTROL_PIN);

  // normalize the values between [0, 1], which is what our Processing program expects
  float xValNormalized = xRawVal / 1023.0;
  float yValNormalized = yRawVal / 1023.0;

  // Send data to Processing program over Serial with format x, y
  Serial.print(xValNormalized, DECIMAL_PLACES);
  Serial.print(",");
  Serial.println(yValNormalized, DECIMAL_PLACES);

  delay(DELAY_INTERVAL);
}
