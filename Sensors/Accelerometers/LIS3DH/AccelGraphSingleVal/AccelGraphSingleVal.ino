
/**
 *  This example graphs an LIS3DH accelerometer value in real-time to an OLED screen. 
 *  You can select which value to graph: x, y, z, or the magnitude of the accelerometer
 *  signal using the GraphData _graphData enum.
 *  
 *  Other configurable options:
 *  - turn on/off the status bar at the top by setting _drawStatusBar to false
 *  - graph points rather than lines by setting _drawGraphPrimitive = POINTS
 *
 *  Adafruit Gfx Library:
 *  https://learn.adafruit.com/adafruit-gfx-graphics-library/overview 
 *
 *  Adafruit OLED tutorials:
 *  https://learn.adafruit.com/monochrome-oled-breakouts
 *
 *  Adafruit LIS3DH tutorial:
 *  https://learn.adafruit.com/adafruit-lis3dh-triple-axis-accelerometer-breakout
 *
 *  By Jon E. Froehlich
 *  @jonfroehlich
 *  http://makeabilitylab.io
 *
 */

#include <Wire.h>
#include <SPI.h>
#include <Adafruit_LIS3DH.h>
#include <Adafruit_Sensor.h>

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET     4 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Used for LIS3DH hardware & software SPI
#define LIS3DH_CS 10
Adafruit_LIS3DH lis = Adafruit_LIS3DH();

int _minSensorVal = 32767; // adaptive value, will be set automatically when sensor values read
int _maxSensorVal = -32767; // adaptive value, will be set automatically when sensor values read
const int DELAY_LOOP_MS = 0; // change to slow down how often to read and graph value

int _circularBuffer[SCREEN_WIDTH]; //fast way to store values (rather than an ArrayList with remove calls)
int _curWriteIndex = 0; // tracks where we are in the circular buffer

// for tracking fps
unsigned long _totalFrameCount = 0;
unsigned long _startTimeStamp = 0;

// status bar
boolean _drawStatusBar = true; // change to show/hide status bar
boolean _drawAxis = true;
int _graphHeight = SCREEN_HEIGHT;

enum GraphPrimitive{
  POINTS,
  LINES
};

GraphPrimitive _drawGraphPrimitive = LINES;

enum GraphData{
  X_DATA,
  Y_DATA,
  Z_DATA,
  MAG
};

// You can change this to X_DATA, Y_DATA, Z_DATA, or MAG
// to display those respective values
GraphData _graphData = X_DATA; 

void setup() {
  Serial.begin(9600);

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3D)) { // Address 0x3D for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    for (;;); // Don't proceed, loop forever
  }

  // Clear the buffer
  display.clearDisplay();

  display.setTextSize(1);
  display.setTextColor(WHITE, BLACK);
  display.setCursor(0, 0);
  display.println("Screen initialized!");
  display.display();
  delay(250);

  display.println("Initializing accelerometer...");
  if (! lis.begin(0x18)) {   // change this to 0x19 for alternative i2c address
    Serial.println("Couldn't start");
    while (1) yield();
  }
  Serial.println("LIS3DH found!");

  lis.setRange(LIS3DH_RANGE_4_G);   // 2, 4, 8 or 16 G!

  Serial.print("Range = ");
  Serial.print(2 << lis.getRange());
  Serial.println("G");

  if(_drawStatusBar){
    _graphHeight = SCREEN_HEIGHT - 10;
  }
}

void loop() {
  if(_startTimeStamp == 0){
    _startTimeStamp = millis();
  }
  
  lis.read(); 
  int sensorVal = 0;

  // We can graph the X, Y, or Z accelerometer data
  // or the magnitude of the signal
  switch(_graphData){
    case X_DATA:
      sensorVal = lis.x;
      break;
    case Y_DATA:
      sensorVal = lis.y;
      break;
    case Z_DATA:
      sensorVal = lis.z;
      break;
    case MAG:
      long x = lis.x;
      long y = lis.y;
      long z = lis.z;
      double mag = sqrt(x * x + y * y + z * z);     
      sensorVal = mag;
      Serial.print("mag: ");
      Serial.print(mag);
      Serial.print(" sensorVal: ");
      Serial.println(sensorVal); 
      break;
  }

  _circularBuffer[_curWriteIndex++] = sensorVal;

  if(sensorVal < _minSensorVal){
    _minSensorVal = sensorVal;
  }

  if(sensorVal > _maxSensorVal){
    _maxSensorVal = sensorVal;
  }
      
  if(_curWriteIndex >= display.width()){
    _curWriteIndex = 0;
  }

  display.clearDisplay();

  // Draw the status bar
  if(_drawStatusBar){
    drawStatusBar(sensorVal);
  }

  // Draw the axis
  if(_drawAxis){
    drawAxis();
  }
  
  // Draw the line graph
  drawLineGraph();

  // Render buffer to screen
  display.display();
  _totalFrameCount++;

  if(DELAY_LOOP_MS > 0){
    delay(DELAY_LOOP_MS);
  }
}

