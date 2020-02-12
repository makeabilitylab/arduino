/*
 * This example attaches an interrupt to D1, which calls a the function 
 * toggle() whenever a RISING or FALLING edge is observed. Toggle turns on/off 
 * the internal RedBear Duo LED (which is on D7).
 * 
 * 
 * By Jon Froehlich for CSE590
 * http://makeabilitylab.io
 * 
 * Based on:
 *  - https://docs.particle.io/reference/firmware/photon/#interrupts
 *  - https://github.com/bjo3rn/idd-examples/blob/master/redbearduo/examples/button_interrupt/button_interrupt.ino
 */ 

/* 
 * IMPORTANT: When working with the RedBear Duo, you must have this line of
 * code at the top of your program. The default state is SYSTEM_MODE(AUTOMATIC);
 * however, this puts the RedBear Duo in a special cloud-based mode that we 
 * are not using. For our purposes, set SYSTEM_MODE(SEMI_AUTOMATIC) or
 * SYSTEM_MODE(MANUAL). See https://docs.particle.io/reference/firmware/photon/#system-modes
 */
SYSTEM_MODE(MANUAL);

// This is the internal LED pin on the Duo
const int LED_OUTPUT_PIN = D7; 

// The official Particle Photon documentation states that external interrupts
// should work on all pins except for D0 and A5, see:
// https://docs.particle.io/reference/firmware/photon/#interrupts
const int BUTTON_INPUT_PIN = D8; 

void toggle(void); // function declaration for interrupt

void setup() {
  pinMode(LED_OUTPUT_PIN, OUTPUT);
  pinMode(BUTTON_INPUT_PIN, INPUT_PULLDOWN);
  attachInterrupt(BUTTON_INPUT_PIN, toggle, CHANGE); //CHANGE, RISING or FALLING
}

void loop() {
  // intentionally empty
}

void toggle(){
  digitalWrite(LED_OUTPUT_PIN, digitalRead(BUTTON_INPUT_PIN));
}

