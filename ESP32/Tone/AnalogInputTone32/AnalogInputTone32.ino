/**
 * Demonstrates how to use the Tone32 class with durations. Hook up a sensor like a pot
 * to A1 and then walk up and down the C scale. If you have an OLED, try out
 * this version as well: AnalogInputTone32WithOLED
 * 
 * ## BACKGROUND ##
 * 
 * The Arduino tone() function relies on timer interrupts to precisely control the frequency 
 * of a PWM waveform:
 * https://github.com/arduino/ArduinoCore-avr/blob/master/cores/arduino/Tone.cpp
 * 
 * This is completely different on the ESP32. So, we need a different approach. Fortunately,
 * there is some basic infrastructure in the arduino-esp32 library to help. Specifically,
 * in esp32-hal-ledc:
 * https://github.com/espressif/arduino-esp32/blob/master/cores/esp32/esp32-hal-ledc.h
 * https://github.com/espressif/arduino-esp32/blob/master/cores/esp32/esp32-hal-ledc.c
 * 
 * Here, there are a few things of interest. First:
 *   typedef enum {
 *     NOTE_C, NOTE_Cs, NOTE_D, NOTE_Eb, NOTE_E, NOTE_F, NOTE_Fs, NOTE_G, NOTE_Gs, NOTE_A, NOTE_Bb, NOTE_B, NOTE_MAX
 *  } note_t;
 * 
 * Which just nicely sets up named indices into the method:
 *   ledcWriteNote(uint8_t chan, note_t note, uint8_t octave){ 
 * 
 * So, we can call ledcWriteNote(<pwm_channel>, NOTE_C, 4); to play middle C (C4)
 * 
 * We can also just call ledcWriteTone(uint8_t chan, double freq) to play a raw
 * frequency at that PWM channel.
 * 
 * By Jon E. Froehlich
 * @jonfroehlich
 * http://makeabilitylab.io
 * 
 * 
 */

#include <Tone32.hpp>

const int NUM_NOTES_IN_SCALE = 8;
const note_t C_SCALE[NUM_NOTES_IN_SCALE] = { NOTE_C, NOTE_D, NOTE_E, NOTE_F, NOTE_G, NOTE_A, NOTE_B, NOTE_C }; 
const int C_SCALE_OCTAVES[NUM_NOTES_IN_SCALE]  = { 4, 4, 4, 4, 4, 4, 4, 5 };
const char C_SCALE_CHARS[NUM_NOTES_IN_SCALE] = { 'C', 'D', 'E', 'F', 'G', 'A', 'B', 'C' }; 
note_t _lastNote = NOTE_C;

// Change this depending on where you connect your piezo buzzer
const int TONE_OUTPUT_PIN = 26;

// Change this depending on where you connect your input
const int SENSOR_INTPUT_PIN = A1;

// The ESP32 has 16 channels which can generate 16 independent waveforms
// We'll just choose PWM channel 0 here
const int TONE_PWM_CHANNEL = 0; 

Tone32 _tone32(TONE_OUTPUT_PIN, TONE_PWM_CHANNEL);
const int PLAY_NOTE_DURATION_MS = 500;
const int MAX_ANALOG_VAL = 4095;

void setup() {
  //Serial.begin(115200);
}

void loop() {
  
  int sensorVal = analogRead(SENSOR_INTPUT_PIN);
  int scaleIndex = map(sensorVal, 0, MAX_ANALOG_VAL, 0, NUM_NOTES_IN_SCALE - 1);

  // Just walk up or down scale based on sensorVal position
  note_t note = C_SCALE[scaleIndex];
  int octave = C_SCALE_OCTAVES[scaleIndex];
  if(_lastNote != note){
     _tone32.playNote(note, octave, PLAY_NOTE_DURATION_MS);
  }

  // IMPORTANT: Unlike the regular Arduino tone function, which uses timer interrupts
  // for tracking time and automatically turning off PWM waves after the duration
  // interval passes, we use "polling". So, you must call update() to turn off
  // the sound automatically after the play duration expires
  _tone32.update();

  // Track the last note (we only play a note on a note change)
  // Yes, that means in this simple demo that we can't repeat the same
  // note twice consecutively!
  _lastNote = note;

  //Serial.println((String)"sensorVal: " + sensorVal + " note: " + note + " scaleIndex: " + scaleIndex);
}
