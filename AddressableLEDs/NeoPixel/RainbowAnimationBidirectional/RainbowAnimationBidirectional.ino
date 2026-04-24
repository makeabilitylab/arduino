/**
 * Cross fades through hue values and reverses. The speed of the cross fade is 
 * determined by the variable _hueStep and the amount of delay.
 * 
 * See:
 * https://makeabilitylab.github.io/physcomp/advancedio/addressable-leds.html
 * 
 * By Professor Jon E. Froehlich
 * Director, Makeability Lab: https://makeabilitylab.cs.washington.edu/
 * Physical Computing Textbook: https://makeabilitylab.github.io/physcomp/
 *
 */

#include <Adafruit_NeoPixel.h>

const int LED_PIN = 2;
const int NUM_LEDS = 8;
const uint32_t MAX_HUE = 65536; // Full circle (360 degrees) in 16-bit hue

// Argument 1 = Number of pixels in NeoPixel _ledStrip
// Argument 2 = Arduino pin number
// Argument 3 = Pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
//   NEO_RGBW    Pixels are wired for RGBW bitstream (NeoPixel RGBW products)
Adafruit_NeoPixel _ledStrip(NUM_LEDS, LED_PIN, NEO_GRB + NEO_KHZ800);

// Use signed types so negative values behave predictably during comparison
int32_t _hueStep = 128;
int32_t _firstPixelHue = 0;

void setup() {
  _ledStrip.begin();
  _ledStrip.setBrightness(50);
  _ledStrip.show();
}

void loop() {
  for (int i = 0; i < _ledStrip.numPixels(); i++) {
    // Offset each pixel's hue to spread the rainbow across the _ledStrip
    uint32_t pixelHue = _firstPixelHue + (i * MAX_HUE / _ledStrip.numPixels());
    
    // ColorHSV accepts a 16-bit hue; gamma32 provides more natural color transitions
    _ledStrip.setPixelColor(i, _ledStrip.gamma32(_ledStrip.ColorHSV(pixelHue)));
  }
  _ledStrip.show();
  
  _firstPixelHue += _hueStep;

  // Reverse direction at boundaries
  if (_firstPixelHue >= (int32_t)MAX_HUE) {
    _firstPixelHue = MAX_HUE;
    _hueStep *= -1;
  } else if (_firstPixelHue <= 0) {
    _firstPixelHue = 0;
    _hueStep *= -1;
  }
  
  delay(10); // ~100 fps
}