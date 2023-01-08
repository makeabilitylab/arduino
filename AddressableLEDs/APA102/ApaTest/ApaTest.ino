/****
 * A simple blink test for the whole APA102 strip
 * 
 * This tutorial uses the popular and well-respected FastLED library. When evaluating the various
 * addressable LED strips, the FastLED devs said: "I'd recommend [the APA102 strips] over just about anything else at 
 * this point." They have a fast data rate, high refresh rate. Note that Adafruit sells these as DotStars.
 * See: https://github.com/FastLED/FastLED/wiki/Chipset-reference
 * 
 * See:
 *  - https://learn.sparkfun.com/tutorials/apa102-addressable-led-hookup-guide/all 
 *  - https://github.com/FastLED/FastLED/wiki/Basic-usage
 *  - https://github.com/FastLED/FastLED/blob/master/examples/Blink/Blink.ino
 *
 * All of the FastLED examples are here:
 * https://github.com/FastLED/FastLED/tree/master/examples
 * 
 * ---Engineering Log---
 * Jan 7, 2023: I got this to work on an Arduino Nano 33 IOT both with an external 5V supply as
 * as well as simply using the 3.3V Nano output.
 *
 * By Jon E. Froehlich
 * @jonfroehlich
 * https://makeabilitylab.github.io/physcomp/
 */

#include <FastLED.h>  // https://github.com/FastLED/FastLED

#define NUM_LEDS_IN_APASTRIP 60  // 1m strip has 60 RGB leds
#define APA102_DATA_PIN 6 // D6
#define APA102_CLOCK_PIN 10 // D10

CRGB _leds[NUM_LEDS_IN_APASTRIP];

void setup() {
  // You specify which type of addressable RGB strip we're using (APA102 vs. NEOPIXEL, for example)
  // and then other attributes like DATA_PIN, CLOCK_PIN, etc.
  // For example: FastLED.addLeds<APA102, DATA_PIN, CLOCK_PIN, RGB, DATA_RATE_MHZ(12)>(leds, NUM_LEDS);
  // I've found that our strip from Sparkfun is BGR
  FastLED.addLeds<APA102, APA102_DATA_PIN, APA102_CLOCK_PIN, BGR>(_leds, NUM_LEDS_IN_APASTRIP);
}

void loop() {
  setColor(CRGB::Blue);
  FastLED.show();
  delay(1000);

  setColor(CRGB::Black);
  FastLED.show();
  delay(1000);
  
}

void setColor(const CRGB &color) {
  for (int i = 0; i < NUM_LEDS_IN_APASTRIP; i++) {
    _leds[i] = color;
  }
}
