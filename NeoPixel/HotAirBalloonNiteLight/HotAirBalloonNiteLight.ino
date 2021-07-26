
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

// Potentiometer input for setting hue and brightness
const int HUE_POT_INPUT_PIN = A1;
const int BRIGHTNESS_POT_INPUT_PIN = A2;
const boolean REVERSE_POTS = true;

const int MODE_SWITCH_BUTTON_INPUT_PIN = 7;
const int MODE_SWITCH_BUTTON_LED_OUTPUT_PIN = 9;

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

// Set threshold to turn on the nightlight based on ambient light (photoresistor value)
// Threshold used by AUTO_ON_DARKNESS_LEVEL nightlight mode
const unsigned int TURN_ON_DARKNESS_THRESHOLD = 550;

// Set the threshold to turn off the night light if ambient light is too bright
// This threshold is used by the BRIGHTNESS_INVERSE_PROPORTIONAL_TO_DARKNESS mode
const unsigned int TURN_OFF_DARKNESS_THRESHOLD = 150; 

unsigned long _newStateEnteredTimestamp = 0;
const unsigned int START_FLASH_AFTER_NEW_STATE_THRESHOLD = 100;
const unsigned int FLASH_ON_MS = 400;
const unsigned int FLASH_OFF_MS = 200;
unsigned long _lastFlashTimestamp = 0;
int _flashNum = -1;

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
  AUTO_ON_DARKNESS_LEVEL, // turns on automatically in dark, brightness controlled by POT
  BRIGHTNESS_INVERSE_PROPORTIONAL_TO_DARKNESS, // brightness controlled by POT but proportional to darkness
  ALWAYS_ON,  // ignores photocell, brightness controlled by POT
  CROSS_FADE, // Cross-fades across colors
  NUM_NITE_LIGHT_MODES,
};

unsigned int _crossFadeVal = 0;
const int MIN_CROSS_FADE_STEP = 1;
const int MAX_CROSS_FADE_STEP = 300;

NiteLightMode _nightLightMode = AUTO_ON_DARKNESS_LEVEL;
unsigned int _lastModeSwitchButtonVal = HIGH; // pull-up resistor config, so default is HIGH

void setup() {
  Serial.begin(9600);
  
  _neopixel.begin();           // Initialize NeoPixel strip object (REQUIRED)
  _neopixel.show();            // Turn OFF all pixels ASAP
  _neopixel.setBrightness(50); // Set brightness to about 1/5 (max = 255)

  pinMode(NEOPIXEL_OUTPUT_PIN, OUTPUT);
  pinMode(PHOTORESISTOR_INPUT_PIN, INPUT);
  pinMode(HUE_POT_INPUT_PIN, INPUT);
  pinMode(BRIGHTNESS_POT_INPUT_PIN, INPUT);
  pinMode(MODE_SWITCH_BUTTON_INPUT_PIN, INPUT_PULLUP);
  pinMode(MODE_SWITCH_BUTTON_LED_OUTPUT_PIN, OUTPUT);
}

