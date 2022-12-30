/**
 * This code is by chrisinsingapore2011 posted to the Adafruit forums here:
 * https://forums.adafruit.com/viewtopic.php?p=708786#p708786
 *
 * It is intended to address the I2S.read() hanging problem also described 
 * in that forum post—and which we also experienced—with the VUMeter code.
 *
 * This code appears to work more reliably than MicInputPlotter, which
 * is from https://learn.adafruit.com/adafruit-i2s-mems-microphone-breakout/arduino-wiring-and-test
 * and also posted in our GitHub here:
 * 
 * 
 * Note: To use an i2s peripheral like an i2s microphone, you must use a microcontroller
 * that has hardware I2S peripheral support such as the Cortex M-series chips like the Arduino Zero, Feather M0, 
 * or single-board computers like the Raspberry Pi. This code has been tested on an
 * Adafruit Feather M0 Express: https://www.adafruit.com/product/3403
 * with the Adafruit i2s MEMs Microphone Breakout for the SPH0645LM4H
 * https://www.adafruit.com/product/3421  
 */ 

#include <I2S.h>
#include <Adafruit_NeoPixel.h>    //  Library that provides NeoPixel functions

#define INFINITY_NEGATIVE -99999
#define I2S_BUFFER_SIZE 512
#define I2S_BITS_PER_SAMPLE 32                    // 32-bits per I2S sample
uint8_t buffer[I2S_BUFFER_SIZE];                  // Allocate the buffer size
int *I2Svalues = (int *) buffer;                  // I2Svalues changes to integer type, rather than an unsigned int 8-bit type, same data, groups it differently
volatile boolean I2SAvailable = false;            // Shouldn't this be a boolean?
int _curI2sMax; 
const int MIN_SOUND_LEVEL = 7000;

// Create a NeoPixel object called onePixel that addresses 1 pixel in pin 8
Adafruit_NeoPixel _onePixel = Adafruit_NeoPixel(1, PIN_NEOPIXEL, NEO_GRB + NEO_KHZ800);
const int NEOPIXEL_IDX = 0;                                  // Used to hold the maximum sound reading from each group of samples (i.e. each 2mS)

const unsigned long MAX_HUE_VALUE = 65535; // Hue is a 16-bit number
const unsigned int MAX_BRIGHTNESS_VALUE = 255;
const unsigned int MAX_SATURATION_VALUE = 255;
const unsigned int BRIGHTNESS_VALUE = 20;
const unsigned int MAX_ANALOG_OUT = 255;

const int SOUND_LEVEL_LED_PIN = 11;

const int FADE_TIME_ELAPSED_THRESHOLD_MS = 1;
const int FADE_STEP_VALUE = 500;
long _lastFadeTimestampMs = -1;
int _curI2sFadeVal = -1;
    
void setup() {
  Serial.begin(115200);
  while (!Serial) { ; }                           // wait for serial port to connect. Needed for native USB port only

  pinMode(SOUND_LEVEL_LED_PIN, OUTPUT);

  _onePixel.begin();             // Start the NeoPixel object
  _onePixel.setPixelColor(NEOPIXEL_IDX, 255, 0, 255);
  _onePixel.setBrightness(20);   // Affects all subsequent settings
  _onePixel.show();              // Show

  I2S.onReceive(onI2SReceive);                    // Set the interrupt for when I2S data is available, to call the onI2SReceive sub
  //int sampleRate = 31250;                         // 48,000,000 / sampleRate must be a multiple of 64
  int sampleRate = 15625;
  if (!I2S.begin(I2S_PHILIPS_MODE, sampleRate, I2S_BITS_PER_SAMPLE)) { Serial.println("Failed to initialize I2S!"); while (1); } // do nothing
  I2S.read();                                     // One read call is required to start the interrupt handler

  _onePixel.begin();             // Start the NeoPixel object
  _onePixel.setPixelColor(NEOPIXEL_IDX, 0, 0, 255);
  _onePixel.show();              // Show
}

