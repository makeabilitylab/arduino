
/**
 *  Draws a scrolling graph of a single analog input (set to A0) on the OLED display
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

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET     4 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

const int ANALOG_INPUT_PIN = A0;
const int MIN_ANALOG_INPUT = 0;
const int MAX_ANALOG_INPUT = 1023;
const int DELAY_LOOP_MS = 5; // change to slow down how often to read and graph value

int _circularBuffer[SCREEN_WIDTH]; //fast way to store values (rather than an ArrayList with remove calls)
int _curWriteIndex = 0; // tracks where we are in the circular buffer

// for tracking fps
unsigned long _totalFrameCount = 0;
unsigned long _startTimeStamp = 0;

// status bar
boolean _drawStatusBar = true; // change to show/hide status bar
int _graphHeight = SCREEN_HEIGHT;

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
  delay(500);
  display.clearDisplay();

  if(_drawStatusBar){
    _graphHeight = SCREEN_HEIGHT - 10;
  }
}

void loop() {
  if(_startTimeStamp == 0){
    _startTimeStamp = millis();
  }
  
  int analogVal = analogRead(ANALOG_INPUT_PIN);
  Serial.println(analogVal);
  _circularBuffer[_curWriteIndex++] = analogVal;
      
  if(_curWriteIndex >= display.width()){
    _curWriteIndex = 0;
  }

  display.clearDisplay();
  //display.display();
  
  if(_drawStatusBar){
    // Draw current val
    display.setCursor(0, 0);
    display.print(analogVal);
  
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
  
  // Draw the line graph
  int xPos = 0; 
  for (int i = _curWriteIndex; i < display.width(); i++){
    // draw the line
    int analogVal = _circularBuffer[i];
    drawLine(xPos, analogVal);
    xPos++;
  }
  
  for(int i = 0; i < _curWriteIndex; i++){
    //draw the line
    int analogVal = _circularBuffer[i];
    drawLine(xPos, analogVal);
    xPos++;;
  }
  
  display.display();
  _totalFrameCount++;
  delay(DELAY_LOOP_MS);
}

void drawLine(int xPos, int analogVal){
  int lineHeight = map(analogVal, MIN_ANALOG_INPUT, MAX_ANALOG_INPUT, 0, _graphHeight);
  int yPos = display.height() - lineHeight;
  display.drawFastVLine(xPos, yPos, lineHeight, SSD1306_WHITE);
}
