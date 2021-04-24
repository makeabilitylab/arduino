#include <Shape.hpp>;
#include <ParallaxJoystick.hpp>;

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED _display width, in pixels
#define SCREEN_HEIGHT 64 // OLED _display height, in pixels

// Declaration for an SSD1306 _display connected to I2C (SDA, SCL pins)
#define OLED_RESET     4 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 _display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

const int DELAY_LOOP_MS = 5; 
const int JOYSTICK_UPDOWN_PIN = A1;
const int JOYSTICK_LEFTRIGHT_PIN = A0;
const int MAX_ANALOG_VAL = 1023;
const enum JoystickYDirection JOYSTICK_Y_DIR = RIGHT;

ParallaxJoystick _joystick(JOYSTICK_UPDOWN_PIN, JOYSTICK_LEFTRIGHT_PIN, MAX_ANALOG_VAL, JOYSTICK_Y_DIR);
Ball _ball(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2, 5);

// for tracking fps
unsigned long _totalFrameCount = 0;
unsigned long _startTimeStamp = 0;

// status bar
const boolean _drawStatusBar = true; // change to show/hide status bar

void setup() {
  Serial.begin(9600);

  // SSD1306_SWITCHCAPVCC = generate _display voltage from 3.3V internally
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

  _ball.setSpeed(0, 0);
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

  // Read new data
  _joystick.read();

  // Update ball based on joystick location
  int upDownVal = _joystick.getUpDownVal();
  int leftRightVal = _joystick.getLeftRightVal();

  int yMovementPixels = map(upDownVal, 0, _joystick.getMaxAnalogValue() + 1, -1, 2);
  int xMovementPixels = map(leftRightVal, 0, _joystick.getMaxAnalogValue() + 1, -1, 2);

  Serial.println((String)"upDownVal:" + upDownVal + " leftRightVal:" + leftRightVal + " xMovementPixels:" + xMovementPixels + " yMovementPixels:" + yMovementPixels);

  _ball.setLocation(_ball.getX() + xMovementPixels, _ball.getY() - yMovementPixels);
  _ball.forceInside(0, 0, _display.width(), _display.height());

  _ball.draw(_display);

  // Render buffer to screen
  _display.display();
  _totalFrameCount++;

  if(DELAY_LOOP_MS > 0){
    delay(DELAY_LOOP_MS);
  }
}
