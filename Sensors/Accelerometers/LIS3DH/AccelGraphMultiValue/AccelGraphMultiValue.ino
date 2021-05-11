
/**
 *  Draws a scrolling graph of the X, Y, and Z axes from the LIS3DH accelerometer
 *
 *  By Jon E. Froehlich
 *  @jonfroehlich
 *  http://makeabilitylab.io
 *
 */

#include <Wire.h>
#include <SPI.h>

// For the OLED Display
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// For the accelerometer
#include <Adafruit_LIS3DH.h> 

#include <ScrollingLineGraphMultiValue.hpp> // from Makeability Lab Arduino Library

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET     4 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 _display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Used for LIS3DH hardware & software SPI
#define LIS3DH_CS 10
Adafruit_LIS3DH _lis3dh = Adafruit_LIS3DH();

const int DELAY_LOOP_MS = 0; // change to slow down how often to read and graph value

// for tracking fps
float _fps = 0;
unsigned long _frameCount = 0;
unsigned long _fpsStartTimeStamp = 0;
boolean _drawFps = false;

const int NUM_GRAPH_LINES = 3;
const PointSymbol GRAPH_SYMBOLS[NUM_GRAPH_LINES] = {CIRCLE, SQUARE, TRIANGLE};
ScrollingLineGraphMultiValue _scrollingLineGraph(NUM_GRAPH_LINES, GRAPH_SYMBOLS, 2);

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
  delay(200);
  _display.clearDisplay();

  _display.println("Initializing accelerometer...");
  if (!_lis3dh.begin(0x18)) {   // change this to 0x19 for alternative i2c address
    Serial.println("Couldn't start");
    while (1) yield();
  }
  Serial.println("LIS3DH found!");

  _lis3dh.setRange(LIS3DH_RANGE_4_G);   // 2, 4, 8 or 16 G!

  Serial.print("Range = ");
  Serial.print(2 << _lis3dh.getRange());
  Serial.println("G");

  _fpsStartTimeStamp = millis();

  _scrollingLineGraph.setMinMaxY(-11000, 12000);
  _scrollingLineGraph.setAutoYAxis(true);
}

void loop() {
  // Clear the display on each frame. 
  _display.clearDisplay();

  _lis3dh.read(); 

  // Read and store the analog data into a circular buffer
  _scrollingLineGraph.addData(0, _lis3dh.x);

  delay(1);
  _scrollingLineGraph.addData(1, _lis3dh.y);

  delay(1);
  _scrollingLineGraph.addData(2, _lis3dh.z);

  if(_drawFps){
    drawFps();
  }

  _scrollingLineGraph.draw(_display);
  
  _display.display();
  
  calcFrameRate();
  
  if(DELAY_LOOP_MS > 0){
    delay(DELAY_LOOP_MS);
  }
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
