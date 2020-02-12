/*
 * This example reads in a button input on D8 (with a pull-down resistor configuration)
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

const int LED0_OUTPUT_PIN = D0;
const int LED1_OUTPUT_PIN = D1;
const int LED2_OUTPUT_PIN = D2;
const int LED3_OUTPUT_PIN = D3;

const int BUTTON_EXTPULLDOWN_INPUT_PIN = D8;
const int BUTTON_EXTPULLUP_INPUT_PIN = D9;
const int BUTTON_INTPULLDOWN_INPUT_PIN = D10;
const int BUTTON_INTPULLUP_INPUT_PIN = D11;

void setup() {
  pinMode(LED0_OUTPUT_PIN, OUTPUT);
  pinMode(LED1_OUTPUT_PIN, OUTPUT);
  pinMode(LED2_OUTPUT_PIN, OUTPUT);
  pinMode(LED3_OUTPUT_PIN, OUTPUT);
  
  pinMode(BUTTON_EXTPULLDOWN_INPUT_PIN, INPUT);
  pinMode(BUTTON_EXTPULLUP_INPUT_PIN, INPUT);
  pinMode(BUTTON_INTPULLDOWN_INPUT_PIN, INPUT_PULLDOWN);
  pinMode(BUTTON_INTPULLUP_INPUT_PIN, INPUT_PULLUP);
}

void loop() {

  // Read the button with an external pull-down resistor. The value will be HIGH when 
  // pressed and LOW when not pressed
  int buttonExtPullDownVal = digitalRead(BUTTON_EXTPULLDOWN_INPUT_PIN);

  // Read the button with an external pull-up resistor. The value will be LOW
  // when pressed and HIGH when not pressed
  int buttonExtPullUpVal = digitalRead(BUTTON_EXTPULLUP_INPUT_PIN);

  // Read the button with an internal pull-down resistor. The value will be HIGH when 
  // pressed and LOW when not pressed
  int buttonIntPullDownVal = digitalRead(BUTTON_INTPULLDOWN_INPUT_PIN);
  
  // Read the button with an internal pull-up resistor. The value will be LOW
  // when pressed and HIGH when not pressed
  int buttonIntPullUpVal = digitalRead(BUTTON_INTPULLUP_INPUT_PIN);

  // Write out HIGH or LOW
  digitalWrite(LED0_OUTPUT_PIN, buttonExtPullDownVal);
  digitalWrite(LED1_OUTPUT_PIN, buttonExtPullUpVal);
  digitalWrite(LED2_OUTPUT_PIN, buttonIntPullDownVal);
  digitalWrite(LED3_OUTPUT_PIN, buttonIntPullUpVal);
  
  // Check for new input every 100ms
  delay(100);
}




