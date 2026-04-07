/**
 * Plays the Imperial March from Star Wars (by John Williams).
 * Also turns on LED_BUILTIN when a note is played (for multimodal feedback)
 *
 * This version also plots the frequency to serial
 * 
 * https://www.arduino.cc/reference/en/language/functions/advanced-io/tone/
 * 
 * Note: Use of the tone() function will interfere with PWM output on pins 3 and 11 
 * (on boards other than the Mega).
 * 
 * By Jon E. Froehlich
 * @jonfroehlich
 * http://makeabilitylab.io
 * 
 */

// Key frequencies for middle C (C4) octave scale
// See: https://en.wikipedia.org/wiki/Piano_key_frequencies
// See also: https://github.com/arduino/arduino-examples/blob/main/examples/02.Digital/toneMelody/pitches.h
#define NOTE_C4  262
#define NOTE_D4  294
#define NOTE_E4  330
#define NOTE_F4  349
#define NOTE_G4  392
#define NOTE_GS4 415
#define NOTE_A4  440
#define NOTE_B4  494
#define NOTE_C5  523
#define NOTE_E5  659
#define NOTE_F5  698

const int BUZZER_PIN = 9;

// The Imperial March - Main Theme (two phrases)
int melody[] = {
  // Phrase 1: The iconic opening
  NOTE_A4, NOTE_A4, NOTE_A4, NOTE_F4, NOTE_C5, 
  NOTE_A4, NOTE_F4, NOTE_C5, NOTE_A4,
  
  // Phrase 2: The response (up the octave)
  NOTE_E5, NOTE_E5, NOTE_E5, NOTE_F5, NOTE_C5, 
  NOTE_GS4, NOTE_F4, NOTE_C5, NOTE_A4
};

// Note durations as divisors of DURATION_BASE:
//   2 = half, 4 = quarter, 6 = dotted eighth, 8 = eighth, 16 = sixteenth
int noteDurations[] = {
  4, 4, 4, 6, 16, 
  4, 6, 16, 2,
  
  4, 4, 4, 6, 16, 
  4, 6, 16, 2
};

const int NUM_NOTES = sizeof(melody) / sizeof(melody[0]);

// Base value for duration calculation, derived from tempo.
// At 104 BPM: quarter note = 60000/104 ≈ 577ms, and 2308/4 = 577.
const int DURATION_BASE = 2308;

// Pause between notes as a multiplier of note duration.
// 0.3 means 30% of the note length itself
const float NOTE_GAP_FACTOR = 0.30;

void setup() {
  // Setup serial to graph the freq
  Serial.begin(9600);

  for (int i = 0; i < NUM_NOTES; i++) {
    // Calculate note duration from the base tempo value
    // e.g., quarter note = 2308/4 = 577ms, dotted eighth = 2308/6 ≈ 385ms
    int duration = DURATION_BASE / noteDurations[i];
    tone(BUZZER_PIN, melody[i], duration);
    digitalWrite(LED_BUILTIN, HIGH);
    Serial.println(melody[i]); // start point
    delay(duration); // music
    Serial.println(melody[i]); // end point

    // Pause between notes
    Serial.println(0); // start point of rest
    int pauseBetweenNotes = duration * NOTE_GAP_FACTOR;
    digitalWrite(LED_BUILTIN, LOW);
    delay(pauseBetweenNotes); // no music
    Serial.println(0); // end point of rest

    noTone(BUZZER_PIN);
  }
}

void loop() {
  // Melody plays once in setup(), nothing to do here
}
