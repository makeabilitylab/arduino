/*
 * This example demonstrates the importance of debouncing buttons by tracking both
 * raw transitions from LOW to HIGH on the input pin D8 as well as debounced
 * transitions from LOW to HIGH. Without debouncing, you'll observe that
 * the raw number of button presses quickly tracks a large number of transient
 * LOW to HIGH transitions due to the conductive materials vibrating against
 * each other when the button is initially pressed.
 * 
 * By Jon Froehlich for CSE590
 * http://makeabilitylab.io
 * 
 * This code is partially based on https://www.arduino.cc/en/Tutorial/Debounce
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

int _numRawButtonPresses = 0; //tracks raw num of button presses
int _numDebouncedButtonPresses = 0; //tracks debounced num of button presses
int _prevRawButtonVal = LOW; // defaults to low because of pull-down resistor
int _prevDebouncedButtonVal = LOW; //defaults to low
int _ledState = LOW; //current state of LED

// the following variables are unsigned longs because the time, measured in
// milliseconds, will quickly become a bigger number than can be stored in an int.
unsigned long _buttonStateChangeTimestamp = 0;  // the last time the output pin was toggled
unsigned long _debounceDelayMs = 50;    // the debounce time; increase if the output flickers

void setup() {
  pinMode(LED_OUTPUT_PIN, OUTPUT);
  pinMode(BUTTON_INPUT_PIN, INPUT_PULLDOWN); // use the RedBear Duo internal pull-down resistor
  Serial.begin(9600);
}

void loop() {

  // Read the button value. It will be HIGH when pressed and
  // LOW when not pressed
  int buttonVal = digitalRead(BUTTON_INPUT_PIN);

  // Track raw number of button presses. We only track transitions from 
  // LOW to HIGH (as that's the definition of a button press)
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
  Serial.print(_numRawButtonPresses);
  Serial.print("\t");
  Serial.print(_numDebouncedButtonPresses);
  Serial.print("\t");
  Serial.println(0); // write zero to force y-axis scale on Serial Plotter
  

  // Write out HIGH or LOW
  digitalWrite(LED_OUTPUT_PIN, _ledState);

  _prevRawButtonVal = buttonVal;
  
  // We eliminate the delay here so we can investigate debouncing issues
  // We want to read from the buttons as fast as possible
  // delay(100);
}




