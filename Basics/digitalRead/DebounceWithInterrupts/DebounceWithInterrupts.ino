/*
 * This code demonstrates debouncing without delays (using timestamps for state tracking)
 * 
 * WARNING: this code is incomplete.
 * 
 * See:
 *  - https://youtu.be/CRJUdf5TTQQ
 *  - https://electronics.stackexchange.com/questions/165044/understanding-interrupts-and-software-button-debounce
 * 
 * By Jon E. Froehlich
 * @jonfroehlich
 * http://makeabilitylab.io
 * 
 */

// The digital pins usable for interrupts:
//  - Arduino Uno, Nano, and Mini: 2, 3
//  - Leondardo, Micro, and other 32u4 boards: 0, 1, 2, 3, 7
// See: https://www.arduino.cc/reference/en/language/functions/external-interrupts/attachinterrupt/
const int BUTTON_INPUT_PIN = 2;
const int LED_OUTPUT_PIN = 3;
const unsigned long DEBOUNCE_WINDOW_MILLISECONDS = 20;
const unsigned long DEBOUNCE_WINDOW_MICROSECONDS = DEBOUNCE_WINDOW_MILLISECONDS * 1000;

volatile unsigned long _diff = 0;

volatile unsigned long _buttonStateChangeTimestamp = 0;
volatile byte _savedButtonVal = LOW; // assume pull-down resistor setup

void setup() {
  pinMode(BUTTON_INPUT_PIN, INPUT);
  pinMode(LED_OUTPUT_PIN, OUTPUT);
  //Serial.begin(9600);

  // The attachInterrupt function takes three parameters:
  //  - The digital pin to interrupt
  //  - A pointer to the interrupt service routine to call when the interrupt occurs.
  //  - A condition in which to trigger the interrupt. Can be LOW, RISING, FALLING, CHANGE.
  attachInterrupt(digitalPinToInterrupt(BUTTON_INPUT_PIN), buttonInterrupt, CHANGE);
}

/**
 * This function is an interrupt service routine (ISR) called automatically by the microcontroller
 * based on the attachInterrupt method above. In this case, it's called whenever the input 
 * on the BUTTON_INPUT_PIN goes from LOW to HIGH or HIGH to LOW
 * 
 * ISR routines take no parameters and return nothing.
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
  // Only use micros at very beginning of ISR (https://arduino.stackexchange.com/a/22237)
  unsigned long interruptTimestamp = micros();
  byte buttonVal = digitalRead(BUTTON_INPUT_PIN);

  _diff = (interruptTimestamp - _buttonStateChangeTimestamp);
  if(_diff >= DEBOUNCE_WINDOW_MICROSECONDS){
    // There's been no change for DEBOUNCE_WINDOW_MICROSECONDS, so let's read 
    _savedButtonVal = buttonVal;
    _buttonStateChangeTimestamp = interruptTimestamp;
  }

}

void loop() {
  // Write out HIGH or LOW
  digitalWrite(LED_OUTPUT_PIN, _savedButtonVal);
//  byte buttonVal = digitalRead(BUTTON_INPUT_PIN);
//  Serial.print(_savedButtonVal);
//  Serial.print(" ");
//  Serial.println(buttonVal);
//
  Serial.print(micros());
  Serial.print(" ");
  Serial.print(_buttonStateChangeTimestamp);
  Serial.print(" ");
  Serial.print(_diff);
  Serial.print(" ");
  Serial.print(DEBOUNCE_WINDOW_MICROSECONDS);
  Serial.print(" ");
  Serial.println((long)(_diff - DEBOUNCE_WINDOW_MICROSECONDS));
  delay(50);
}
