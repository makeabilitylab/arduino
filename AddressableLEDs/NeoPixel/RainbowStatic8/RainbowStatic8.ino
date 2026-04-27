/**
 * Sets a static rainbow across the 8 LEDs
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

// Argument 1 = Number of pixels in NeoPixel _led_ledStrip
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

  // Set each LED to a different color
  _ledStrip.setPixelColor(0, _ledStrip.Color(255, 0, 0));     // Red
  _ledStrip.setPixelColor(1, _ledStrip.Color(255, 128, 0));   // Orange
  _ledStrip.setPixelColor(2, _ledStrip.Color(255, 255, 0));   // Yellow
  _ledStrip.setPixelColor(3, _ledStrip.Color(0, 255, 0));     // Green
  _ledStrip.setPixelColor(4, _ledStrip.Color(0, 255, 255));   // Cyan
  _ledStrip.setPixelColor(5, _ledStrip.Color(0, 0, 255));     // Blue
  _ledStrip.setPixelColor(6, _ledStrip.Color(128, 0, 255));   // Purple
  _ledStrip.setPixelColor(7, _ledStrip.Color(255, 0, 128));   // Pink

  _ledStrip.show();  // Don't forget this!
}

void loop() {
  // Nothing to do — the colors persist until changed
}