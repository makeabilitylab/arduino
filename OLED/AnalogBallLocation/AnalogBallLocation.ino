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
const int X_ANALOG_INPUT_PIN = A0; // control x location of ball
const int Y_ANALOG_INPUT_PIN = A1; // control y location of ball
const int BALL_RADIUS = 15;

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

  // Read the analog input value
  int xSensorVal = analogRead(X_ANALOG_INPUT_PIN);
  delay(1); // give ADC time
  int ySensorVal = analogRead(Y_ANALOG_INPUT_PIN);

  // Translate sensor readings to x, y pixel locations
  int xLoc = map(xSensorVal, 0, MAX_ANALOG_INPUT, 0, _display.width());
  int yLoc = map(ySensorVal, 0, MAX_ANALOG_INPUT, 0, _display.width());

  // Draw it on the screen
  _display.fillCircle(xLoc, yLoc,  BALL_RADIUS, SSD1306_WHITE);

  // Render the graphics buffer to screen
  _display.display(); 

  delay(50);
}
