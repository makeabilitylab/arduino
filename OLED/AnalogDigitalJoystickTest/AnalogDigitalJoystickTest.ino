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

ParallaxJoystick _analogJoystick(JOYSTICK_UPDOWN_PIN, JOYSTICK_LEFTRIGHT_PIN, MAX_ANALOG_VAL, JOYSTICK_Y_DIR);
Ball _analogBall(20, SCREEN_HEIGHT / 2, 8);
Ball _digitalBall(SCREEN_WIDTH - 20, SCREEN_HEIGHT / 2, 5);

const int UP_BUTTON_INPUT_PIN = 6;
const int DOWN_BUTTON_INPUT_PIN =7;
const int LEFT_BUTTON_INPUT_PIN = 5;
const int RIGHT_BUTTON_INPUT_PIN = 4;

// for tracking fps
float _fps = 0;
unsigned long _frameCount = 0;
unsigned long _fpsStartTimeStamp = 0;

// status bar
const boolean _drawStatusBar = true; // change to show/hide status bar

void setup() {
  Serial.begin(9600);

  pinMode(UP_BUTTON_INPUT_PIN, INPUT_PULLUP);
  pinMode(DOWN_BUTTON_INPUT_PIN, INPUT_PULLUP);
  pinMode(LEFT_BUTTON_INPUT_PIN, INPUT_PULLUP);
  pinMode(RIGHT_BUTTON_INPUT_PIN, INPUT_PULLUP);

  initializeOledAndShowStartupScreen();

  _analogBall.setSpeed(0, 0);
  _digitalBall.setSpeed(0, 0);
  _digitalBall.setDrawFill(true);
  _fpsStartTimeStamp = millis();
}

void loop() {

  _display.clearDisplay();

  if(_drawStatusBar){
    drawStatusBar();
  }

  calcFrameRate();
  

  // Read new digital joystick data
  int upButtonVal = digitalRead(UP_BUTTON_INPUT_PIN);
  int downButtonVal = digitalRead(DOWN_BUTTON_INPUT_PIN);
  int leftButtonVal = digitalRead(LEFT_BUTTON_INPUT_PIN);
  int rightButtonVal = digitalRead(RIGHT_BUTTON_INPUT_PIN);

  // Buttons are setup with internal pullups, so go low when pressed
  if(upButtonVal == LOW){
    _digitalBall.setY(_digitalBall.getY() - 1);
  }

  if(downButtonVal == LOW){
    _digitalBall.setY(_digitalBall.getY() + 1);
  }

  if(leftButtonVal == LOW){
    _digitalBall.setX(_digitalBall.getX() - 1);
  }

  if(rightButtonVal == LOW){
    _digitalBall.setX(_digitalBall.getX() + 1);
  }
  _digitalBall.forceInside(0, 0, _display.width(), _display.height());
  

  // Read analog joystick to control player ball
  _analogJoystick.read();
  int upDownVal = _analogJoystick.getUpDownVal();
  int leftRightVal = _analogJoystick.getLeftRightVal();

  // Translate joystick movement into amount of pixels to move
  int yMovementPixels = map(upDownVal, 0, _analogJoystick.getMaxAnalogValue() + 1, -4, 5);
  int xMovementPixels = map(leftRightVal, 0, _analogJoystick.getMaxAnalogValue() + 1, -10, 11);

  // Serial.println((String)"upDownVal:" + upDownVal + " leftRightVal:" + leftRightVal + " xMovementPixels:" + xMovementPixels + " yMovementPixels:" + yMovementPixels);

  _analogBall.setLocation(_analogBall.getX() + xMovementPixels, _analogBall.getY() - yMovementPixels);
  _analogBall.forceInside(0, 0, _display.width(), _display.height());

  int distanceBetweenBallCenterPts = Shape::distance(_digitalBall.getCenterX(), _digitalBall.getCenterY(), _analogBall.getCenterX(), _analogBall.getCenterY());
  // Serial.print((String)"digitalBall.CenterPt: (" + _digitalBall.getCenterX() + ", " + _digitalBall.getCenterY() + ")");
  // Serial.print((String)" analogBall.CenterPt: (" + _analogBall.getCenterX() + ", " + _analogBall.getCenterY() + ")");
  // Serial.println((String)" Distance: " + distanceBetweenBallCenterPts);
  _display.setCursor(0, 0);
  _display.print(distanceBetweenBallCenterPts);
  _display.print(" px");

  // If the balls touch, draw them both filled
  if(_digitalBall.overlaps(_analogBall)){
    _analogBall.setDrawFill(true);
  }else{
    _analogBall.setDrawFill(false);
  }

  // Draw the balls
  _digitalBall.draw(_display);
  _analogBall.draw(_display);

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
