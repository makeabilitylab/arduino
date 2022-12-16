/*
 * Simple VUMeter using an electret microphone (with built-in amp) on A0
 * and the built-in NeoPixel on the Adafruit nRF52840 Feather
 * 
* Tested on:
 *  - Adafruit nRF52840: https://learn.adafruit.com/introducing-the-adafruit-nrf52840-feather
 *
 * By Jon E. Froehlich
 * @jonfroehlich
 * http://makeabilitylab.io
 * 
 */

#include <Adafruit_NeoPixel.h>    //  Library that provides NeoPixel functions

// Create a NeoPixel object called onePixel that addresses 1 pixel in pin 8
Adafruit_NeoPixel _onePixel = Adafruit_NeoPixel(1, PIN_NEOPIXEL, NEO_GRB + NEO_KHZ800);
const int NEOPIXEL_IDX = 0;

// The nRF52840 has 10-bit ADC (0..1023)
const int MAX_ANALOG_IN = 1023;
const int MIC_INPUT_PIN = A0;
const int MAX_MIC_LEVEL = 950;

const int MIC_SAMPLE_WINDOW_MS = 50; // Sample window width in ms (50 ms = 20Hz)

const unsigned long MAX_HUE_VALUE = 65535; // Hue is a 16-bit number
const unsigned int MAX_BRIGHTNESS_VALUE = 50;
const unsigned int MAX_SATURATION_VALUE = 255;
const unsigned int BRIGHTNESS_VALUE = MAX_BRIGHTNESS_VALUE;

unsigned long _startSampleTimeMs = -1;
unsigned int _signalMax = 0;
unsigned int _signalMin = MAX_ANALOG_IN;

void setup() {
  pinMode(MIC_INPUT_PIN, INPUT);

  _onePixel.begin();             // Start the NeoPixel object
  _onePixel.clear();             // Set NeoPixel color to black (0,0,0)
  _onePixel.setBrightness(30);   // Affects all subsequent settings
  _onePixel.show();              // Show

  _startSampleTimeMs = millis();
}

void loop() {  

  // Read in current sound level from microphone
  int micLevel = analogRead(MIC_INPUT_PIN);

  if(micLevel > _signalMax){
    _signalMax = micLevel;
  }else if(micLevel < _signalMin){
    _signalMin = micLevel;
  }

  if(millis() - _startSampleTimeMs > MIC_SAMPLE_WINDOW_MS){
    unsigned int peakToPeak = _signalMax - _signalMin;

    int hueVal = map(peakToPeak, 0, MAX_MIC_LEVEL, 0, MAX_HUE_VALUE);
    int brightnessVal = map(peakToPeak,0, MAX_MIC_LEVEL, 5, MAX_BRIGHTNESS_VALUE);

    const int saturation = MAX_SATURATION_VALUE;
    const int brightness = BRIGHTNESS_VALUE;
    uint32_t rgbColor = _onePixel.ColorHSV(hueVal, saturation, brightness);
    
    // See: https://adafruit.github.io/Adafruit_NeoPixel/html/class_adafruit___neo_pixel.html
    _onePixel.fill(rgbColor, 0, 1);
    _onePixel.setBrightness(brightnessVal); 
    _onePixel.show();

    // Serial.print(_signalMin);
    // Serial.print(", ");
    // Serial.print(_signalMax);
    // Serial.print(", ");
    // Serial.print(peakToPeak);
    // Serial.print(", ");
    // Serial.println(rgbColor);

    _signalMax = 0;
    _signalMin = MAX_ANALOG_IN;
    _startSampleTimeMs = millis();
  }
}