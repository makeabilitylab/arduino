/**
 * Plays the Imperial March from Star Wars (by John Williams)
 * Advanced version with BPM-based timing and extended melody.
 * 
 * This version uses tempo-aware duration constants for more accurate rhythm,
 * includes a more complete transcription of the main theme, and supports
 * replay via a button press. Also sets the LED_BUILTIN HIGH while music 
 * is being played
 * 
 * Circuit:
 *  - Piezo buzzer on pin 9
 *  - Momentary pushbutton on pin 7 (INPUT_PULLUP, other leg to GND)
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

// Note frequencies (Hz)
// See: https://en.wikipedia.org/wiki/Piano_key_frequencies
#define NOTE_REST 0
#define NOTE_C4   262
#define NOTE_D4   294
#define NOTE_DS4  311
#define NOTE_E4   330
#define NOTE_F4   349
#define NOTE_G4   392
#define NOTE_GS4  415
#define NOTE_A4   440
#define NOTE_AS4  466
#define NOTE_B4   494
#define NOTE_C5   523
#define NOTE_CS5  554
#define NOTE_D5   587
#define NOTE_DS5  622
#define NOTE_E5   659
#define NOTE_F5   698
#define NOTE_FS5  740
#define NOTE_G5   784
#define NOTE_GS5  831
#define NOTE_A5   880

// Pin assignments
const int BUZZER_PIN = 9;
const int BUTTON_PIN = 7;

// Tempo and duration constants
// The Imperial March is typically performed around 104 BPM
const int BPM = 104;
const int QUARTER     = 60000 / BPM;          // ~577ms
const int EIGHTH      = QUARTER / 2;          // ~288ms
const int SIXTEENTH   = QUARTER / 4;          // ~144ms
const int DOTTED_QTR  = QUARTER + EIGHTH;     // ~865ms
const int DOTTED_8TH  = EIGHTH + SIXTEENTH;   // ~433ms
const int HALF        = QUARTER * 2;          // ~1154ms
const int WHOLE       = QUARTER * 4;          // ~2308ms

// Fraction of note duration to remain silent between notes.
// Creates articulation — set to 0 for fully legato.
const float NOTE_GAP_FACTOR = 0.15;

// The Imperial March - Extended Main Theme
// Transcription covers the two main A phrases and the B phrase.
int melody[] = {
  // --- A1: The iconic opening ---
  NOTE_A4,  NOTE_A4,  NOTE_A4,
  NOTE_F4,  NOTE_C5,
  NOTE_A4,  NOTE_F4,  NOTE_C5,
  NOTE_A4,

  // --- A2: Up the octave ---
  NOTE_E5,  NOTE_E5,  NOTE_E5,
  NOTE_F5,  NOTE_C5,
  NOTE_GS4, NOTE_F4,  NOTE_C5,
  NOTE_A4,

  // --- B1: The triplet/development section ---
  NOTE_A5,  NOTE_A4,  NOTE_A4,
  NOTE_A5,  NOTE_GS5, NOTE_G5,
  NOTE_FS5, NOTE_F5,
  NOTE_FS5,
  NOTE_REST,
  NOTE_AS4,
  NOTE_DS5, NOTE_D5,  NOTE_CS5,
  NOTE_C5,  NOTE_B4,
  NOTE_C5,
  NOTE_REST,
  NOTE_F4,
  NOTE_GS4, NOTE_F4,  NOTE_GS4,
  NOTE_C5,  NOTE_A4,  NOTE_C5,
  NOTE_E5
};

// Durations in milliseconds, matched 1:1 with melody[]
int durations[] = {
  // --- A1 ---
  QUARTER,     QUARTER,     QUARTER,
  DOTTED_8TH,  SIXTEENTH,
  QUARTER,     DOTTED_8TH,  SIXTEENTH,
  HALF,

  // --- A2 ---
  QUARTER,     QUARTER,     QUARTER,
  DOTTED_8TH,  SIXTEENTH,
  QUARTER,     DOTTED_8TH,  SIXTEENTH,
  HALF,

  // --- B1 ---
  QUARTER,     DOTTED_8TH,  SIXTEENTH,
  QUARTER,     DOTTED_8TH,  SIXTEENTH,
  SIXTEENTH,   SIXTEENTH,
  EIGHTH,
  EIGHTH,       // rest
  EIGHTH,
  QUARTER,     DOTTED_8TH,  SIXTEENTH,
  SIXTEENTH,   SIXTEENTH,
  EIGHTH,
  EIGHTH,       // rest
  EIGHTH,
  QUARTER,     DOTTED_8TH,  SIXTEENTH,
  QUARTER,     DOTTED_8TH,  SIXTEENTH,
  HALF
};

const int NUM_NOTES = sizeof(melody) / sizeof(melody[0]);

void setup() {

  // Sets replay button pin to internal pullup
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  
  // Play once on power-up
  playImperialMarch();
}

void loop() {
  // Press button to replay
  if (digitalRead(BUTTON_PIN) == LOW) {
    delay(50); // simple debounce
    playImperialMarch();
    
    // Wait for button release before allowing another play
    while (digitalRead(BUTTON_PIN) == LOW) {
      delay(10);
    }
  }
}

/**
 * Plays the entire melody once.
 */
void playImperialMarch() {
  for (int i = 0; i < NUM_NOTES; i++) {
    playNote(melody[i], durations[i]);
  }
}

/**
 * Plays a single note on the buzzer with a brief gap for articulation.
 * Also sets the LED_BUILTIN HIGH while music is being played
 * 
 * @param frequency  Note frequency in Hz (0 for rest)
 * @param duration   Total time for this note slot in ms
 */
void playNote(int frequency, int duration) {
  int playTime = duration * (1.0 - NOTE_GAP_FACTOR);
  int gapTime  = duration - playTime;

  if (frequency == NOTE_REST) {
    delay(duration);
  } else {
    tone(BUZZER_PIN, frequency, playTime);
    digitalWrite(LED_BUILTIN, HIGH);
    delay(playTime);
    noTone(BUZZER_PIN);
    digitalWrite(LED_BUILTIN, LOW);
    delay(gapTime);
  }
}
