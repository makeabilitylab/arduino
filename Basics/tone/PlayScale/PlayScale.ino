/**
 * Plays the C Scale using tone:
 * https://www.arduino.cc/reference/en/language/functions/advanced-io/tone/
 *
 * See also the OLED variant: PlayScaleWithOLED.ino
 * 
 * By Jon E. Froehlich
 * @jonfroehlich
 * http://makeabilitylab.io
 * 
 */

// Key frequencies for middle C (C4) octave scale
// See: https://en.wikipedia.org/wiki/Piano_key_frequencies
// See also: https://github.com/arduino/arduino-examples/blob/main/examples/02.Digital/toneMelody/pitches.h
#define NOTE_C4 262
#define NOTE_D4 294
#define NOTE_E4 330
#define NOTE_F4 349
#define NOTE_G4 392
#define NOTE_A4 440
#define NOTE_B4 494
#define NOTE_C5 523

// Setup C scale vars
const int NUM_NOTES_IN_SCALE = 8;
const int C_SCALE_FREQ[NUM_NOTES_IN_SCALE] = { NOTE_C4, NOTE_D4, NOTE_E4, NOTE_F4, NOTE_G4, NOTE_A4, NOTE_B4, NOTE_C5 }; 
const char C_SCALE_CHARS[NUM_NOTES_IN_SCALE] = { 'C', 'D', 'E', 'F', 'G', 'A', 'B', 'C' }; 

const int TONE_OUTPUT_PIN = 8; // Change this depending on where you put your speaker
const int DELAY_BETWEEN_NOTE_CHANGE = 1000; // In milliseconds

void setup() {
  // Setup tone output pin
  pinMode(TONE_OUTPUT_PIN, OUTPUT);
}

void loop() {
  // Plays the middle C scale
  for(int i = 0; i < NUM_NOTES_IN_SCALE; i++){

    // The tone function generates a square wave with the specified freq
    // and a 50% duty cycle. See:
    // https://www.arduino.cc/reference/en/language/functions/advanced-io/tone/
    tone(TONE_OUTPUT_PIN, C_SCALE_FREQ[i]);
    delay(DELAY_BETWEEN_NOTE_CHANGE);
  }
}
