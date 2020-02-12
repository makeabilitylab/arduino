/*
 * This example reads in a force resistive sensor value (from A0 using a voltage-divider circuit) and 
 * proportionally sets the brightness of an LED (hooked up to D0).
 * 
 * By Jon Froehlich for CSE590
 * http://makeabilitylab.io
 */

/* 
 * IMPORTANT: When working with the RedBear Duo, you must have this line of
 * code at the top of your program. The default state is SYSTEM_MODE(AUTOMATIC);
 * however, this puts the RedBear Duo in a special cloud-based mode that we 
 * are not using. For our purposes, set SYSTEM_MODE(SEMI_AUTOMATIC) or
 * SYSTEM_MODE(MANUAL). See https://docs.particle.io/reference/firmware/photon/#system-modes
 */
SYSTEM_MODE(MANUAL); 

const int LED_OUTPUT_PIN = D0;
const int FORCESENSOR_INPUT_PIN = A0;

// Set the min and max force sensitive resistor values
const int MIN_FORCE_VAL = 0;
const int MAX_FORCE_VAL = 4000;

void setup() {
  pinMode(LED_OUTPUT_PIN, OUTPUT);
  pinMode(FORCESENSOR_INPUT_PIN, INPUT);
  Serial.begin(9600);
}

void loop() {

  // Read the force-sensitive resistor value
  int forceVal = analogRead(FORCESENSOR_INPUT_PIN);

  // Remap the value for output. 
  int ledVal = map(forceVal, MIN_FORCE_VAL, MAX_FORCE_VAL, 0, 255);

  // The map function does not constrain output outside of the provided range
  // so, we need to make sure that things are within range for the led
  ledVal = constrain(ledVal, 0, 255);

  // Print the raw force sensor value and the converted led value so we can
  // see them in Serial Monitor and Serial Plotter
  Serial.print(forceVal);
  Serial.print(",");
  Serial.println(ledVal);

  // Write out the LED value. 
  analogWrite(LED_OUTPUT_PIN, ledVal);

  delay(100);
}




