/**
 * Simple example that animates an increasingly large circle on screen
 * When max radius reached, animates smaller and oscillates back and forth.
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

#define SCREEN_WIDTH 128  // OLED display width, in pixels
#define SCREEN_HEIGHT 64  // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET 4  // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 _display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

void setup() {
  Serial.begin(9600);

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if (!_display.begin(SSD1306_SWITCHCAPVCC, 0x3D)) {  // Address 0x3D for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    for (;;)
      ;  // Don't proceed, loop forever
  }

  _display.clearDisplay();
}

void loop() {
  // Clear the buffer
  _display.clearDisplay();

  // We could certainly modify this to not use an embedded loop
  // But fine for now just to demonstrate drawing function
  int16_t radius = 0;
  int16_t maxRadius = max(_display.width(), _display.height()) / 2;
  const int16_t xCenter = _display.width() / 2;
  const int16_t yCenter = _display.width() / 2;
  for(; radius < maxRadius; radius += 2) {
    // Draw a filled circle in center of screen with radius i
    _display.fillCircle(xCenter, yCenter, radius, SSD1306_WHITE);
    _display.display();
    delay(1);
  }

  // Make the circle smaller
  for(; radius > 0; radius -= 2) {
    _display.clearDisplay();  // have to clear each time as getting smaller
    _display.fillCircle(xCenter, yCenter, radius, SSD1306_WHITE);
    _display.display();
    delay(1);
  }

  delay(100);
}
