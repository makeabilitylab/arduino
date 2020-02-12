/*
 * This example reads in a button input on D8 (with an external pull-down resistor configuration)
 * and turns on/off an LED on D0 accordingly
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
const int BUTTON_INPUT_PIN = D8;

void setup() {
  pinMode(LED_OUTPUT_PIN, OUTPUT);
  pinMode(BUTTON_INPUT_PIN, INPUT);
}

void loop() {

  // read the button value. It will be HIGH when pressed and
  // LOW when not pressed
  int buttonVal = digitalRead(BUTTON_INPUT_PIN);

  // Write out HIGH or LOW
  digitalWrite(LED_OUTPUT_PIN, buttonVal);
  
  // Check for new input every 100ms
  delay(100);
}




