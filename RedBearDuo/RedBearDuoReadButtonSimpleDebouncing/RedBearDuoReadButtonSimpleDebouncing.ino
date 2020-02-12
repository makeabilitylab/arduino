/*
 * This example reads in a button input on D8 with an internal pull-down resistor configuration
 * and turns on/off an LED on D0 accordingly. I've also added in some simple debouncing
 * but the approach used here is not ideal since we are blocked in a delay. For a better
 * approach, see: 
 * https://github.com/jonfroehlich/CSE590Sp2018/tree/master/L05-Arduino/RedBearDuoReadButtonDebounce
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
const int DEBOUNCE_DELAY = 50; //in milliseconds

int _lastButtonVal = LOW; //starts low because using pull-down resistor

void setup() {
  pinMode(LED_OUTPUT_PIN, OUTPUT);
  pinMode(BUTTON_INPUT_PIN, INPUT_PULLDOWN);
}

void loop() {

  // Read the button value. It will be HIGH when pressed and
  // LOW when not pressed
  int buttonVal = digitalRead(BUTTON_INPUT_PIN);

  // Wait to check the button state again
  delay(DEBOUNCE_DELAY);

  // read the button value again
  int buttonVal2 = digitalRead(BUTTON_INPUT_PIN);

  // If buttonVal and buttonVal2 are the same, then we are in steady state
  // Save a new buttonval if a transition occurred.
  if(buttonVal == buttonVal2 && _lastButtonVal != buttonVal){
    _lastButtonVal = buttonVal;
  }

  // Write out HIGH or LOW
  digitalWrite(LED_OUTPUT_PIN, _lastButtonVal);
}




