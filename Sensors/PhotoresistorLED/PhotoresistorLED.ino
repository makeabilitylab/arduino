/*
 * This example reads in a photocell value (from A0 using a voltage-divider circuit) and 
 * proportionally sets the brightness of an LED (hooked up to 3).
 * 
 * By Jon E. Froehlich
 * @jonfroehlich
 * http://makeabilitylab.io
 * 
 * For a walkthrough and circuit diagram, see:
 * https://makeabilitylab.github.io/physcomp/sensors/photoresistors
 * 
 */

const int LED_OUTPUT_PIN = 3;
const int PHOTOCELL_INPUT_PIN = A0;

// Set the min and max photocell values (this will be based on
// the brightness of your environment and the size of the voltage-divider
// resistor that you selected). So, the best way to set these values
// is to view the photocellVal in the Serial Monitor or Serial Plotter
// under different expected lighting conditions and observe the values
// The comments below assume that the photoresistor is in the R2 position
// in the voltage divider

// light level threshold to begin turning on LED (LED should be off until 200 threshold reached)
const int MIN_PHOTOCELL_VAL = 200; 

// max darkness level (LED should be fully on for anything 800+)
const int MAX_PHOTOCELL_VAL = 800; 
const boolean PHOTOCELL_IS_R2_IN_VOLTAGE_DIVIDER = true; // set false if photocell is R1

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

  if(PHOTOCELL_IS_R2_IN_VOLTAGE_DIVIDER == false){
    // We need to invert the LED (it should be brighter when environment is darker)
    // This assumes the photocell is Rtop in the voltage divider
    ledVal = 255 - ledVal;
  }

  // Print the raw photocell value and the converted led value (e,g., for Serial 
  // Console and Serial Plotter)
  Serial.print(photocellVal);
  Serial.print(",");
  Serial.println(ledVal);

  // Write out the LED value. 
  analogWrite(LED_OUTPUT_PIN, ledVal);

  delay(100); //read from the photocell ~10 times/sec
}
