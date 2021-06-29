
/**
 * TODO
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
const int NEOPIXEL_OUTPUT_PIN = 5;

const int PHOTORESISTOR_INPUT_PIN = A0;
const int HUE_POT_INPUT_PIN = A1;
const int BRIGHTNESS_POT_INPUT_PIN = A2;

const int MODE_SWITCH_BUTTON_INPUT_PIN = 2;
const int MODE_SWITCH_BUTTON_LED_OUTPUT_PIN = 3;

const unsigned int MAX_ANALOG_INPUT = 1023; 

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

const unsigned long MAX_HUE_VALUE = 65535; // Hue is a 16-bit number
const unsigned int MAX_BRIGHTNESS_VALUE = 255;
const unsigned int MAX_SATURATION_VALUE = 255;

const unsigned int TURN_ON_DARKNESS_THRESHOLD = 550;

// Argument 1 = Number of pixels in NeoPixel strip
// Argument 2 = Arduino pin number (most are valid)
// Argument 3 = Pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
//   NEO_RGBW    Pixels are wired for RGBW bitstream (NeoPixel RGBW products)
Adafruit_NeoPixel _neopixel(NUM_NEOPIXELS, NEOPIXEL_OUTPUT_PIN, NEO_GRB + NEO_KHZ800);

enum NiteLightMode {
  AUTO_ON_DARKNESS_LEVEL, // turns on automatically in dark
  BRIGHTNESS_INVERSE_PROPORTIONAL_TO_DARKNESS, // brightness proportional to darkness
  ALWAYS_ON,
  NUM_NITE_LIGHT_MODES,
};

NiteLightMode _nightLightMode = AUTO_ON_DARKNESS_LEVEL;
unsigned int _lastModeSwitchButtonVal = HIGH; // pull-up resistor config, so default is HIGH

void setup() {
  Serial.begin(9600);
  
  _neopixel.begin();           // Initialize NeoPixel strip object (REQUIRED)
  _neopixel.show();            // Turn OFF all pixels ASAP
  _neopixel.setBrightness(50); // Set brightness to about 1/5 (max = 255)

  pinMode(MODE_SWITCH_BUTTON_INPUT_PIN, INPUT_PULLUP);
  pinMode(MODE_SWITCH_BUTTON_LED_OUTPUT_PIN, OUTPUT);
}

void loop() {

  int buttonVal = digitalRead(MODE_SWITCH_BUTTON_INPUT_PIN);
  if(buttonVal == LOW && _lastModeSwitchButtonVal != buttonVal){
    _nightLightMode = (NiteLightMode)(_nightLightMode + 1);

    if(_nightLightMode == NUM_NITE_LIGHT_MODES){
      _nightLightMode = AUTO_ON_DARKNESS_LEVEL;
    }
  }
  _lastModeSwitchButtonVal = buttonVal;

  
  digitalWrite(MODE_SWITCH_BUTTON_LED_OUTPUT_PIN, !buttonVal);
  

  int photoCellVal = analogRead(PHOTORESISTOR_INPUT_PIN);
  Serial.println((String)photoCellVal + ", " + _nightLightMode);

  if(photoCellVal >= TURN_ON_DARKNESS_THRESHOLD || _nightLightMode == ALWAYS_ON){

    int potHueValue = analogRead(HUE_POT_INPUT_PIN);
    delay(1);
    int potBrightnessValue = analogRead(BRIGHTNESS_POT_INPUT_PIN);
  
    unsigned long hue = map(potHueValue, 0, MAX_ANALOG_INPUT, 0, MAX_HUE_VALUE);
    unsigned int saturation = 255;
    unsigned int brightness = map(potBrightnessValue, 0, MAX_ANALOG_INPUT, 0, MAX_BRIGHTNESS_VALUE);

    if(_nightLightMode == BRIGHTNESS_INVERSE_PROPORTIONAL_TO_DARKNESS){
      brightness = brightness * (photoCellVal / (float)MAX_ANALOG_INPUT);
    }
    
    uint32_t rgbColor = _neopixel.ColorHSV(hue, saturation, brightness);
    _neopixel.fill(rgbColor, 0, NUM_NEOPIXELS);
    _neopixel.show();
  }else{
    // Turn off the neopixels, which we can do either by _neopixel.fill() (no args)
    // or by calling .clear()
    _neopixel.clear();
    _neopixel.show();
  }

  delay(10);
}
