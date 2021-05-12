/**
 * Plays a scale on ESP32 and outputs current note on OLED
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

// The ESP32 has 16 channels which can generate 16 independent waveforms
// We'll just choose PWM channel 0 here
const int TONE_PWM_CHANNEL = 0; 

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET     4 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 _display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

void setup() {
  ledcAttachPin(TONE_OUTPUT_PIN, TONE_PWM_CHANNEL);

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!_display.begin(SSD1306_SWITCHCAPVCC, 0x3D)) { // Address 0x3D for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }

  _display.clearDisplay();
  _display.setTextSize(1);      // Normal 1:1 pixel scale
  _display.setTextColor(SSD1306_WHITE); // Draw white text
  _display.setCursor(0, 0);     // Start at top-left corner
}

void loop() {
  // Plays the middle C scale
  ledcWriteNote(TONE_PWM_CHANNEL, NOTE_C, 4);
  drawNote("C");
  delay(500);
  
  ledcWriteNote(TONE_PWM_CHANNEL, NOTE_D, 4);
  drawNote("D");
  delay(500);
  
  ledcWriteNote(TONE_PWM_CHANNEL, NOTE_E, 4);
  drawNote("E");
  delay(500);
  
  ledcWriteNote(TONE_PWM_CHANNEL, NOTE_F, 4);
  drawNote("F");
  delay(500);
  
  ledcWriteNote(TONE_PWM_CHANNEL, NOTE_G, 4);
  drawNote("G");
  delay(500);
  
  ledcWriteNote(TONE_PWM_CHANNEL, NOTE_A, 4);
  drawNote("A");
  delay(500);
  
  ledcWriteNote(TONE_PWM_CHANNEL, NOTE_B, 4);
  drawNote("B");
  delay(500);
  
  ledcWriteNote(TONE_PWM_CHANNEL, NOTE_C, 5);
  drawNote("HIGH C");
  delay(500);
  
  // ledcDetachPin(TONE_OUTPUT_PIN);
}

void drawNote(String strNote){
  _display.clearDisplay();
  int16_t x1, y1;
  uint16_t textWidth, textHeight;
  _display.setTextSize(3);      
  _display.getTextBounds(strNote, 0, 0, &x1, &y1, &textWidth, &textHeight);
  uint16_t yText = _display.height() / 2 - textHeight / 2;
  uint16_t xText = _display.width() / 2 - textWidth / 2;
  _display.setCursor(xText, yText);
  _display.print(strNote);
  _display.display();
}
