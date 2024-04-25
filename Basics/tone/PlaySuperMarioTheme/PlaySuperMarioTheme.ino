/**
 * Plays the Super Mario main theme and the Underworld theme
 * Based on code by Dipto Pratyaksa, which seems to also
 * derive from the Arduino example: toneMelody.ino
 * 
 * By Jon E. Froehlich
 * @jonfroehlich
 * http://makeabilitylab.io
 * 
 */

#include "Notes.h"
#include "MarioMusic.h"

const int TONE_OUTPUT_PIN = 8;

void setup() {
  Serial.begin(9600);
  pinMode(TONE_OUTPUT_PIN, OUTPUT);
}

void loop() {
  Serial.println("Playing Mario Main Theme!");
  int arraySize = sizeof(mario_main_theme_melody) / sizeof(int);
  playMusic(mario_main_theme_melody, mario_main_theme_tempo, arraySize);
  Serial.println("Playing mario_underworld_melody!");
  arraySize = sizeof(mario_underworld_melody) / sizeof(int);
  playMusic(mario_underworld_melody, mario_underworld_tempo, arraySize);

  delay(1000);
}

void playMusic(const int* melody, const int* tempo, const int arraySize){ 
  Serial.println((String)"The size of the array is: " + arraySize);
  delay(100);

  for (int thisNote = 0; thisNote < arraySize; thisNote++) {
    // to calculate the note duration, take one second divided by the note type.
    //e.g. quarter note = 1000 / 4, eighth note = 1000/8, etc.
    int noteDuration = 1000 / tempo[thisNote];
    tone(TONE_OUTPUT_PIN, melody[thisNote], noteDuration);

    Serial.println((String)"Playing freq: " + melody[thisNote] + " for " + noteDuration + " ms");

    // to distinguish the notes, set a minimum time between them.
    // the note's duration + 30% seems to work well:
    int pauseBetweenNotes = noteDuration * 1.30;
    delay(pauseBetweenNotes);

    // stop the tone playing:
    noTone(TONE_OUTPUT_PIN);
  }
}
