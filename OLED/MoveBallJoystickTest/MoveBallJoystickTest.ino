/**
 * Tests the ParallaxJoystick library
 *  
 * This code requires the Shape.hpp and ParallaxJoystick.hpp from the MakeabilityLab_Arduino_Library
 * 
 * To install and use these files:
 * 1. Get it from here: https://github.com/makeabilitylab/arduino/tree/master/MakeabilityLab_Arduino_Library
 * 2. Move the entire folder to the Arduino libraries folder on your computer.
 *   - On my Windows box, this is C:\Users\jonfr\Documents\Arduino\libraries
 *   - On Mac, it's: /Users/jonf/Documents/Arduino/libraries
 * 3. Then include the relevant libraries via #include <libraryname.h> or <libraryname.hpp>
 * 
 * This code has been tested and works on: 
 *  - the Adafruit Huzzah32 ESP32
 *  - Arduino Leonardo
 * 
 * For our OLED tutorials, see:
 * https://makeabilitylab.github.io/physcomp/
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

#include <Shape.hpp>
#include <ParallaxJoystick.hpp>

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED _display width, in pixels
#define SCREEN_HEIGHT 64 // OLED _display height, in pixels

// Declaration for an SSD1306 _display connected to I2C (SDA, SCL pins)
#define OLED_RESET     4 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 _display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

const int DELAY_LOOP_MS = 5;

// Change these depending on where you have your joystick hooked up!
const int JOYSTICK_UPDOWN_PIN = A5;
const int JOYSTICK_LEFTRIGHT_PIN = A0;

// Change to 4095 for 12-bit ADCs like ESP32 and 1023 for 10-bit ADCs like Arduino Uno/Leonardo
const int MAX_ANALOG_VAL = 4095; 

const enum JoystickYDirection JOYSTICK_Y_DIR = RIGHT;

ParallaxJoystick _joystick(JOYSTICK_UPDOWN_PIN, JOYSTICK_LEFTRIGHT_PIN, MAX_ANALOG_VAL, JOYSTICK_Y_DIR);
Ball _ball(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, 5);

// for tracking fps
float _fps = 0;
unsigned long _frameCount = 0;
unsigned long _fpsStartTimeStamp = 0;

// status bar
const boolean _drawStatusBar = true; // change to show/hide status bar

void setup() {
  Serial.begin(9600);

  pinMode(JOYSTICK_LEFTRIGHT_PIN, INPUT);

  initializeOledAndShowStartupScreen();

  _ball.setSpeed(0, 0);
  _fpsStartTimeStamp = millis();

  Serial.println((String)"Joystick max analog val: " + _joystick.getMaxAnalogValue() + " Center: " + _joystick.getCenter());
}

void loop() {
  _display.clearDisplay();
  
  if(_drawStatusBar){
    drawStatusBar();
  }
  
  calcFrameRate();
  
  // Read new data
  _joystick.read();

  // Update ball based on joystick location
  int upDownVal = _joystick.getUpDownVal();
  int leftRightVal = _joystick.getLeftRightVal();

  // Translate joystick movement into amount of pixels to move
  int yMovementPixels = map(upDownVal, 0, _joystick.getMaxAnalogValue() + 1, -1, 2);
  int xMovementPixels = map(leftRightVal, 0, _joystick.getMaxAnalogValue() + 1, -1, 2);

  //Serial.println((String)"DEFINES UpDownPin: " + JOYSTICK_UPDOWN_PIN+ " LeftRightPin: " + JOYSTICK_LEFTRIGHT_PIN);
  //Serial.println((String)"Joystick UpDownPin: " + _joystick.getUpDownPin() + " LeftRightPin: " + _joystick.getLeftRightPin() );
  //Serial.println((String)"Joystick max analog val: " + _joystick.getMaxAnalogValue() + " Center: " + _joystick.getCenter());
  
  // Set the new ball location based on joystick position
  Serial.println((String)"upDownVal:" + upDownVal + " leftRightVal:" + leftRightVal + " xMovementPixels:" + xMovementPixels + " yMovementPixels:" + yMovementPixels);
  _ball.setLocation(_ball.getX() + xMovementPixels, _ball.getY() - yMovementPixels);
  _ball.forceInside(0, 0, _display.width(), _display.height());

  _ball.draw(_display);

  // Render buffer to screen
  _display.display();

  if(DELAY_LOOP_MS > 0){
    delay(DELAY_LOOP_MS);
  }
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
