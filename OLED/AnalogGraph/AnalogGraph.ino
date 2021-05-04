
/**
 * Graphs a single analog input value to an OLED _display. The graph is 
 * cleared each time SCREEN_WIDTH is reached. For a scrolling version,
 * see AnalogGraphScrolling.ino:
 * https://github.com/makeabilitylab/arduino/blob/master/OLED/AnalogGraphScrolling/AnalogGraphScrolling.ino
 *
 * Very similar to the Processing sketch by Tom Igoe:
 * https://www.arduino.cc/en/tutorial/graph
 * 
 * And many of my previous Processing graphs:
 * https://github.com/jonfroehlich/ArduinoGraphTutorials
 * 
 * By Jon E. Froehlich
 * @jonfroehlich
 * http://makeabilitylab.io
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
Adafruit_SSD1306 _display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

const int ANALOG_INPUT_PIN = A0;
const int MIN_ANALOG_INPUT = 0;
const int MAX_ANALOG_INPUT = 1023;
const int DELAY_LOOP_MS = 5; // change to slow down how often to read and graph value

int _xPos = 0;

// for tracking fps
float _fps = 0;
unsigned long _frameCount = 0;
unsigned long _fpsStartTimeStamp = 0;

// status bar
boolean _drawStatusBar = true; // change to show/hide status bar
int _graphHeight = SCREEN_HEIGHT;

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
  delay(50);
  _display.clearDisplay();

  if(_drawStatusBar){
    _graphHeight = SCREEN_HEIGHT - 10;
  }

  _fpsStartTimeStamp = millis();
}

void loop() {

  // If the x-position of is off the right side of the screen, clear the display
  // and start the graph over
  if (_xPos >= _display.width()) {
    _xPos = 0;
    _display.clearDisplay();
  }

  // Read the analog voltage value
  int analogVal = analogRead(ANALOG_INPUT_PIN);
  
  if(_drawStatusBar){
    drawStatusBar(analogVal);
  }

  // Draw the line for the given sensor value
  int lineHeight = map(analogVal, MIN_ANALOG_INPUT, MAX_ANALOG_INPUT, 0, _graphHeight);
  int yPos = _display.height() - lineHeight;

  // For purely horizontal and vertical lines, there are optimized line-drawing functions
  // that avoid angular calculations. 
  // See: https://learn.adafruit.com/adafruit-gfx-graphics-library/graphics-primitives#drawing-lines-2002778-4
  _display.drawFastVLine(_xPos++, yPos, lineHeight, SSD1306_WHITE);
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
 * Draws the status bar at top of screen with fps and analog value
 */
void drawStatusBar(int analogVal) {

   // erase status bar by drawing all black
  _display.fillRect(0, 0, _display.width(), 8, SSD1306_BLACK); 
  
  // Draw current val
  _display.setCursor(0, 0);
  _display.print(analogVal);

  // Draw frame count
  int16_t x1, y1;
  uint16_t w, h;
  _display.getTextBounds("XX.XX fps", 0, 0, &x1, &y1, &w, &h);
  _display.setCursor(_display.width() - w, 0);
  _display.print(_fps);
  _display.print(" fps");
}
