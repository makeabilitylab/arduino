/**
 * Feel free to modify this template for your OLED prototype
 *
 * See: https://makeabilitylab.github.io/physcomp/advancedio/oled.html
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

// #include <SPI.h> Uncomment this if you use SPI
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET     4 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 _display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

void setup() {
  Serial.begin(9600); // Just for debugging

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!_display.begin(SSD1306_SWITCHCAPVCC, 0x3D)) { // Address 0x3D for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }

  // show title screen
  showTitleScreen("OLED Template", 1000);
}

void loop() {

  // Clear all contents of the display buffer (set all pixels to off)
  _display.clearDisplay();

  // Draw something!
  // In this case, we are drawing a circle in the center of the screen
  // But you can put all your drawing code here!
  const int circleRadius = 40;
  const int xCirclePos = _display.width() / 2;
  const int yCirclePos = _display.height() / 2;
  _display.fillCircle(xCirclePos, yCirclePos, circleRadius, SSD1306_WHITE);

  // Contents of the screen buffer (memory) are not render to the display
  // until display is called.
  _display.display();
}

void showTitleScreen(const char* title, const int durationInMs){
  // Clear the buffer
  _display.clearDisplay();

  // Display opening screen
  _display.setTextSize(1);
  _display.setTextColor(WHITE, BLACK);
  int16_t x1, y1;
  uint16_t textWidth, textHeight;  
  _display.getTextBounds(title, 0, 0, &x1, &y1, &textWidth, &textHeight);
  uint16_t xText = _display.width() / 2 - textWidth / 2;
  uint16_t yText = _display.height() / 2 - textHeight / 2;
  _display.setCursor(xText, yText);
  _display.println(title);
  _display.display();
  delay(durationInMs);

  _display.clearDisplay();
}