void drawStatusBar(int sensorVal){
  // Draw current val
  display.setCursor(0, 0);

  switch(_graphData){
    case X_DATA:
      display.print("X: ");
      break;
    case Y_DATA:
      display.print("Y: ");
      break;
    case Z_DATA:
      display.print("Z: ");
      break;
    case MAG:
      display.print("MAG: ");     
      break;
  }
  
  display.print(sensorVal);

  if(_totalFrameCount > 0){
    int16_t  x1, y1;
    uint16_t w, h;
    unsigned long elapsedTime = millis() - _startTimeStamp;
    float fps = _totalFrameCount / (elapsedTime / 1000.0);
    display.getTextBounds("XX.XX fps", 0, 0, &x1, &y1, &w, &h);
    display.setCursor(display.width() - w, 0);
    display.print(fps);
    display.print(" fps");
  }
}

// Draw the axis
void drawAxis(){
  int y = getYPixelFromSensorVal(0);

  // draw dotted line
  for(int x = 0; x < display.width(); x += 5){
    display.drawPixel(x, y, SSD1306_WHITE);
  }

  display.setCursor(0, y - 9);
  display.print("0");
}

// Draw the line graph
void drawLineGraph(){
  int xPos = 0; 
  int lastSensorVal = 0;
  for (int i = _curWriteIndex; i < display.width(); i++){
    // draw the line
    if(_drawGraphPrimitive == POINTS){
      int sensorVal = _circularBuffer[i];
      drawValue(xPos, sensorVal);
    }else{
      if(xPos > 0){
        int x1 = xPos - 1;
        int x2 = xPos;
        int sensorVal1 = _circularBuffer[i - 1];
        int sensorVal2 = _circularBuffer[i];
        lastSensorVal = sensorVal2;
        drawLine(x1, sensorVal1, x2, sensorVal2);
      }
    }
    
    xPos++;
  }
  
  for(int i = 0; i < _curWriteIndex; i++){
    //draw the line
    if(_drawGraphPrimitive == POINTS){
      int sensorVal = _circularBuffer[i];
      drawValue(xPos, sensorVal);
    }else{
      int x1 = xPos - 1;
      int x2 = xPos;
      int sensorVal1 = 0;
      if(i == 0){
        sensorVal1 = lastSensorVal;
      }else{
        sensorVal1 = _circularBuffer[i - 1];
      }
      
      int sensorVal2 = _circularBuffer[i];
      drawLine(x1, sensorVal1, x2, sensorVal2);
    }
    
    xPos++;;
  }
}

// Draws a line between two sensor values at the given x positions
void drawLine(int x1, int sensorVal1, int x2, int sensorVal2){
  int y1 = getYPixelFromSensorVal(sensorVal1);
  int y2 = getYPixelFromSensorVal(sensorVal2);

  display.drawLine(x1, y1, x2, y2, SSD1306_WHITE);
}

// Get the y-pixel position for the sensor value
int getYPixelFromSensorVal(int sensorVal){
  int lineHeight = map(sensorVal, _minSensorVal, _maxSensorVal, 0, _graphHeight);
  int yPixel = display.height() - lineHeight;
  return yPixel;
}

// Draws the value at the x pixel position
void drawValue(int xPos, int sensorVal){
  int lineHeight = map(sensorVal, _minSensorVal, _maxSensorVal, 0, _graphHeight);
  int yPos = display.height() - lineHeight;
  display.drawPixel(xPos, yPos, SSD1306_WHITE);
}
