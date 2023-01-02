// TODO
//
// If you're using a 3.3V board like the Adafruit Feather series or Arduino Nano 33 IoT,
// you may need a level shifter like the 74AHCT125 (https://www.adafruit.com/product/1787)
// or the LXB0108 for even larger projects (https://www.adafruit.com/products/395).
// I was able to successfully run this code with both a 30 NeoPixel strand and, surprisingly,
// a 150 NeoPixel strand (5m & 30 pixels/m) off USB laptop power without a level shifter.
// With the latter, I measured the current draw on USB to be ~370mA max with a stable rate
// of ~340mA (with all red illuminated) and ~275mA (with all blue illuminated).
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

#include <Adafruit_NeoPixel.h> // https://github.com/adafruit/Adafruit_NeoPixel

const int NUM_NEOPIXELS = 30;       // Change this to match your strand length
const int NEOPIXEL_PIN_OUTPUT = 5;  // Change this to match your output pin

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


// Sound level stuff
const int MIC_INPUT_PIN = A4;
const int MAX_MIC_LEVEL = 950;

const int MIC_SAMPLE_WINDOW_MS = 50; // Sample window width in ms (50 ms = 20Hz)

unsigned long _startSampleTimeMs = -1;
unsigned int _signalMax = 0;
unsigned int _signalMin = MAX_ANALOG_INPUT;

void setup() {
  Serial.begin(115200);

  //Wait for serial port to be opened, remove this line for 'standalone' operation
  while (!Serial) { delay(1); }

  _neopixelStrip.begin();            // Initialize NeoPixel strip object (REQUIRED)
  _neopixelStrip.setBrightness(50);  // Set brightness to about 1/5 (max = 255)
  _neopixelStrip.show();             // Turn OFF all pixels ASAP

  pinMode(MIC_INPUT_PIN, INPUT);

  _startSampleTimeMs = millis();
}

void loop() {
   // Read in current sound level from microphone
  int micLevel = analogRead(MIC_INPUT_PIN);

  if(micLevel > _signalMax){
    _signalMax = micLevel;
  }else if(micLevel < _signalMin){
    _signalMin = micLevel;
  }

  if(millis() - _startSampleTimeMs > MIC_SAMPLE_WINDOW_MS){
    unsigned int peakToPeak = _signalMax - _signalMin;

    int hueVal = map(peakToPeak, 0, MAX_MIC_LEVEL, 0, MAX_HUE_VALUE * .8);
    const int saturation = MAX_SATURATION_VALUE;
    const int brightness = 150;
    uint32_t rgbColor = _neopixelStrip.ColorHSV(hueVal, saturation, brightness);

    unsigned int numNeoPixelsToIlluminate = map(peakToPeak, 0, MAX_MIC_LEVEL, 0, NUM_NEOPIXELS);
    numNeoPixelsToIlluminate = constrain(numNeoPixelsToIlluminate, 0, NUM_NEOPIXELS);
    
    // See: https://adafruit.github.io/Adafruit_NeoPixel/html/class_adafruit___neo_pixel.html
    //_neopixelStrip.clear();
    _neopixelStrip.fill(rgbColor, 0, numNeoPixelsToIlluminate);
    _neopixelStrip.fill(0, numNeoPixelsToIlluminate, NUM_NEOPIXELS); // clear others
    //_neopixelStrip.setBrightness(brightnessVal); 
    _neopixelStrip.show();

    Serial.print(_signalMin);
    Serial.print(", ");
    Serial.print(_signalMax);
    Serial.print(", ");
    Serial.print(peakToPeak);
    Serial.print(", ");
    Serial.print(numNeoPixelsToIlluminate);
    Serial.print(", ");
    Serial.println(hueVal);

    _signalMax = 0;
    _signalMin = MAX_ANALOG_INPUT;
    _startSampleTimeMs = millis();
  }
}
