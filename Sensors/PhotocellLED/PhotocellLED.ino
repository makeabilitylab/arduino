/*
 * This example reads in a photocell value (from A0 using a voltage-divider circuit) and 
 * proportionally sets the brightness of an LED (hooked up to 3).
 * 
 * For tutorials, see:
 *  - https://learn.adafruit.com/photocells/using-a-photocell
 *  - https://learn.adafruit.com/adafruit-arduino-lesson-9-sensing-light/overview
 * 
 * By Jon Froehlich
 * http://makeabilitylab.io
 * 
 */

const int LED_OUTPUT_PIN = 3;
const int PHOTOCELL_INPUT_PIN = A0;

// Set the min and max photocell values (this will be based on
// the brightness of your environment and the size of the voltage-divider
// resistor that you selected). So, the best way to set these values
// is to view the photocellVal in the Serial Monitor or Serial Plotter
// under different expected lighting conditions and observe the values
const int MIN_PHOTOCELL_VAL = 500; // Photocell reading in dark
const int MAX_PHOTOCELL_VAL = 850; // Photocell reading in ambient light

void setup() {
  pinMode(LED_OUTPUT_PIN, OUTPUT);
  pinMode(PHOTOCELL_INPUT_PIN, INPUT);
  Serial.begin(9600);
}

void loop() {

  // Read the photo-sensitive resistor value. If you have the photocell resistor hooked 
  // up as Rtop in the voltage divider (that is, one leg of the photocell is connected to 5V), 
  // then higher values correspond to brightness. If you have the photocell hooked up as Rbottom 
  // in the voltage divider (that is, one leg of the photocell is connected to GND), then
  // higher values correspond to darkness.
  int photocellVal = analogRead(PHOTOCELL_INPUT_PIN);

  // Remap the value for output. 
  int ledVal = map(photocellVal, MIN_PHOTOCELL_VAL, MAX_PHOTOCELL_VAL, 0, 255);

  // The map function does not constrain output outside of the provided range
  // so, we need to make sure that things are within range for the led
  ledVal = constrain(ledVal, 0, 255);

  // We want to invert the LED (it should be brighter when environment is darker)
  // This assumes the photocell is Rtop in the voltage divider
  ledVal = 255 - ledVal;

  // Print the raw photocell value and the converted led value (e,g., for Serial Plotter)
  Serial.print(photocellVal);
  Serial.print(",");
  Serial.println(ledVal);

  // Write out the LED value. 
  analogWrite(LED_OUTPUT_PIN, ledVal);

  delay(100); //read from the photocell ~10 times/sec
}
