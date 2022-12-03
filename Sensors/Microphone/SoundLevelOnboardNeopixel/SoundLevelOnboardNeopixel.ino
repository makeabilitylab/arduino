/*
 * Simple VUMeter using an electret microphone (with built-in amp) on A0
 * and the built-in NeoPixel on the Adafruit nRF52840 Feather
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

const unsigned long MAX_HUE_VALUE = 65535; // Hue is a 16-bit number
const unsigned int MAX_BRIGHTNESS_VALUE = 50;
const unsigned int MAX_SATURATION_VALUE = 255;
const unsigned int BRIGHTNESS_VALUE = MAX_BRIGHTNESS_VALUE;

void setup() {
  pinMode(MIC_INPUT_PIN, INPUT);

  _onePixel.begin();             // Start the NeoPixel object
  _onePixel.clear();             // Set NeoPixel color to black (0,0,0)
  _onePixel.setBrightness(20);   // Affects all subsequent settings
  _onePixel.show();              // Show
}

void loop() {  

  // Read in current sound level from microphone
  int soundLevel = analogRead(MIC_INPUT_PIN);

  // Since microphone works peak-to-peak from 0V to the reference voltage (either 3.3V or 5V)
  // we only look at half of the amplitude from MAX_ANALOG_IN/2 to MAX_ANALOG_IN
  int hueVal = map(soundLevel, MAX_ANALOG_IN/2, MAX_ANALOG_IN, 0, MAX_HUE_VALUE);
  int brightnessVal = map(soundLevel, MAX_ANALOG_IN/2, MAX_ANALOG_IN, 0, MAX_BRIGHTNESS_VALUE);

  const int saturation = MAX_SATURATION_VALUE;
  const int brightness = BRIGHTNESS_VALUE;
  uint32_t rgbColor = _onePixel.ColorHSV(hueVal, saturation, brightness);
  
  // See: https://adafruit.github.io/Adafruit_NeoPixel/html/class_adafruit___neo_pixel.html
  _onePixel.fill(rgbColor, 0, 1);
  _onePixel.setBrightness(brightnessVal); 
  _onePixel.show();

  Serial.println(soundLevel);
}