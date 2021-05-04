
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

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET     4 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 _display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

const int DELAY_LOOP_MS = 5; // change to slow down how often to read and graph value

// Ball variables
const int _ballRadius = 5;
int _xBall = 0;
int _yBall = 0;
int _xSpeed = 0;
int _ySpeed = 0;

// for tracking fps
float _fps = 0;
unsigned long _frameCount = 0;
unsigned long _fpsStartTimeStamp = 0;

// status bar
const boolean _drawStatusBar = true; // change to show/hide status bar

void setup() {
  Serial.begin(9600);

  initializeOledAndShowStartupScreen();

  // Initialize ball to center of screen
  _xBall = _display.width() / 2;
  _yBall = _display.height() / 2;

  // Gets a random long between min and max - 1
  // https://www.arduino.cc/reference/en/language/functions/random-numbers/random/
  _xSpeed = random(1, 4);
  _ySpeed = random(1, 4);

  //Serial.println((String)"_xBall:" + _xBall + " _xBall:" + _xBall + " _xSpeed:" + _xSpeed + " _ySpeed:" + _ySpeed);
  _fpsStartTimeStamp = millis();
}

void loop() {

  _display.clearDisplay();

  if(_drawStatusBar){
    drawStatusBar();
  }

  calcFrameRate();
  
  // Update ball based on speed location
  _xBall += _xSpeed;
  _yBall += _ySpeed;

  // Check for ball bounce
  if(_xBall - _ballRadius <= 0 || _xBall + _ballRadius >= _display.width()){
    _xSpeed = _xSpeed * -1; // reverse x direction
  }
  
  if(_yBall - _ballRadius <= 0 || _yBall + _ballRadius >= _display.height()){
    _ySpeed = _ySpeed * -1; // reverse y direction
  }

  // Draw circle
  _display.drawCircle(_xBall, _yBall, _ballRadius, SSD1306_WHITE);
  
  // Render buffer to screen
  _display.display();

  if(DELAY_LOOP_MS > 0){
    delay(DELAY_LOOP_MS);
  }

  //Serial.println((String)"_xBall:" + _xBall + " _xBall:" + _xBall + " _xSpeed:" + _xSpeed + " _ySpeed:" + _ySpeed);
}

/**
 * Call this from setup() to initialize the OLED screen
 */
void initializeOledAndShowStartupScreen(){
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
void drawStatusBar() {

  // Draw frame count
  int16_t x1, y1;
  uint16_t w, h;
  _display.getTextBounds("XX.XX fps", 0, 0, &x1, &y1, &w, &h);
  _display.setCursor(_display.width() - w, 0);
  _display.print(_fps);
  _display.print(" fps");
}
