// Controls a NeoPixel strand with three potentiometers:
//  1. The potentiometer on A0 (NUM_NEOPIXELS_POT_PIN) controls the number of NeoPixels to illuminate
//  2. The potentiometer on A1 (HUE_POT_PIN) controls the NeoPixel strand color
//  3. The potentiometer on A2 (BRIGHTNESS_POT_PIN) controls the NeoPixel strand brightness
//
// If you're using a 3.3V board like the Adafruit Feather series or Arduino Nano 33 IoT,
// you may need a level shifter like the 74AHCT125 (https://www.adafruit.com/product/1787)
// or the LXB0108 for even larger projects (https://www.adafruit.com/products/395).
// I was able to successfully run this code with a 30 NeoPixel strand and off USB laptop
// power without a level shifter.
//
// For more on level shifting, see:
// - https://learn.adafruit.com/neopixel-levelshifter and the YouTube video https://www.youtube.com/watch?v=y6HiDnyhRa0
// - https://learn.adafruit.com/adafruit-neopixel-uberguide/best-practices
//
// Remember Adafruit's NEOPIXEL BEST PRACTICES for most reliable operation:
// - Add 1000 uF CAPACITOR between NeoPixel strip's + and - connections.
// - MINIMIZE WIRING LENGTH between microcontroller board and first pixel.
// - NeoPixel strip's DATA-IN should pass through a 300-500 OHM RESISTOR.
// - AVOID connecting NeoPixels on a LIVE CIRCUIT. If you must, ALWAYS
//   connect GROUND (-) first, then +, then data.
// - When using a 3.3V microcontroller with a 5V-powered NeoPixel strip,
//   a LOGIC-LEVEL CONVERTER on the data line is STRONGLY RECOMMENDED.
// (Skipping these may work OK on your workbench but can fail in the field)
//
// See also:
// - strandtest: https://github.com/adafruit/Adafruit_NeoPixel/blob/master/examples/strandtest/strandtest.ino
// - strandtest_wheel: https://github.com/adafruit/Adafruit_NeoPixel/blob/master/examples/strandtest_wheel/strandtest_wheel.ino
// - strandtest_nodelay: https://github.com/adafruit/Adafruit_NeoPixel/blob/master/examples/strandtest_nodelay/strandtest_nodelay.ino
//
// By Jon E. Froehlich
// @jonfroehlich
// http://makeabilitylab.io
//

#include <Adafruit_NeoPixel.h>

const int NUM_NEOPIXELS = 30;       // Change this to match your strand length
const int NEOPIXEL_PIN_OUTPUT = 5;  // Change this to match your output pin

const int NUM_NEOPIXELS_POT_PIN = A0;
const int HUE_POT_PIN = A1;
const int BRIGHTNESS_POT_PIN = A2;

const unsigned long MAX_HUE_VALUE = 65535;  // Hue is a 16-bit number
const unsigned int MAX_BRIGHTNESS_VALUE = 255;
const unsigned int MAX_SATURATION_VALUE = 255;

// Max ADC on the NRF52840 and m0 is 12-bit (4096)
// But according to Adafruit docs, default values for ADC on both is 10-bit
// (this is for compatibility reasons with other Arduino code). So, the default
// 10-bit resolution (0..1023) with a 3.6V reference voltage, meaning every digit
// returned from the ADC = 3600mV/1024 = 3.515625mV. You can change this with:
// https://www.arduino.cc/reference/en/language/functions/zero-due-mkr-family/analogreadresolution/
// See also: https://learn.adafruit.com/adafruit-feather-sense/nrf52-adc#analog-reference-voltage-2865006
const int MAX_ANALOG_INPUT = 929;  // in practice on my NRF52840, found this to be ~929

Adafruit_NeoPixel _neopixelStrip = Adafruit_NeoPixel(NUM_NEOPIXELS, NEOPIXEL_PIN_OUTPUT,
                                                     NEO_GRB + NEO_KHZ800);

void setup() {
  //Serial.begin(115200);

  //Wait for serial port to be opened, remove this line for 'standalone' operation
  //while (!Serial) { delay(1); }

  _neopixelStrip.begin();            // Initialize NeoPixel strip object (REQUIRED)
  _neopixelStrip.setBrightness(50);  // Set brightness to about 1/5 (max = 255)
  _neopixelStrip.show();             // Turn OFF all pixels ASAP

  pinMode(NUM_NEOPIXELS_POT_PIN, INPUT);
  pinMode(HUE_POT_PIN, INPUT);
  pinMode(BRIGHTNESS_POT_PIN, INPUT);
}

void loop() {
  int potNumNeoPixelsValue = analogRead(NUM_NEOPIXELS_POT_PIN);
  delay(1);
  int potHueValue = analogRead(HUE_POT_PIN);
  delay(1);
  int potBrightnessValue = analogRead(BRIGHTNESS_POT_PIN);
  delay(1);

  // Serial.print(potNumNeoPixelsValue);
  // Serial.print(", ");
  // Serial.print(potHueValue);
  // Serial.print(", ");
  // Serial.println(potBrightnessValue);

  unsigned long hue = map(potHueValue, 0, MAX_ANALOG_INPUT, 0, MAX_HUE_VALUE);
  hue = constrain(hue, 0, MAX_HUE_VALUE);
  unsigned int saturation = 255;  // max saturation
  unsigned int brightness = map(potBrightnessValue, 0, MAX_ANALOG_INPUT, 0, MAX_BRIGHTNESS_VALUE);
  brightness = constrain(brightness, 0, MAX_BRIGHTNESS_VALUE);
  unsigned int numNeoPixelsToIlluminate = map(potNumNeoPixelsValue, 0, MAX_ANALOG_INPUT, 0, NUM_NEOPIXELS);
  numNeoPixelsToIlluminate = constrain(numNeoPixelsToIlluminate, 0, NUM_NEOPIXELS);

  // Serial.print(numNeoPixelsToIlluminate);
  // Serial.print(", ");
  // Serial.print(hue);
  // Serial.print(", ");
  // Serial.print(saturation);
  // Serial.print(", ");
  // Serial.println(brightness);

  uint32_t rgbColor = _neopixelStrip.ColorHSV(hue, saturation, brightness);
  //_neopixelStrip.clear();
  _neopixelStrip.fill(rgbColor, 0, numNeoPixelsToIlluminate);
  _neopixelStrip.fill(0, numNeoPixelsToIlluminate, NUM_NEOPIXELS); // clear others
  _neopixelStrip.show();

  delay(10);
}
