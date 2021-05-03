/**
 * Uses the Adafruit TCS34725 color sensor to sense a color,
 * set an RGB appropriately to that color, and print out the closest
 * color to serial.
 * 
 * Based on:
 * https://learn.adafruit.com/adafruit-color-sensors/overview
 * 
 * Closest color code based on:
 * https://stackoverflow.com/a/20670056/388117
 *  
 * By Jon E. Froehlich
 * @jonfroehlich
 * http://makeabilitylab.io
 *
 */

#include <Wire.h>
#include <Adafruit_TCS34725.h>
#include <ColorName.hpp>

// Includes for the OLED
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED _display width, in pixels
#define SCREEN_HEIGHT 64 // OLED _display height, in pixels
#define OLED_RESET     4 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 _display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Change this to based on whether you are using a common anode or common cathode
// RGB LED. See: https://makeabilitylab.github.io/physcomp/arduino/rgb-led
// If you are working with a common cathode RGB LED, set this to false.
// Note Adafruit sells a common anode design: https://www.adafruit.com/product/159
// but for our courses, we try to purchase common cathodes (as they're more straightforward
// to use).
const boolean COMMON_ANODE = false;
const int DELAY_MS = 10;
const int RGB_RED_PIN = 5;
const int RGB_GREEN_PIN = 9;
const int RGB_BLUE_PIN = 10;
const int MAX_COLOR_VALUE = 255;
const boolean USE_GAMMA_CORRECTION = true;

Adafruit_TCS34725 _colorSensor = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_50MS, TCS34725_GAIN_4X);

// our RGB -> eye-recognized gamma color
// See: https://learn.adafruit.com/chameleon-scarf/code
byte _gammaTable[256];



// Raw values from https://www.rapidtables.com/web/color/RGB_Color.html
//const int NUM_COLOR_NAMES = 8;
//ColorName _colorNames[NUM_COLOR_NAMES] = {
//  ColorName("Red", 0xFF, 0x00, 0x00),
//  ColorName("Orange", 0xFF, 0xA5, 0x00),
//  ColorName("Yellow", 0xFF, 0xFF, 0x00),
//  ColorName("Green", 0x00, 0xFF, 0x00),
//  ColorName("Blue", 0x00, 0x00, 0xFF),
//  ColorName("Cyan", 0x00, 0xFF, 0xFF),
//  ColorName("Magenta", 0xFF, 0x00, 0xFF),
//  ColorName("Purple", 0x80, 0x00, 0x80)
//};

// Measured values. Change these according to the values measured
// and printed out via Serial.print to the screen
const int NUM_COLOR_NAMES = 12;
ColorName _colorNames[NUM_COLOR_NAMES] = {
  ColorName("Red", 150, 40, 40),
  ColorName("Red", 160, 50, 50),
  ColorName("Red", 255, 0, 0),
  ColorName("Orange", 160, 66, 34),
  ColorName("Yellow", 116, 93, 37),
  ColorName("Green", 0, 255, 0),
  ColorName("Green", 50, 130, 50),
  ColorName("Blue", 0, 0, 255),
  ColorName("Blue", 50, 50, 130),
  ColorName("Cyan", 0, 255, 255),
  ColorName("Magenta", 255, 0, 255),
  ColorName("Purple", 90, 80, 90)
};



void setup() {
  Serial.begin(9600);

  // Start TCS34725 color sensor
  if (!_colorSensor.begin()) {
    Serial.println("No TCS34725 found ... check your connections");
    while (1); // halt!
  }

  // Start OLED display
  // SSD1306_SWITCHCAPVCC = generate _display voltage from 3.3V internally
  if (!_display.begin(SSD1306_SWITCHCAPVCC, 0x3D)) { // Address 0x3D for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    for (;;); // Don't proceed, loop forever
  }
  _display.clearDisplay();

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

    _gammaTable[i] = x;
  }

}

void loop() {
  _display.clearDisplay();
  
  float sensedRed, sensedGreen, sensedBlue;

  // Read from color sensor
  _colorSensor.setInterrupt(false);      // turn on LED
  delay(60);                    // takes 50ms to read
  _colorSensor.setInterrupt(true);       // turn off LED
  _colorSensor.getRGB(&sensedRed, &sensedGreen, &sensedBlue);

  // Set the RGB LED color
  int rawRed = (int)sensedRed;
  int rawGreen = (int)sensedGreen;
  int rawBlue = (int)sensedBlue;
  setRgbLedColor(rawRed, rawGreen, rawBlue);

  // Get the name of closest color and print to serial
  ColorName *closestColorName = ColorName::getClosestColorName(_colorNames, 
    NUM_COLOR_NAMES, rawRed, rawGreen, rawBlue);
  Serial.println(closestColorName->getName());

  drawColorName(closestColorName);
  _display.display();

  delay(DELAY_MS);
}

/**
 * Draws the color name to the display
 */
void drawColorName(ColorName *colorName){
  int16_t x, y;
  uint16_t w, h;

  _display.setTextSize(2);
  _display.setTextColor(WHITE, BLACK);
  _display.getTextBounds(colorName->getName(), 0, 0, &x, &y, &w, &h);

  // Center the text on the display
  _display.setCursor(_display.width() / 2 - w / 2, _display.height() / 2 - h / 2);
  _display.print(colorName->getName());
}


/**
 * Sets the RGB LED color
 */
void setRgbLedColor(int red, int green, int blue)
{
  int gammaRed, gammaGreen, gammaBlue;
  
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

    if (COMMON_ANODE == true) {
      gammaRed = MAX_COLOR_VALUE - _gammaTable[red];
      gammaGreen = MAX_COLOR_VALUE - _gammaTable[green];
      gammaBlue = MAX_COLOR_VALUE - _gammaTable[blue];
    } else {
      gammaRed = _gammaTable[red];
      gammaGreen = _gammaTable[green];
      gammaBlue = _gammaTable[blue];
    }
    
    analogWrite(RGB_RED_PIN, gammaRed);
    analogWrite(RGB_GREEN_PIN, gammaGreen);
    analogWrite(RGB_BLUE_PIN, gammaBlue);
  }

  Serial.print("RAW\tRed:"); Serial.print(red);
  Serial.print(", Green:"); Serial.print(green);
  Serial.print(", Blue:"); Serial.print(blue);
  Serial.println();

  Serial.print("GAMMA\tRed:"); Serial.print(gammaRed);
  Serial.print(", Green:"); Serial.print(gammaGreen);
  Serial.print(", Blue:"); Serial.print(gammaBlue);
  Serial.println();
}
