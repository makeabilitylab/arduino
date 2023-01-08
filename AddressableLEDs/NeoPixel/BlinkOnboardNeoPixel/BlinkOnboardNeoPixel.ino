// Blinks the on-board NeoPixel on the Adafruit nRF52840 Feather
// See: https://learn.adafruit.com/introducing-the-adafruit-nrf52840-feather/overview
//
// To program the nRF52840, I had to:
// 1. Add the board via Preferences -> Additional Board Manager URLS: 
//    https://adafruit.github.io/arduino-board-index/package_adafruit_index.json
// 2. Actually add the board in the Boards Manager. Search for nRF52840 and install 
//    the 'Adafruit nRF52 by Adafruit' (I installed 1.2.0)
// 3. Install the Adafruit Neopixel library. Open the Library Manager and search for 
//    'neopixel' then select 'Adafruit NeoPixel by Adafruit' (I installed version 1.10.7)
// 
// Based on https://web.cecs.pdx.edu/~gerry/class/feather_sense/on-board/blinkNeoPixel/
//
// By Jon E. Froehlich
// @jonfroehlich
// http://makeabilitylab.io
// All source code here: https://github.com/makeabilitylab/arduino

#include <Adafruit_NeoPixel.h>    //  Library that provides NeoPixel functions

// Create a NeoPixel object called onePixel that addresses 1 pixel in pin 8
Adafruit_NeoPixel onePixel = Adafruit_NeoPixel(1, PIN_NEOPIXEL, NEO_GRB + NEO_KHZ800);

const int BLINK_DELAY = 1000;
const int NEOPIXEL_IDX = 0;

void setup()  {
  onePixel.begin();             // Start the NeoPixel object
  onePixel.clear();             // Set NeoPixel color to black (0,0,0)
  onePixel.setBrightness(20);   // Affects all subsequent settings
  onePixel.show();              // Show
}

void loop()  {

  showNeoPixelWithDelay(NEOPIXEL_IDX, 255, 0, 0, BLINK_DELAY);  // red
  showNeoPixelWithDelay(NEOPIXEL_IDX, 0, 0, 0, BLINK_DELAY);    // off
  showNeoPixelWithDelay(NEOPIXEL_IDX, 0, 255, 0, BLINK_DELAY);  // green
  showNeoPixelWithDelay(NEOPIXEL_IDX, 0, 0, 0, BLINK_DELAY);    // off
  showNeoPixelWithDelay(NEOPIXEL_IDX, 0, 0, 255, BLINK_DELAY);  // blue
  showNeoPixelWithDelay(NEOPIXEL_IDX, 0, 0, 0, BLINK_DELAY);    // off
  showNeoPixelWithDelay(NEOPIXEL_IDX, 255, 0, 255, BLINK_DELAY);// purple
  showNeoPixelWithDelay(NEOPIXEL_IDX, 0, 0, 0, BLINK_DELAY);    // off
  showNeoPixelWithDelay(NEOPIXEL_IDX, 0, 255, 255, BLINK_DELAY);// teal
  showNeoPixelWithDelay(NEOPIXEL_IDX, 0, 0, 0, BLINK_DELAY);    // off
}

void showNeoPixelWithDelay(int neoPixelIndex, int red, int green, int blue, int delayInMs){
  onePixel.setPixelColor(neoPixelIndex, red, green, blue);   
  onePixel.show();                        
  delay(delayInMs);
}