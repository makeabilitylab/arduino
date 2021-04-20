/*
 * This code demonstrates debouncing without delays (using timestamps for state tracking)
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

int _prevRawButtonVal = LOW;
int _debouncedButtonVal = LOW;
unsigned long _buttonStateChangeTimestamp = 0;  // the last time the input pin was toggled

void setup() {
  pinMode(BUTTON_INPUT_PIN, INPUT);
  pinMode(LED_OUTPUT_PIN, OUTPUT);
}

void loop() {

  // Read the button value. We assume a pull-down resistor button configuration so
  // the button will be HIGH when pressed and LOW when not pressed
  int rawButtonVal = digitalRead(BUTTON_INPUT_PIN);

  // If the button state has changed since the last reading, grab timestamp
  // This state change may be due to a legitimate user action or noise
  if(rawButtonVal != _prevRawButtonVal){
    _buttonStateChangeTimestamp = millis();
  }

  if((millis() - _buttonStateChangeTimestamp) >= DEBOUNCE_WINDOW){
    // If we're here, then the button state hasn't changed for at least DEBOUNCE_WINDOW
    // So, we must be in a steady state

    _debouncedButtonVal = rawButtonVal;
  }

  // Write out HIGH or LOW
  digitalWrite(LED_OUTPUT_PIN, _debouncedButtonVal);

  _prevRawButtonVal = rawButtonVal;
}
