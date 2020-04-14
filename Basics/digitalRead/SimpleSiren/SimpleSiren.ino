/*
 * Demonstrates how to make a simple siren using a piezo
 * 
 * By Jon E. Froehlich
 * @jonfroehlich
 * http://makeabilitylab.io
 * 
 * For a walkthrough and circuit diagram, see:
 * https://makeabilitylab.github.io/physcomp/arduino/piano
 * 
 * Note: Use of the tone() function will interfere with PWM output on pins 3 and 11 
 * (on boards other than the Mega). See:
 * https://www.arduino.cc/reference/en/language/functions/advanced-io/tone/
 * 
 */
const int OUTPUT_PIEZO_PIN = 9; // hook up one piezo leg to GND, other leg to Pin 9
const int OUTPUT_LED_PIN = LED_BUILTIN; // we'll flash an LED on/off with the sound
const int SOUND_DURATION_MS = 500; // duration to play each siren part

void setup() {
  pinMode(OUTPUT_PIEZO_PIN, OUTPUT);
  pinMode(OUTPUT_LED_PIN, OUTPUT);
}

void loop() {
  
  // tone() generates a square wave of the specified frequency 
  // (and 50% duty cycle) on a pin. 
  tone(OUTPUT_PIEZO_PIN, 392);
  digitalWrite(OUTPUT_LED_PIN, HIGH);
  delay(SOUND_DURATION_MS);
  
  tone(OUTPUT_PIEZO_PIN, 262);
  digitalWrite(OUTPUT_LED_PIN, LOW);
  delay(SOUND_DURATION_MS);
}
