/**
 * Sets a static rainbow across the RGB LEDs.
 * 
 * Wiring:
 *  - NeoPixel DIN  -> Pin 2
 *  - NeoPixel VCC  -> 5V
 *  - NeoPixel GND  -> GND
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

// Argument 1 = Number of pixels in NeoPixel _ledStrip
// Argument 2 = Arduino pin number
// Argument 3 = Pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
//   NEO_RGBW    Pixels are wired for RGBW bitstream (NeoPixel RGBW products)
Adafruit_NeoPixel _ledStrip(NUM_LEDS, LED_PIN, NEO_GRB + NEO_KHZ800);

void setup() {
  _ledStrip.begin();
  _ledStrip.setBrightness(50);  // Keep it gentle on the eyes!

  // 65536 total hues. Dividing by NUM_LEDS gives us equal spacing across the color wheel.
  long hueStep = 65536L / NUM_LEDS; 

  for(int i = 0; i < NUM_LEDS; i++) {
    uint16_t hue = i * hueStep;

    // Saturation and Value are 8-bit (0-255). We use max for both here.
    // Generate and store the raw HSV color
    uint32_t rawColor = _ledStrip.ColorHSV(hue, 255, 255);

    // Gamma correction prevents colors from washing out by compensating for our eye's high sensitivity to dim light.
    uint32_t correctedColor = _ledStrip.gamma32(rawColor);

    // Set the pixel
    _ledStrip.setPixelColor(i, correctedColor); 
  }

  _ledStrip.show();  // Don't forget this!
}

void loop() {
  // Nothing to do — the colors persist until changed
}