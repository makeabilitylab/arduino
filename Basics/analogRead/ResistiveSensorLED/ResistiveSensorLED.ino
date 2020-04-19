/*
 * This example reads in a variable resistor sensor value (from A0 using a voltage-divider 
 * circuit) and proportionally sets the brightness of an LED. Some example variable resisitive
 * sensors include potentiometers, force-sensisitive resistors, photosensistive resistors, 
 * strain gauages, thermistors, etc.
 * 
 * By Jon E. Froehlich
 * @jonfroehlich
 * http://makeabilitylab.io
 * 
 */

const int OUTPUT_LED_PIN = LED_BUILTIN;
const int INPUT_VAR_RESISTOR_PIN = A0;

// Set the min and max sensor values. The best way to figure out what to 
// set these values is by experimentation with the sensor using
// Serial Monitor or Serial Plotter. You will definitely need
// to change these constants depending on the sensor that you are using
const int MIN_SENSOR_VAL = 0; 
const int MAX_SENSOR_VAL = 1023;

const int DELAY_MS = 50; // how often to read from the sensor input

boolean _invertLedBrightness = false; // invert LED brightness to sensor input

void setup() {
  pinMode(OUTPUT_LED_PIN, OUTPUT);
  pinMode(INPUT_VAR_RESISTOR_PIN, INPUT);
  Serial.begin(9600);
}

void loop() {

  // Read the variable resistor sensor value
  int sensorVal = analogRead(INPUT_VAR_RESISTOR_PIN);

  // Remap the value for output. 
  int ledVal = map(sensorVal, MIN_SENSOR_VAL, MAX_SENSOR_VAL, 0, 255);

  // The map function does not constrain output outside of the provided range
  // so, we need to make sure that things are within range for the led
  ledVal = constrain(ledVal, 0, 255);

  if(_invertLedBrightness){
    // Depending on the circuit and sensor, we may want an inverted
    // relationship between LED brightness and sensor val
    ledVal = 255 - ledVal;
  }

  // Print the raw sensor value and the converted led value (e,g., for Serial Plotter)
  Serial.print(sensorVal);
  Serial.print(",");
  Serial.println(ledVal);

  // Write out the LED value. 
  analogWrite(OUTPUT_LED_PIN, ledVal);

  delay(DELAY_MS);
}
