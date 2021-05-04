/**
   Simple drawing demo that draws a random number of circles, rects, and other
   shapes on each frame

   Adafruit Gfx Library:
   https://learn.adafruit.com/adafruit-gfx-graphics-library/overview

   Adafruit OLED tutorials:
   https://learn.adafruit.com/monochrome-oled-breakouts

   By Jon E. Froehlich
   @jonfroehlich
   http://makeabilitylab.io

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

const char STR_TITLE[] = "DRAW DEMO";

void setup() {
  Serial.begin(9600);

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if (!_display.begin(SSD1306_SWITCHCAPVCC, 0x3D)) { // Address 0x3D for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    for (;;); // Don't proceed, loop forever
  }

  // We need to randomly seed our random number generator, which we can
  // do with noise off of A5 (assuming nothing is attached to it)
  //
  // If we don't do this, then the random values will always be the same
  // after reboot
  randomSeed(analogRead(A5));

  _display.clearDisplay();
}

void loop() {
  // On each loop, we'll want to clear the display so we're not writing over
  // previously drawn data
  _display.clearDisplay();

  int numCircles = random(2, 6);
  drawRandomCircles(numCircles);

  int numRects = random(2, 6);
  drawRandomRectangles(numRects);

  int numRoundedRects = random(2, 6);
  drawRandomRoundedRectangles(numRoundedRects);

  int numTriangles = random(2, 6);
  drawRandomTriangles(numTriangles);
  
  drawTitleText();

  // Render the graphics buffer to screen
  _display.display();

  delay(500);
}

/**
 * Draws the given number of circles with random x,y location
 * and random radius
 * 
 * See: https://learn.adafruit.com/adafruit-gfx-graphics-library/graphics-primitives#circles-2002788-14
 */
void drawRandomCircles(int numCircles){
  const int MIN_RADIUS = 2;
  const int MAX_RADIUS = 10;
  int x, y, radius;
  
  for(int i = 0; i < numCircles; i++){
    x = random(0, _display.width());
    y = random(0, _display.height());
    radius = random(MIN_RADIUS, MAX_RADIUS);

    // Returns a random number from 0 to 9, inclusive
    int randomFill = random(0, 10);

    if(randomFill < 5){
      _display.drawCircle(x, y, radius, SSD1306_WHITE);
    }else{
      _display.fillCircle(x, y, radius, SSD1306_WHITE);
    }
  }
}

/**
 * Draws the given number of rectangles with random x,y location
 * and random width, height dimensions
 * 
 * See: https://learn.adafruit.com/adafruit-gfx-graphics-library/graphics-primitives#rectangles-2002784-10
 */
void drawRandomRectangles(int numRectangles){
  const int MIN_WIDTH = 2;
  const int MAX_WIDTH = 30;
  const int MIN_HEIGHT = 2;
  const int MAX_HEIGHT = 30;
  
  int x, y, width, height;
  
  for(int i = 0; i < numRectangles; i++){
    x = random(0, _display.width());
    y = random(0, _display.height());
    width = random(MIN_WIDTH, MAX_WIDTH);
    height = random(MIN_HEIGHT, MAX_WIDTH);

    // Returns a random number from 0 to 9, inclusive
    int randomFill = random(0, 10);

    if(randomFill < 5){
      _display.drawRect(x, y, width, height, SSD1306_WHITE);
    }else{
      _display.fillRect(x, y, width, height, SSD1306_WHITE);
    }
  }
}

/**
 * Draws the given number of triangles with random location and dimensions
 * 
 * See: https://learn.adafruit.com/adafruit-gfx-graphics-library/graphics-primitives#rectangles-2002784-10
 */
void drawRandomTriangles(int numTriangles){
  const int MAX_SIZE = 30;
  int x0, y0, x1, y1, x2, y2;
  
  for(int i = 0; i < numTriangles; i++){
    x0 = random(0, _display.width());
    y0 = random(0, _display.height());

    x1 = random(x0, x0 + MAX_SIZE);
    y1 = random(y0, y0 + MAX_SIZE);

    x2 = random(x0, x0 + MAX_SIZE);
    y2 = random(y0, y0 + MAX_SIZE);

    // Returns a random number from 0 to 9, inclusive
    int randomFill = random(0, 10);
    
    if(randomFill < 5){
      _display.drawTriangle(x0, y0, x1, y1, x2, y2, SSD1306_WHITE);
    }else{
      _display.fillTriangle(x0, y0, x1, y1, x2, y2, SSD1306_WHITE);
    }
  }
}

/**
 * Draws the given number of rounded rectangles with random x,y location
 * and random width, height, and rounded corner dimensions
 * 
 * See: https://learn.adafruit.com/adafruit-gfx-graphics-library/graphics-primitives#rounded-rectangles-2002792-18
 */
void drawRandomRoundedRectangles(int numRectangles){
  const int MIN_WIDTH = 4;
  const int MAX_WIDTH = 30;
  const int MIN_HEIGHT = 4;
  const int MAX_HEIGHT = 30;
  
  int x, y, width, height;
  
  for(int i = 0; i < numRectangles; i++){
    x = random(0, _display.width());
    y = random(0, _display.height());
    width = random(MIN_WIDTH, MAX_WIDTH);
    height = random(MIN_HEIGHT, MAX_WIDTH);

    int cornerRadius = random(2, min(width, height) / 2);

    // Returns a random number from 0 to 9, inclusive
    int randomFill = random(0, 10);

    if(randomFill < 5){
      _display.drawRoundRect(x, y, width, height, cornerRadius, SSD1306_WHITE);
    }else{
      _display.fillRoundRect(x, y, width, height, cornerRadius, SSD1306_WHITE);
    }
  }
}

/**
 * Draws the text centered on the screen
 * 
 * See: https://learn.adafruit.com/adafruit-gfx-graphics-library/graphics-primitives#characters-and-text-2002798-24
 */
void drawTitleText() {
  int16_t x, y;
  uint16_t textWidth, textHeight;

  // Setup text rendering parameters
  _display.setTextSize(1);
  _display.setTextColor(BLACK, WHITE);

  // Measure the text with those parameters
  _display.getTextBounds(STR_TITLE, 0, 0, &x, &y, &textWidth, &textHeight);

  // Center the text on the display
  _display.setCursor(_display.width() / 2 - textWidth / 2, _display.height() / 2 - textHeight / 2);

  // Print out the string
  _display.print(STR_TITLE);
}
