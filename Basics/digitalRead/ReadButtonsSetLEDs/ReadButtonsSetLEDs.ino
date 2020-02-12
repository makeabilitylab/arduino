/*
 * This example demonstrates the difference between a pull-down and pull-up resistor configuration
 * with digital input. There is a button hooked up to D2 with a pull-down resistor and a button
 * hooked up to D4 with a pull-up resistor. The LEDs on D3 and D5 illuminate when their respective
 * buttons are pressed.
 * 
 * By Jon Froehlich
 * http://makeabilitylab.io
 */

const int BUTTON_INPUT_PIN_PD = 2; // button with a pull-down (PD) resistor
const int LED_OUTPUT_PIN_PD = 3;   // the LED for the PD button
const int BUTTON_INPUT_PIN_PU = 4; // btton with a pull-up (PU) resistor
const int LED_OUTPUT_PIN_PU = 5;   // the LED for the PU button

void setup() {
  pinMode(BUTTON_INPUT_PIN_PD, INPUT);
  pinMode(LED_OUTPUT_PIN_PD, OUTPUT);

  pinMode(BUTTON_INPUT_PIN_PU, INPUT);
  pinMode(LED_OUTPUT_PIN_PU, OUTPUT);

  Serial.begin(9600);
}

void loop() {

  // Read the button configured with a pull-down resistor. It will be HIGH when pressed and
  // LOW when not pressed
  int buttonValPD = digitalRead(BUTTON_INPUT_PIN_PD);

  // Write out HIGH or LOW
  digitalWrite(LED_OUTPUT_PIN_PD, buttonValPD);

  // Read the button configured with a pull-up resistor. It will be LOW when pressed and
  // HIGH when not pressed
  int buttonValPU = digitalRead(BUTTON_INPUT_PIN_PU);

  // Write out HIGH or LOW. Note that we invert the button value because it's hooked up
  // to a pull-up resistor (that is, when the button is pressed, the signal goes LOW but
  // we want to illuminate our LED when the button is pressed).
  digitalWrite(LED_OUTPUT_PIN_PU, !buttonValPU);

  // Print values to serial
  Serial.print("pull-down button=");
  Serial.print(buttonValPD);
  Serial.print(" led=");
  Serial.print(buttonValPD);
  Serial.print("\tpull-up button=");
  Serial.print(buttonValPU);
  Serial.print(" led=");
  Serial.println(!buttonValPU);
  
  // Check for new input every 100ms (10 times a sec)
  delay(100);
}
