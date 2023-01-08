/**
 * Cross fades through hue values. The speed of the cross fade is determined by
 * the variable _hueStep and the amount of delay.
 * 
 * See the NeoPixel library:
 * https://learn.adafruit.com/adafruit-neopixel-uberguide/arduino-library-use
 * 
 * By Jon E. Froehlich
 * @jonfroehlich
 * http://makeabilitylab.io
 *
 */

#include <Adafruit_NeoPixel.h>

const int NUM_NEOPIXELS = 7;
const int NEOPIXEL_PIN_OUTPUT = 2;

const unsigned long MAX_HUE_VALUE = 65535; // Hue is a 16-bit number
const unsigned int MAX_BRIGHTNESS_VALUE = 255;
const unsigned int MAX_SATURATION_VALUE = 255;
const unsigned int BRIGHTNESS_VALUE = MAX_BRIGHTNESS_VALUE;

unsigned long _hue = 0;
const unsigned long _hueStep = 50;

// Argument 1 = Number of pixels in NeoPixel strip
// Argument 2 = Arduino pin number (most are valid)
// Argument 3 = Pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
//   NEO_RGBW    Pixels are wired for RGBW bitstream (NeoPixel RGBW products)
Adafruit_NeoPixel _neopixel(NUM_NEOPIXELS, NEOPIXEL_PIN_OUTPUT, NEO_GRB + NEO_KHZ800);

void setup() {
  _neopixel.begin();           // Initialize NeoPixel strip object (REQUIRED)
  _neopixel.show();            // Turn OFF all pixels ASAP
  _neopixel.setBrightness(50); // Set brightness to about 1/5 (max = 255)
}

void loop() {
  // - Hue is expressed as a 16-bit number. Starting from 0 for red, this increments 
  //   first toward yellow (around 65536/6, or 10922 give or take a bit), and on through 
  //   green, cyan (at the halfway point of 32768), blue, magenta and back to red. In your 
  //   own code, you can allow any hue-related variables to overflow or underflow and they’ll 
  //   “wrap around” and do the correct and expected thing, it’s really nice
  //
  // - Saturation determines the intensity or purity of the color…this is an 8-bit number 
  //   ranging from 0 (no saturation, just grayscale) to 255 (maximum saturation, pure hue). 
  //   In the middle, you’ll start to get sort of pastel tones.
  //
  //  - Value determines the brightness of a color…it’s also an 8-bit number ranging from 0 
  //   (black, regardless of hue or saturation) to 255 (maximum brightness)
  // https://learn.adafruit.com/adafruit-neopixel-uberguide/arduino-library-use#hsv-hue-saturation-value-colors-dot-dot-dot-3024464-41

  const int saturation = MAX_SATURATION_VALUE;
  const int brightness = BRIGHTNESS_VALUE;
  uint32_t rgbColor = _neopixel.ColorHSV(_hue, saturation, brightness);
  
  _neopixel.fill(rgbColor, 0, NUM_NEOPIXELS);
  _neopixel.show();

  _hue += _hueStep;
  delay(10);
}
