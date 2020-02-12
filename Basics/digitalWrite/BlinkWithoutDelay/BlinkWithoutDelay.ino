/*
 * Simply turns on and off pin 3 once a second without using delay()
 * This is useful because a delay prevents us from reading new input
 * 
 * By Jon Froehlich
 * http://makeabilitylab.io
 * 
 * Adapted from the official Arduino Blink example:
 *   File -> Examples -> 02. Digital -> BlinkWithoutDelay
 *   
 * Also available here:
 *   http://www.arduino.cc/en/Tutorial/BlinkWithoutDelay
 */

const int LED_OUTPUT_PIN = 3;
const int INTERVAL_IN_MS = 1000; // interval at which to blink (in milliseconds)

// Generally, you should use "unsigned long" for variables that hold time
// The value will quickly become too large for an int to store
unsigned long _lastToggledTimestampMs = 0; // tracks the last time the LED was updated
int _ledState = LOW; // will toggle between LOW and HIGH

// The setup function runs once when you press reset or power the board
void setup() {
  // Because pins 0 - 13 can either be input or output, we must specify
  // how we're using the pin by using pinMode. In this case, we want to
  // control an LED, so set the pin to OUTPUT
  pinMode(LED_OUTPUT_PIN, OUTPUT);
}

// The loop function runs over and over again forever
void loop() {

  unsigned long currentTimestampMs = millis();

  if (currentTimestampMs - _lastToggledTimestampMs >= INTERVAL_IN_MS) {
    _lastToggledTimestampMs = currentTimestampMs;
    _ledState = _ledState == HIGH ? LOW : HIGH;
    digitalWrite(LED_OUTPUT_PIN, _ledState);
  }
  
}
