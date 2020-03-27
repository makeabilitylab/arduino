/*
 * This example reads in a button input on D2 (with an external pull-down resistor configuration)
 * and turns on/off the embedded LED on the Arduino (at LED_BUILTIN) accordingly
 * 
 * By Jon Froehlich
 * @jonfroehlich
 * http://makeabilitylab.io
 * 
 */

const int BUTTON_INPUT_PIN = 2;
const int LED_OUTPUT_PIN = LED_BUILTIN; // change this if you want to use an external LED 

void setup() {
  pinMode(BUTTON_INPUT_PIN, INPUT);
  pinMode(LED_OUTPUT_PIN, OUTPUT);
  Serial.begin(9600); // used for debugging
}

void loop() {

  // read the button value. It will be HIGH when pressed and
  // LOW when not pressed
  int buttonVal = digitalRead(BUTTON_INPUT_PIN);

  // Write out HIGH or LOW
  digitalWrite(LED_OUTPUT_PIN, buttonVal);
  Serial.println(buttonVal);
  
  // Check for new input every 50ms (20 times a sec)
  delay(50);
}
