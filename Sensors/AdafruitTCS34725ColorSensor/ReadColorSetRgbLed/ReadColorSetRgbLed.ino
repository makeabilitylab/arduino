/**
 * Uses the Adafruit TCS34725 color sensor to sense a color and
 * set an RGB appropriately to that color.
 * 
 * Based on:
 * https://learn.adafruit.com/adafruit-color-sensors/overview
 *  
 * By Jon E. Froehlich
 * @jonfroehlich
 * http://makeabilitylab.io
 *
 */

#include <Wire.h>
#include <Adafruit_TCS34725.h>

// Change this to based on whether you are using a common anode or common cathode
// RGB LED. See: https://makeabilitylab.github.io/physcomp/arduino/rgb-led
// If you are working with a common cathode RGB LED, set this to false.
// Note Adafruit sells a common anode design: https://www.adafruit.com/product/159
// but for our courses, we try to purchase common cathodes (as they're more straightforward
// to use).
const boolean COMMON_ANODE = false;
const int DELAY_MS = 50;
const int RGB_RED_PIN = 5;
const int RGB_GREEN_PIN = 9;
const int RGB_BLUE_PIN = 10;
const int MAX_COLOR_VALUE = 255;
const boolean USE_GAMMA_CORRECTION = true;

Adafruit_TCS34725 _colorSensor = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_50MS, TCS34725_GAIN_4X);

// our RGB -> eye-recognized gamma color
// See: https://learn.adafruit.com/chameleon-scarf/code
byte _gammaTable[256];

void setup() {
  Serial.begin(9600);

  if (_colorSensor.begin()) {
    //Serial.println("Found sensor");
  } else {
    Serial.println("No TCS34725 found ... check your connections");
    while (1); // halt!
  }

  // Set the RGB pins to output
  pinMode(RGB_RED_PIN, OUTPUT);
  pinMode(RGB_GREEN_PIN, OUTPUT);
  pinMode(RGB_BLUE_PIN, OUTPUT);

  // Gamma table code from: https://learn.adafruit.com/chameleon-scarf/code
  // Corrects color output based on human perception
  for (int i = 0; i < 256; i++) {
    float x = i;
    x /= 255;
    x = pow(x, 2.5);
    x *= 255;

    if (COMMON_ANODE == true) {
      _gammaTable[i] = MAX_COLOR_VALUE - x;
    } else {
      _gammaTable[i] = x;
    }
  }

}

void loop() {
  float sensedRed, sensedGreen, sensedBlue;

  // Read from color sensor
  _colorSensor.setInterrupt(false);      // turn on LED
  delay(60);                    // takes 50ms to read
  _colorSensor.setInterrupt(true);       // turn off LED
  _colorSensor.getRGB(&sensedRed, &sensedGreen, &sensedBlue);

  

  setColor((int)sensedRed, (int)sensedGreen, (int)sensedBlue);

  delay(DELAY_MS);
}

/**
   setColor takes in values between 0 - 255 for the amount of red, green, and blue, respectively
   where 255 is the maximum amount of that color and 0 is none of that color. You can illuminate
   all colors by intermixing different combinations of red, green, and blue
*/
void setColor(int red, int green, int blue)
{
  
  if (USE_GAMMA_CORRECTION == false) {
    // If a common anode LED, invert values
    if (COMMON_ANODE == true) {
      red = MAX_COLOR_VALUE - red;
      green = MAX_COLOR_VALUE - green;
      blue = MAX_COLOR_VALUE - blue;
    }
    analogWrite(RGB_RED_PIN, red);
    analogWrite(RGB_GREEN_PIN, green);
    analogWrite(RGB_BLUE_PIN, blue);
  } else {
    analogWrite(RGB_RED_PIN, _gammaTable[red]);
    analogWrite(RGB_GREEN_PIN, _gammaTable[green]);
    analogWrite(RGB_BLUE_PIN, _gammaTable[blue]);
  }

  Serial.print("RAW\tRed:"); Serial.print(red);
  Serial.print(", Green:"); Serial.print(green);
  Serial.print(", Blue:"); Serial.print(blue);
  Serial.println();

  Serial.print("GAMMA\tRed:"); Serial.print(_gammaTable[red]);
  Serial.print(", Green:"); Serial.print(_gammaTable[green]);
  Serial.print(", Blue:"); Serial.print(_gammaTable[blue]);
  Serial.println();
}
