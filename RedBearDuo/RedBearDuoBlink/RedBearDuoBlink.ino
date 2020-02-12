/*
 * Simply turns on and off D0 once a second
 * By Jon Froehlich for CSE590
 * http://makeabilitylab.io
 * 
 * Adapted from the official RedBear Duo Blink example accessible in the 
 * Arduino IDE once the ReadBear Duo library is installed:
 *   File -> RedBear_Duo -> Basics -> Blink
 */

/* 
 * IMPORTANT: When working with the RedBear Duo, you must have this line of
 * code at the top of your program. The default state is SYSTEM_MODE(AUTOMATIC);
 * however, this puts the RedBear Duo in a special cloud-based mode that we 
 * are not using. For our purposes, set SYSTEM_MODE(SEMI_AUTOMATIC) or
 * SYSTEM_MODE(MANUAL). See https://docs.particle.io/reference/firmware/photon/#system-modes
 */
SYSTEM_MODE(MANUAL); 

const int RED_LED_OUTPUT_PIN = D0;

void setup() {
  pinMode(RED_LED_OUTPUT_PIN, OUTPUT);
}

void loop() {
  
  // Write out a HIGH value (3.3V in this case as the RedBear Duo is a 3.3V board)
  digitalWrite(RED_LED_OUTPUT_PIN, HIGH);

  // Delay 1000ms (or 1 s)
  delay(1000);

  // Write out a LOW value (0V in this case)
  digitalWrite(RED_LED_OUTPUT_PIN, LOW);

  // Wait another second
  delay(1000);
}
