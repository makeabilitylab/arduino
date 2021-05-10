
/**
 *  Draws a scrolling graph of a three inputs on A0, A1, and A2
 *
 *  By Jon E. Froehlich
 *  @jonfroehlich
 *  http://makeabilitylab.io
 *
 */

#include <Wire.h>
#include <SPI.h>

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include <ScrollingLineGraphMultiValue.hpp> // from Makeability Lab Arduino Library

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET     4 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 _display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

const int ANALOG_INPUT_PIN1 = A0;
const int ANALOG_INPUT_PIN2 = A1;
const int ANALOG_INPUT_PIN3 = A2;

const int MIN_ANALOG_INPUT = 0;
const int MAX_ANALOG_INPUT = 1023;
const int DELAY_LOOP_MS = 5; // change to slow down how often to read and graph value

// for tracking fps
float _fps = 0;
unsigned long _frameCount = 0;
unsigned long _fpsStartTimeStamp = 0;
boolean _drawFps = false;

const int NUM_GRAPH_LINES = 3;
const PointSymbol GRAPH_SYMBOLS[NUM_GRAPH_LINES] = {CIRCLE, SQUARE, TRIANGLE};
ScrollingLineGraphMultiValue _scrollingLineGraph(NUM_GRAPH_LINES, GRAPH_SYMBOLS);

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

  _fpsStartTimeStamp = millis();
}

void loop() {
  // Clear the display on each frame. 
  _display.clearDisplay();

  // Read and store the analog data into a circular buffer
  int analogVal = analogRead(ANALOG_INPUT_PIN1);
  _scrollingLineGraph.addData(0, analogVal);

  delay(1);
  analogVal = analogRead(ANALOG_INPUT_PIN2);
  _scrollingLineGraph.addData(1, analogVal);

  delay(1);
  analogVal = analogRead(ANALOG_INPUT_PIN3);
  _scrollingLineGraph.addData(2, analogVal);

  if(_drawFps){
    drawFps();
  }

  _scrollingLineGraph.drawLegend(_display, 0, 0);
  _scrollingLineGraph.draw(_display);
  
  _display.display();
  
  calcFrameRate();
  
  delay(DELAY_LOOP_MS);
}

/**
 * Call this every frame to calculate frame rate
 */
void calcFrameRate() {
    
  unsigned long elapsedTime = millis() - _fpsStartTimeStamp;
  _frameCount++;
  if (elapsedTime > 1000) {
    _fps = _frameCount / (elapsedTime / 1000.0);
    _fpsStartTimeStamp = millis();
    _frameCount = 0;
  }
}

/**
 * Draws the status bar at top of screen with fps
 */
void drawFps() {

  // Draw frame count
  int16_t x1, y1;
  uint16_t w, h;
  _display.getTextBounds("XX.X fps", 0, 0, &x1, &y1, &w, &h);
  _display.setCursor(_display.width() - w, 0);
  _display.print(_fps, 1);
  _display.print(" fps");
}
