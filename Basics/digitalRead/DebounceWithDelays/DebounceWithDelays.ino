/*
 * This code demonstrates using delays() for software debouncing
 * 
 * For more information, see:
 * https://makeabilitylab.github.io/physcomp/arduino/debouncing.html
 * 
 * By Jon E. Froehlich
 * @jonfroehlich
 * http://makeabilitylab.io
 * 
 */

const int BUTTON_INPUT_PIN = 2;
const int LED_OUTPUT_PIN = 3;
const int DEBOUNCE_WINDOW = 40; // in milliseconds

int _savedButtonVal = LOW; //starts low because using pull-down resistor

void setup() {
  pinMode(BUTTON_INPUT_PIN, INPUT);
  pinMode(LED_OUTPUT_PIN, OUTPUT);
}

void loop() {

  // Read the button value. We assume a pull-down resistor button configuration so
  // the button will be HIGH when pressed and LOW when not pressed
  int buttonVal = digitalRead(BUTTON_INPUT_PIN);

  // Wait to check the button state again
  delay(DEBOUNCE_WINDOW);

  // read the button value again
  int buttonVal2 = digitalRead(BUTTON_INPUT_PIN);

  // If buttonVal and buttonVal2 are the same, then we are in steady state
  // If this stead state value does not match our _lastButtonVal, then
  // a transition has occurred and we should save the new buttonVal
  // This works both for open-to-close transitions and close-to-open transitions
  if(buttonVal == buttonVal2 && _savedButtonVal != buttonVal){
    _savedButtonVal = buttonVal;
  }

  // Write out HIGH or LOW
  digitalWrite(LED_OUTPUT_PIN, _savedButtonVal);
}
