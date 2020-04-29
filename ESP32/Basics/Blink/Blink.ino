/*
 * Simply turns on and off GPIO Pin 21 once a second. 
 * 
 * By Jon E. Froehlich
 * @jonfroehlich
 * http://makeabilitylab.io
 * 
 * For a walkthrough and circuit diagram, see:
 * https://makeabilitylab.github.io/physcomp/esp32/led-blink
 * 
 */

// The pin numbering on the Huzzah32 is a bit strange
// See pin diagram here: https://makeabilitylab.github.io/physcomp/esp32/
const int LED_OUTPUT_PIN = 21;

void setup() {
  pinMode(LED_OUTPUT_PIN, OUTPUT);
}

void loop() {
  digitalWrite(LED_OUTPUT_PIN, HIGH);   // turn LED on (3.3V)
  delay(1000);                          // delay is in milliseconds; so wait one second
  digitalWrite(LED_OUTPUT_PIN, LOW);    // turn LED off (0V)
  delay(1000);                          // wait for a second
}
