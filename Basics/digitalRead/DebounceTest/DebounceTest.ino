/*
 * This example demonstrates the importance of debouncing buttons by tracking both
 * raw transitions from LOW to HIGH on the input pin as well as debounced
 * transitions from LOW to HIGH. Without debouncing, you'll observe that
 * the raw number of button presses quickly tracks a large number of transient
 * LOW to HIGH transitions due to the conductive materials vibrating against
 * each other when the button is initially pressed.
 * 
 * For more information, see:
 * https://makeabilitylab.github.io/physcomp/arduino/debouncing.html
 * 
 * By Jon E. Froehlich
 * @jonfroehlich
 * http://makeabilitylab.io
 * 
 * This code is partially based on https://www.arduino.cc/en/Tutorial/Debounce
 */

const int BUTTON_INPUT_PIN = 2;
const int LED_OUTPUT_PIN = 3;

int _numRawButtonPresses = 0; //tracks raw num of button presses
int _numDebouncedButtonPresses = 0; //tracks debounced num of button presses
int _prevRawButtonVal = LOW; // defaults to low because of pull-down resistor
int _prevDebouncedButtonVal = LOW; //defaults to low
int _ledState = LOW; //current state of LED

// the following variables are unsigned longs because the time, measured in
// milliseconds, will quickly become a bigger number than can be stored in an int.
unsigned long _buttonStateChangeTimestamp = 0;  // the last time the input pin was toggled
unsigned long _debounceDelayMs = 50;    // the debounce time; increase if the output flickers

void setup() {
  pinMode(LED_OUTPUT_PIN, OUTPUT);
  pinMode(BUTTON_INPUT_PIN, INPUT); // hook up button with a pull-down resistor
  Serial.begin(9600);
}

void loop() {

  // Read the button value. It will be HIGH when pressed and
  // LOW when not pressed
  int buttonVal = digitalRead(BUTTON_INPUT_PIN);

  // Track raw number of button presses. We only track transitions from 
  // LOW to HIGH (as that's the definition of a button *press*)
  if(buttonVal == HIGH && buttonVal != _prevRawButtonVal){
    _numRawButtonPresses++;
  }

  // If the button state has changed since the last reading, grab timestamp
  if(buttonVal != _prevRawButtonVal){
    _buttonStateChangeTimestamp = millis();
  }

  if((millis() - _buttonStateChangeTimestamp) >= _debounceDelayMs){
    // If we're here, then the button state hasn't changed for at least _debounceDelayMs
    // So, we must be in a steady state

    // We only count transitions from LOW to HIGH (as that's a button press)
    if(buttonVal == HIGH && buttonVal != _prevDebouncedButtonVal){
      _numDebouncedButtonPresses++;
    }
    _ledState = buttonVal;
    _prevDebouncedButtonVal = buttonVal;
  }

  // Write out tracking vals
  Serial.print("Raw:");
  Serial.print(_numRawButtonPresses);
  Serial.print(", Debounced:");
  Serial.println(_numDebouncedButtonPresses);
  
  // Write out HIGH or LOW
  digitalWrite(LED_OUTPUT_PIN, _ledState);

  _prevRawButtonVal = buttonVal;
  
  // We eliminate the delay here so we can investigate debouncing issues
  // We want to read from the buttons as fast as possible
  // delay(10);
}
