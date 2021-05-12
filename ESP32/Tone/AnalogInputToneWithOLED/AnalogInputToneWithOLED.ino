/**
 * Demonstrates how to play arbitrary tones on the ESP32 using an analog input
 * to change the tone frequency. This version also outputs the played tone freq
 * on an attached OLED display using i2c. For a version without the OLED, see 
 * AnalogInputTone.
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
 */

// #include <SPI.h> // Comment out when using i2c
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// Change this depending on where you put your piezo buzzer
const int TONE_OUTPUT_PIN = 26;

const int SENSOR_INTPUT_PIN = A1;

// The ESP32 has 16 channels which can generate 16 independent waveforms
// We'll just choose PWM channel 0 here
const int TONE_PWM_CHANNEL = 0; 

const int MIN_FREQ = 32;
const int MAX_FREQ = 1500;
const int MAX_ANALOG_VAL = 4095;

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET     4 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 _display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

void setup() {
  Serial.begin(115200);
  ledcAttachPin(TONE_OUTPUT_PIN, TONE_PWM_CHANNEL);

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!_display.begin(SSD1306_SWITCHCAPVCC, 0x3D)) { // Address 0x3D for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }

  _display.clearDisplay();
  _display.setTextSize(3);      // Normal 1:1 pixel scale
  _display.setTextColor(SSD1306_WHITE); // Draw white text
  _display.setCursor(0, 0);     // Start at top-left corner
}

void loop() {
  
  int sensorVal = analogRead(SENSOR_INTPUT_PIN);
  int pwmFreq = map(sensorVal, 0, MAX_ANALOG_VAL, MIN_FREQ, MAX_FREQ);
  
  // The ledcWriteTone signature: double ledcWriteTone(uint8_t chan, double freq)
  // See: https://github.com/espressif/arduino-esp32/blob/master/cores/esp32/esp32-hal-ledc.c
  ledcWriteTone(TONE_PWM_CHANNEL, pwmFreq);

  Serial.println((String)"pwmFreq: " + pwmFreq);

  _display.clearDisplay();
  drawFreq(pwmFreq);
  _display.display();
  
  delay(100);
}

void drawFreq(int pwmFreq){
  int16_t x1, y1;
  uint16_t textWidth, textHeight;
  _display.setTextSize(3);
  String strPwmFreq = (String)pwmFreq + " Hz";
  _display.getTextBounds(strPwmFreq, 0, 0, &x1, &y1, &textWidth, &textHeight);
  uint16_t yText = _display.height() / 2 - textHeight /2;
  uint16_t xText = _display.width() / 2 - textWidth / 2;
  _display.setCursor(xText, yText);
  _display.print(strPwmFreq);
}
