/**
 * Change the size of the circle depending on analog input
 * 
 * Adafruit Gfx Library:
 * https://learn.adafruit.com/adafruit-gfx-graphics-library/overview 
 *
 * Adafruit OLED tutorials:
 * https://learn.adafruit.com/monochrome-oled-breakouts
 *  
 * By Jon E. Froehlich
 * @jonfroehlich
 * http://makeabilitylab.io
 *
 */

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET     4 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 _display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

const int MAX_ANALOG_INPUT = 1023;
const int ANALOG_INPUT_PIN = A0;

void setup() {
  Serial.begin(9600);

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!_display.begin(SSD1306_SWITCHCAPVCC, 0x3D)) { // Address 0x3D for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }
  
  _display.clearDisplay();         
}

void loop() {
  // On each loop, we'll want to clear the display so we're not writing over
  // previously drawn data
  _display.clearDisplay(); 
  
  int sensorVal = analogRead(ANALOG_INPUT_PIN);

  int maxRadius = min(_display.width(), _display.height());
  int radius = map(sensorVal, 0, MAX_ANALOG_INPUT, 0, maxRadius);

  int xCircle = _display.width() / 2;
  int yCircle = _display.height() / 2;
  _display.fillCircle(xCircle, yCircle,  radius, SSD1306_WHITE);

  // Render the graphics buffer to screen
  _display.display(); 

  delay(50);
}
