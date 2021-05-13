/**
 * Demonstrates how to play arbitrary tones on the ESP32 using an analog input
 * to change the tone frequency. We also have a version that outputs the 
 * tone freq on an attached OLED display: AnalogInputToneWithOLED.ino
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
 * Circuit diagram and walkthrough here:
 * https://makeabilitylab.github.io/physcomp/esp32/pot-fade
 * 
 */

// Change this depending on where you put your piezo buzzer
const int TONE_OUTPUT_PIN = 26;

// Change this depending on where you hook up your sensor
const int SENSOR_INPUT_PIN = A1;

// The ESP32 has 16 channels which can generate 16 independent waveforms
// We'll just choose PWM channel 0 here
const int TONE_PWM_CHANNEL = 0; 

const int MIN_FREQ = 32;
const int MAX_FREQ = 1500;
const int MAX_ANALOG_VAL = 4095;

void setup() {
  Serial.begin(115200);
  ledcAttachPin(TONE_OUTPUT_PIN, TONE_PWM_CHANNEL);
}

void loop() {
  
  int sensorVal = analogRead(SENSOR_INPUT_PIN);
  int pwmFreq = map(sensorVal, 0, MAX_ANALOG_VAL, MIN_FREQ, MAX_FREQ);
  
  // The ledcWriteTone signature: double ledcWriteTone(uint8_t chan, double freq)
  // See: https://github.com/espressif/arduino-esp32/blob/master/cores/esp32/esp32-hal-ledc.c
  ledcWriteTone(TONE_PWM_CHANNEL, pwmFreq);

  Serial.println((String)"pwmFreq: " + pwmFreq);
  delay(100);
}