void loop() {

  int buttonVal = digitalRead(MODE_SWITCH_BUTTON_INPUT_PIN);
  if(buttonVal == LOW && _lastModeSwitchButtonVal != buttonVal){

    // If button just pressed, turn on button LED
    digitalWrite(MODE_SWITCH_BUTTON_LED_OUTPUT_PIN, HIGH); 

    // Increment the night light mode
    _nightLightMode = (NiteLightMode)(_nightLightMode + 1);

    if(_nightLightMode == NUM_NITE_LIGHT_MODES){
      _nightLightMode = AUTO_ON_DARKNESS_LEVEL;
    }
    
  }else if(buttonVal == HIGH && _lastModeSwitchButtonVal != buttonVal){
    // Button just released, turn off button LED
    digitalWrite(MODE_SWITCH_BUTTON_LED_OUTPUT_PIN, LOW);

    // Setup state flashing variables (to give feedback to user about which state
    // we are in). We'll flash once for night light mode 1, twice for night light mode 2, etc.
    _newStateEnteredTimestamp = millis();
    _lastFlashTimestamp = millis();
    _flashNum = (int)(_nightLightMode + 1) * 2;
    
//    Serial.println((String)"_nightLightMode=" + _nightLightMode + 
//                  " _newStateEnteredTimestamp=" + _newStateEnteredTimestamp +
//                  " _lastFlashTimestamp=" + _lastFlashTimestamp +
//                  " _flashNum=" + _flashNum);
  }
  _lastModeSwitchButtonVal = buttonVal;

  // Flash the button LED given the night light mode and flash num
  if(_flashNum >= 0 && millis() - _newStateEnteredTimestamp > START_FLASH_AFTER_NEW_STATE_THRESHOLD){
    unsigned long timeSinceLastFlash = millis() - _lastFlashTimestamp;
//    Serial.println((String)"_flashNum=" + _flashNum +
//                  " timeSinceLastFlash=" + timeSinceLastFlash);
    if(_flashNum % 2 == 0 && timeSinceLastFlash > FLASH_ON_MS){
      digitalWrite(MODE_SWITCH_BUTTON_LED_OUTPUT_PIN, LOW);
      Serial.println((String)"FLASH OFF: _nightLightMode=" + _nightLightMode +
                  " _flashNum=" + _flashNum +
                  " timeSinceLastFlash=" + timeSinceLastFlash +
                  " millis()=" + millis() + 
                  " FLASH_ON_MS=" + FLASH_ON_MS + 
                  " FLASH_OFF_MS=" + FLASH_OFF_MS);
      _lastFlashTimestamp = millis();
      _flashNum--;
    }else if(_flashNum % 2 == 1 && timeSinceLastFlash > FLASH_OFF_MS){
      digitalWrite(MODE_SWITCH_BUTTON_LED_OUTPUT_PIN, HIGH);
//      Serial.println((String)"FLASH ON: _nightLightMode=" + _nightLightMode +
//                  " _flashNum=" + _flashNum +
//                  " timeSinceLastFlash=" + timeSinceLastFlash +
//                  " millis()=" + millis() + 
//                  " FLASH_ON_MS=" + FLASH_ON_MS + 
//                  " FLASH_OFF_MS=" + FLASH_OFF_MS);
      _lastFlashTimestamp = millis();
      _flashNum--;
    }
  }

  // Read the photoresistor setup as a "darkness" configuration, see:
  // https://makeabilitylab.github.io/physcomp/sensors/photoresistors.html#using-photoresistors-with-microcontrollers
  int photoCellVal = analogRead(PHOTORESISTOR_INPUT_PIN);

  // If the photoresistor darkness threshold is surpassed or the night light mode is always on
  if((_nightLightMode == AUTO_ON_DARKNESS_LEVEL && photoCellVal >= TURN_ON_DARKNESS_THRESHOLD) || 
      _nightLightMode == BRIGHTNESS_INVERSE_PROPORTIONAL_TO_DARKNESS ||
      _nightLightMode == CROSS_FADE ||
      _nightLightMode == ALWAYS_ON){

    // Read the hue and brightness pots
    int potHueValue = analogRead(HUE_POT_INPUT_PIN);
    delay(1);
    int potBrightnessValue = analogRead(BRIGHTNESS_POT_INPUT_PIN);

    if(REVERSE_POTS){
      potHueValue = MAX_ANALOG_INPUT - potHueValue;
      potBrightnessValue = MAX_ANALOG_INPUT - potBrightnessValue;
    }
  
    unsigned long hue = map(potHueValue, 0, MAX_ANALOG_INPUT, 0, MAX_HUE_VALUE);

    // if we are in cross fade, then use potHueValue to set cross fade speed
    if(_nightLightMode == CROSS_FADE){
      unsigned int crossFadeStep = map(potHueValue, 0, MAX_ANALOG_INPUT, MIN_CROSS_FADE_STEP, MAX_CROSS_FADE_STEP);;
      _crossFadeVal += crossFadeStep;

      if(_crossFadeVal > MAX_HUE_VALUE){
        _crossFadeVal = 0;
      }
      hue = _crossFadeVal;
    }
    
    unsigned int saturation = 255;
    unsigned int brightness = map(potBrightnessValue, 0, MAX_ANALOG_INPUT, 0, MAX_BRIGHTNESS_VALUE);

    // Moderate the brightness by the photoresistor value if that mode is set
    if(_nightLightMode == BRIGHTNESS_INVERSE_PROPORTIONAL_TO_DARKNESS){
      brightness = brightness * (photoCellVal / (float)MAX_ANALOG_INPUT);

      if(photoCellVal < TURN_OFF_DARKNESS_THRESHOLD){
        brightness = 0;
      }
    }
    
    uint32_t rgbColor = _neopixel.ColorHSV(hue, saturation, brightness);
    _neopixel.fill(rgbColor, 0, NUM_NEOPIXELS);
    _neopixel.show();

    Serial.println((String)"potHueValue=" + potHueValue + 
                    " potBrightnessValue=" + potBrightnessValue + 
                    " photoCellVal=" + photoCellVal + 
                    " nightLightMode=" + _nightLightMode +
                    " (h, s, b)=(" + hue + ", " + saturation + ", " + brightness + ")");
  }else{
    // Turn off the neopixels, which we can do either by _neopixel.fill() (no args)
    // or by calling .clear()
    _neopixel.clear();
    _neopixel.show();

    Serial.println((String)"Night Light OFF: photoCellVal=" + photoCellVal + 
                    " TURN_ON_DARKNESS_THRESHOLD=" + TURN_ON_DARKNESS_THRESHOLD + 
                    " nightLightMode=" + _nightLightMode);
  }

  delay(10);
}
