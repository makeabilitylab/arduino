/*
 * This example reads in a button input on D2 (with an external pull-down resistor configuration)
 * and turns on/off an LED on D3 accordingly
 * 
 * By Jon Froehlich
 * http://makeabilitylab.io
 */

const int BUTTON_INPUT_PIN = 2;
const int LED_OUTPUT_PIN = 3;

void setup() {
  pinMode(BUTTON_INPUT_PIN, INPUT);
  pinMode(LED_OUTPUT_PIN, OUTPUT);
  Serial.begin(9600);
}

void loop() {

  // read the button value. It will be HIGH when pressed and
  // LOW when not pressed
  int buttonVal = digitalRead(BUTTON_INPUT_PIN);

  // Write out HIGH or LOW
  digitalWrite(LED_OUTPUT_PIN, buttonVal);
  Serial.println(buttonVal);
  
  // Check for new input every 100ms (10 times a sec)
  delay(100);
}
