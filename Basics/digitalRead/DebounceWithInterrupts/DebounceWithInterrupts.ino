/*
 * This code demonstrates debouncing without delays (using timestamps for state tracking)
 * 
 * WARNING: this code is incomplete.
 * 
 * See:
 *  - https://youtu.be/CRJUdf5TTQQ
 * 
 * By Jon E. Froehlich
 * @jonfroehlich
 * http://makeabilitylab.io
 * 
 */

const int BUTTON_INPUT_PIN = 2;
const int LED_OUTPUT_PIN = 3;
const int DEBOUNCE_WINDOW_MILLISECONDS = 40;
const int DEBOUNCE_WINDOW_MICROSECONDS = DEBOUNCE_WINDOW_MILLISECONDS * 1000;

volatile unsigned long _timestampLastInterrupt = LOW;

int _prevRawButtonVal = LOW;
int _debouncedButtonVal = LOW;
unsigned long _buttonStateChangeTimestamp = 0;  // the last time the input pin was toggled

void setup() {
  pinMode(BUTTON_INPUT_PIN, INPUT);
  pinMode(LED_OUTPUT_PIN, OUTPUT);

  // The attachInterrupt function takes three parameters:
  //  - The digital pin to interrupt
  //  - A pointer to the interrupt service routine to call when the interrupt occurs.
  //  - A condition in which to trigger the interrupt. Can be LOW, RISING, FALLING, CHANGE.
  attachInterrupt(digitalPinToInterrupt(BUTTON_INPUT_PIN), buttonInterrupt, CHANGE);
}

/**
 * This function is an interrupt service routine (ISR) called automatically by the microcontroller
 * based on the attachInterrupt method above. It must take no parameters and return nothing.
 * 
 * Generally, an ISR should be as short and fast as possible. If your sketch uses multiple ISRs, only 
 * one can run at a time, other interrupts will be executed after the current one finishes in an order 
 * that depends on the priority they have.
 * 
 * Inside this function, delay() won’t work and the value returned by millis() will not increment. 
 * Serial data received while in the function may be lost. You should declare as volatile any variables that 
 * you modify within the attached function. See the section on ISRs below for more information.
 */
void buttonInterrupt(){
  unsigned long interruptTimestamp = micros();
  if(interruptTimestamp - _buttonStateChangeTimestamp >= DEBOUNCE_WINDOW_MICROSECONDS){
    // There's been no change for DEBOUNCE_WINDOW_MICROSECONDS, so let's shift
  }
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
