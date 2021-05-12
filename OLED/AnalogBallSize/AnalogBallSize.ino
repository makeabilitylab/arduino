/**
 * Change the size of the circle depending on analog input
 * 
 * This code has been tested on: 
 *  - the Adafruit Huzzah32 ESP32
 *  - Arduino Leonardo
 * 
 * For our OLED tutorials, see:
 * https://makeabilitylab.github.io/physcomp/
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

// Change to 4095 for 12-bit ADCs like ESP32 & 1023 for 10-bit ADCs like Arduino Uno/Leonardo
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

  // Read the analog input value
  int sensorVal = analogRead(ANALOG_INPUT_PIN);

  // The maximum radius is either the display width or height, whichever is smallest
  int maxRadius = min(_display.width(), _display.height());

  // Now calculate the radius based on the sensor val
  int radius = map(sensorVal, 0, MAX_ANALOG_INPUT, 0, maxRadius);

  // Center the circle
  int xCircle = _display.width() / 2;
  int yCircle = _display.height() / 2;

  // Draw it on the screen
  _display.fillCircle(xCircle, yCircle,  radius, SSD1306_WHITE);

  // Render the graphics buffer to screen
  _display.display(); 

  delay(50);
}
