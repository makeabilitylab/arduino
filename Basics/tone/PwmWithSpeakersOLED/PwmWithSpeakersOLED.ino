/**
 * Demonstrates that we can use analogOut's PWM wave to drive a speaker
 * but it will just be stuck at the same frequency.
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

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET     4 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 _display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Change this depending on where you put your speaker
// It needs to be hooked up to a PWM pin. We assume Leonardo for this
// demonstration
const int SPEAKER_OUTPUT_PIN = 5;

// The Leonardo has 7 PWM pins most of which run at 490 Hz
// but two (3 and 11) run at 980 Hz
const int NUM_PWM_PINS = 7;
const int PWM_PINS_ARDUINO_LEONARDO [] =  {  3,   5,   6,   9,  10,  11,  13};
const int PWM_FREQS_ARDUINO_LEONARDO [] = {980, 490, 490, 490, 490, 980, 490};
const int MAX_ANALOG_OUT = 255;
int _indexOfSpeakerOutputPin = -1;

void setup() {
  
  // Setup tone output pin
  pinMode(SPEAKER_OUTPUT_PIN, OUTPUT);

  for(int i = 0; i < NUM_PWM_PINS; i++){
    if(SPEAKER_OUTPUT_PIN == PWM_PINS_ARDUINO_LEONARDO[i]){
      _indexOfSpeakerOutputPin = i;
    }
  }

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

  // Write out wave form
  int dutyCycle = 128; // 255 is 100% duty cycle
  analogWrite(SPEAKER_OUTPUT_PIN, dutyCycle);
  drawFreqAndDutyCycle(PWM_FREQS_ARDUINO_LEONARDO[_indexOfSpeakerOutputPin], dutyCycle);
}

void loop() {
  // Empty on purpose
}

void drawFreqAndDutyCycle(int freq, int analogOutVal){
  _display.clearDisplay();

  int16_t x1, y1;
  uint16_t textWidth, textHeight;
  _display.setTextSize(3);
  String strFreq = (String)(freq) + " Hz";

  // Draw the freq 
  _display.setTextSize(2);      
  _display.getTextBounds(strFreq, 0, 0, &x1, &y1, &textWidth, &textHeight);
  uint16_t yText = _display.height() / 2 - textHeight - 2;
  uint16_t xText = _display.width() / 2 - textWidth / 2;
  _display.setCursor(xText, yText);
  _display.print(strFreq);

  // Draw the duty cycle
  _display.setTextSize(1);
  yText += textHeight + 4;
  float dutyCycle = analogOutVal / (float)MAX_ANALOG_OUT * 100;
  
  String strDutyCycle = String(dutyCycle, 0) + "% duty cycle";
  _display.getTextBounds(strDutyCycle, 0, 0, &x1, &y1, &textWidth, &textHeight);
  xText = _display.width() / 2 - textWidth / 2;
  _display.setCursor(xText, yText);
  _display.print(strDutyCycle);
  _display.display();
}