// This function will run at a frequency of (sampleRate / 64).  
// At 31.25khz, this is every 1962.5 microseconds so make sure any processing here takes less time than that
// At 15.625kHz, this is every 3925 microseconds (~4ms)
void onI2SReceive() { 
  _curI2sMax = INFINITY_NEGATIVE;                 // Reset the maximum counter to detect a new peak from this cycle
  if (I2SAvailable) { Serial.println("POTENTIAL OVERFLOW"); } // This indicates that the program is not keeping up with processing
  I2S.read(buffer, I2S_BUFFER_SIZE);              // Will actually read 256 bytes
  for (int i = 0; i < I2S_BITS_PER_SAMPLE; i++) { // Process the data
    if ((I2Svalues[i]!= 0) && (I2Svalues[i] != -1) ) {  // All 0 or -1 readings need to be discarded (every second reading depending on SEL
      I2Svalues[i] >>= 14;   // convert to 18 bit signed
      int absVal = abs(I2Svalues[i]);
      if (_curI2sMax < absVal) { _curI2sMax = absVal; } // Record the new maximum value       
    }
  }

  //Serial.println(I2Smax_2ms);
  I2SAvailable = true;
}

void loop() {
  if (I2SAvailable) {
    if(_curI2sFadeVal < _curI2sMax){
      _curI2sFadeVal = _curI2sMax;
      _lastFadeTimestampMs = millis();
    }

    int hueVal = map(_curI2sFadeVal, MIN_SOUND_LEVEL, 60000, 0, MAX_HUE_VALUE);
    hueVal = constrain(hueVal, 0, MAX_HUE_VALUE);
    Serial.print(_curI2sMax);
    Serial.print(", ");
    Serial.print(_curI2sFadeVal);
    Serial.print(", ");
    Serial.println(hueVal);
    const int saturation = MAX_SATURATION_VALUE;
    const int brightness = BRIGHTNESS_VALUE;

    // ColorHSV https://adafruit.github.io/Adafruit_NeoPixel/html/class_adafruit___neo_pixel.html#a1f16aee5b96e16e62598f826e292b23b
    // hue	An unsigned 16-bit value, 0 to 65535, representing one full loop of the color wheel, which allows 16-bit hues to "roll over" 
    //      while still doing the expected thing (and allowing more precision than the wheel() function that was common to prior NeoPixel examples).
    // sat	Saturation, 8-bit value, 0 (min or pure grayscale) to 255 (max or pure hue). Default of 255 if unspecified.
    // val	Value (brightness), 8-bit value, 0 (min / black / off) to 255 (max or full brightness). Default of 255 if unspecified.
    uint32_t rgbColor = _onePixel.ColorHSV(hueVal, saturation, brightness);
    
    // See: https://adafruit.github.io/Adafruit_NeoPixel/html/class_adafruit___neo_pixel.html
    _onePixel.fill(rgbColor, 0, 1);
    _onePixel.setBrightness(brightness); 
    _onePixel.show();

    int ledBrightnessVal = map(_curI2sFadeVal, MIN_SOUND_LEVEL, 25000, 0, MAX_ANALOG_OUT);
    ledBrightnessVal = constrain(ledBrightnessVal, 0, MAX_ANALOG_OUT);
    analogWrite(SOUND_LEVEL_LED_PIN, ledBrightnessVal);

    if(_lastFadeTimestampMs == -1 || millis() - _lastFadeTimestampMs > FADE_TIME_ELAPSED_THRESHOLD_MS){
      _curI2sFadeVal = _curI2sFadeVal - FADE_STEP_VALUE;
      if(_curI2sFadeVal < MIN_SOUND_LEVEL){
        _curI2sFadeVal = MIN_SOUND_LEVEL;
      }
      _lastFadeTimestampMs = millis();
    }
    
    // Add a second round of processing to pick out a longer term maximum
    I2SAvailable = false;
  }
}

// long mapLog(long x, long in_min, long in_max, long out_min, long out_max)
// {
//   // return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
//   return log(x + 1)/log(1024)*255;

//   // float mapLog(float value, float in_min, float stop1, float start2, float stop2) {
//   float out_min_log = log(out_min);
//   float out_max_log = log(out_max);
//   float outgoing =
//         exp(out_min_log + (out_max_log - out_min_log) * ((x - in_min) / (in_max - in_min)));
// }

