
/**
 *  Draws a bouncing ball on the OLED display
 *
 *  Based on: https://makeabilitylab.github.io/p5js/Animation/BallBounce2D
 *  Source: https://github.com/makeabilitylab/p5js/tree/master/Animation/BallBounce2D
 *
 *  Adafruit Gfx Library:
 *  https://learn.adafruit.com/adafruit-gfx-graphics-library/overview 
 *
 *  Adafruit OLED tutorials:
 *  https://learn.adafruit.com/monochrome-oled-breakouts
 *  
 *  By Jon E. Froehlich
 *  @jonfroehlich
 *  http://makeabilitylab.io
 *
 */

#include <Wire.h>
#include <SPI.h>
#include "Shape.hpp"

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET     4 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 _display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

const int DELAY_LOOP_MS = 5; 

// Initialize ball
Ball _ball(20, 20, 5);  

// for tracking fps
unsigned long _totalFrameCount = 0;
unsigned long _startTimeStamp = 0;

// status bar
const boolean _drawStatusBar = true; // change to show/hide status bar

void setup() {
  Serial.begin(9600);

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if (!_display.begin(SSD1306_SWITCHCAPVCC, 0x3D)) { // Address 0x3D for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    for (;;); // Don't proceed, loop forever
  }

  // Clear the buffer
  _display.clearDisplay();

  _display.setTextSize(1);
  _display.setTextColor(WHITE, BLACK);
  _display.setCursor(0, 0);
  _display.println("Screen initialized!");
  _display.display();
  delay(500);
  _display.clearDisplay();
}

void loop() {
  if(_startTimeStamp == 0){
    _startTimeStamp = millis();
  }

  _display.clearDisplay();
  
  if(_drawStatusBar && _totalFrameCount > 0){
    int16_t x1, y1;
    uint16_t w, h;
    unsigned long elapsedTime = millis() - _startTimeStamp;
    float fps = _totalFrameCount / (elapsedTime / 1000.0);
    _display.getTextBounds("XX.XX fps", 0, 0, &x1, &y1, &w, &h);
    _display.setCursor(_display.width() - w, 0);
    _display.print(fps);
    _display.print(" fps");
  }
  
  // Update ball based on speed location
  _ball.update();

  if(_ball.checkXBounce(0, _display.width())){
    _ball.reverseX();
  }

  if(_ball.checkYBounce(0, _display.height())){
    _ball.reverseY();
  }

  _ball.draw(_display);

  // Render buffer to screen
  _display.display();
  _totalFrameCount++;

  if(DELAY_LOOP_MS > 0){
    delay(DELAY_LOOP_MS);
  }
}
