/**
 * Plays the C Scale using tone and prints the note and frequency to the screen.
 * See:
 * https://www.arduino.cc/reference/en/language/functions/advanced-io/tone/
 *
 * There is also a non-OLED variant in this repo: PlayScale.ino
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

const int NUM_NOTES_IN_SCALE = 8;
const int C_SCALE_FREQ[NUM_NOTES_IN_SCALE] = { NOTE_C4, NOTE_D4, NOTE_E4, NOTE_F4, NOTE_G4, NOTE_A4, NOTE_B4, NOTE_C5 }; 
const char C_SCALE_CHARS[NUM_NOTES_IN_SCALE] = { 'C', 'D', 'E', 'F', 'G', 'A', 'B', 'C' }; 

// Change this depending on where you put your speaker
const int TONE_OUTPUT_PIN = 8;
const int DELAY_BETWEEN_NOTE_CHANGE = 1000; // In milliseconds

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET     4 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 _display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

void setup() {
  
  // Setup tone output pin
  pinMode(TONE_OUTPUT_PIN, OUTPUT);

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!_display.begin(SSD1306_SWITCHCAPVCC, 0x3D)) { // Address 0x3D for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }

  // Setup display
  _display.clearDisplay();
  _display.setTextSize(1);      // Normal 1:1 pixel scale
  _display.setTextColor(SSD1306_WHITE); // Draw white text
  _display.setCursor(0, 0);     // Start at top-left corner
}

void loop() {
  // Plays the middle C scale
  for(int i = 0; i < NUM_NOTES_IN_SCALE; i++){
    // The tone function generates a square wave with the specified freq
    // and a 50% duty cycle. See:
    // https://www.arduino.cc/reference/en/language/functions/advanced-io/tone/
    tone(TONE_OUTPUT_PIN, C_SCALE_FREQ[i]);
    drawNote(i, C_SCALE_FREQ, C_SCALE_CHARS, NUM_NOTES_IN_SCALE);
    delay(DELAY_BETWEEN_NOTE_CHANGE);
  }
}

/**
 * @brief Displays a musical note and its frequency on the screen.
 * 
 * This function takes an index for a musical scale, retrieves the corresponding
 * note character and frequency from the provided arrays, and displays them on
 * the screen. If the note is the last in the scale, it prefixes the note with
 * "High ". The note is displayed centered and boldly, with its frequency in
 * smaller text below.
 * 
 * @param scaleIndex The index of the note in the scale arrays.
 * @param scaleFreqs The array of musical note frequencies.
 * @param scaleChars The array of musical note characters.
 * @param numNotes The number of notes in the scale arrays.
 */
void drawNote(int scaleIndex, const int* scaleFreqs, const char* scaleChars, int numNotes){
  _display.clearDisplay();

  int16_t x1, y1;
  uint16_t textWidth, textHeight;
  _display.setTextSize(3);
  String strNote = (String)scaleChars[scaleIndex];

  if(scaleIndex == numNotes - 1){ 
    strNote = (String)"High " + strNote; 
  }

  // Draw the note centered boldly on the screen
  _display.setTextSize(3);      
  _display.getTextBounds(strNote, 0, 0, &x1, &y1, &textWidth, &textHeight);
  uint16_t yText = _display.height() / 2 - textHeight + 4;
  uint16_t xText = _display.width() / 2 - textWidth / 2;
  _display.setCursor(xText, yText);
  _display.print(strNote);
  
  // Draw the freq of the note in smaller text below the note
  yText += textHeight + 4;
  _display.setTextSize(1); 
  String strFreq = (String)scaleFreqs[scaleIndex] + " Hz";
  _display.getTextBounds(strFreq, 0, 0, &x1, &y1, &textWidth, &textHeight);
  xText = _display.width() / 2 - textWidth / 2;
  _display.setCursor(xText, yText);
  _display.print(strFreq);
  _display.display();
}
