#include <Wire.h>
#include <SPI.h>
#include <Shape.hpp>;
#include <ParallaxJoystick.hpp>;

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED _display width, in pixels
#define SCREEN_HEIGHT 64 // OLED _display height, in pixels

// Declaration for an SSD1306 _display connected to I2C (SDA, SCL pins)
#define OLED_RESET     4 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 _display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

const int JOYSTICK_UPDOWN_PIN = A1;
const int JOYSTICK_LEFTRIGHT_PIN = A0;

const char STR_LOADSCREEN_CREATOR[] = "Makeability Lab";
const char STR_LOADSCREEN_APP_NAME[] = "Collision Test!";

const int MAX_ANALOG_VAL = 1023;
const enum JoystickYDirection JOYSTICK_Y_DIR = RIGHT;

// Analog joystick
ParallaxJoystick _analogJoystick(JOYSTICK_UPDOWN_PIN, JOYSTICK_LEFTRIGHT_PIN, MAX_ANALOG_VAL, JOYSTICK_Y_DIR);

// Digital buttons (joystick)
const int UP_BUTTON_INPUT_PIN = 6;
const int DOWN_BUTTON_INPUT_PIN = 7;
const int LEFT_BUTTON_INPUT_PIN = 5;
const int RIGHT_BUTTON_INPUT_PIN = 4;

Ball _playerBall(20, 20, 3);

const int NUM_TARGET_SHAPES = 8;

// Dynamic array of target shapes. We'll create and initialize this in setup()
Shape **_targetShapes;

// You cannot create the shapes array like this because polymorphism won't work otherwise!
// Good reminder that C++ is not Java :)
// Shape _targetShapes[NUM_TARGET_SHAPES] = { Ball(15, 15, 10),
//                                           Rectangle(50, 30, 10, 5),
//                                           Rectangle(30, 50, 15, 10),
//                                           Rectangle(100, 10, 10, 10),
//                                           Rectangle(80, 40, 15, 5)
//                                         };
//


// for tracking fps
unsigned long _totalFrameCount = 0;
unsigned long _startTimeStamp = 0;

// status bar
const boolean _drawFrameCount = true; // change to show/hide status bar
const int DELAY_LOOP_MS = 5;
const int LOAD_SCREEN_SHOW_MS = 500;

void setup() {
  Serial.begin(9600);

  // SSD1306_SWITCHCAPVCC = generate _display voltage from 3.3V internally
  if (!_display.begin(SSD1306_SWITCHCAPVCC, 0x3D)) { // Address 0x3D for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    for (;;); // Don't proceed, loop forever
  }

  // if analog input pin 5 is unconnected, random analog
  // noise will cause the call to randomSeed() to generate
  // different seed numbers each time the sketch runs.
  // randomSeed() will then shuffle the random function.
  randomSeed(analogRead(A5));
  
  // In order for polymorphism to work, we need to use pointers
  // See: https://stackoverflow.com/a/45176572
  // So, we dynamically create our shapes here
  _targetShapes = new Shape*[NUM_TARGET_SHAPES];
  for(int i = 0; i < NUM_TARGET_SHAPES; i++){
    
    // Create dimensions of shape
    int randWidth = random(3, 25);
    int randHeight = random(3, 25);
    int randX = random(0, _display.width() - randWidth / 2); // don't let objects go off screen
    int randY = random(0, _display.height() - randHeight / 2);
    
    int randShape = random(0, 9); // simple way to randomly choose what shape to make

    // Create random shape
    if(randShape <= 4){
      // create slightly fewer balls than rects
      // balls want center pts rather than top left 
      int radius = max(2, randWidth / 2);
      _targetShapes[i] = new Ball(randX + radius, randY + radius, radius);
    }else{
      _targetShapes[i] = new Rectangle(randX, randY, randWidth, randHeight);
    }
  }

  // Show load screen
  showLoadScreen();

  _playerBall.setDrawFill(true);
  _playerBall.setCenter(_display.width()/2, _display.height()/2);
}

void showLoadScreen(){
  // Clear the buffer
  _display.clearDisplay();
  
  // Show load screen
  _display.setTextSize(1);
  _display.setTextColor(WHITE, BLACK);

  int16_t x1, y1;
  uint16_t w, h;
  _display.getTextBounds(STR_LOADSCREEN_CREATOR, 0, 0, &x1, &y1, &w, &h);
  _display.setCursor(_display.width() / 2 - w / 2, 15);
  _display.print(STR_LOADSCREEN_CREATOR);

  _display.setTextSize(2);
  _display.getTextBounds(STR_LOADSCREEN_APP_NAME, 0, 0, &x1, &y1, &w, &h);
  _display.setCursor(_display.width() / 2 - w / 2, 25);
  _display.print(STR_LOADSCREEN_APP_NAME);

  _display.display();
  delay(LOAD_SCREEN_SHOW_MS);
  _display.clearDisplay();
  _display.setTextSize(1);
  
}

void loop() {
  if(_startTimeStamp == 0){
    _startTimeStamp = millis();
  }
  
  _display.clearDisplay();

  // Draw frame count
  if(_drawFrameCount && _totalFrameCount > 0){
    int16_t x1, y1;
    uint16_t w, h;
    unsigned long elapsedTime = millis() - _startTimeStamp;
    float fps = _totalFrameCount / (elapsedTime / 1000.0);
    _display.getTextBounds("XX.XX fps", 0, 0, &x1, &y1, &w, &h);
    _display.setCursor(_display.width() - w, 0);
    _display.print(fps);
    _display.print(" fps");
  }

  // Read analog joystick to control player ball
  _analogJoystick.read();
  int upDownVal = _analogJoystick.getUpDownVal();
  int leftRightVal = _analogJoystick.getLeftRightVal();

  int yMovementPixels = map(upDownVal, 0, _analogJoystick.getMaxAnalogValue() + 1, -4, 5);
  int xMovementPixels = map(leftRightVal, 0, _analogJoystick.getMaxAnalogValue() + 1, -10, 11);

  _playerBall.setLocation(_playerBall.getX() + xMovementPixels, _playerBall.getY() - yMovementPixels);
  _playerBall.forceInside(0, 0, _display.width(), _display.height()); // don't let ball off screen

  // Cycle through all target shapes and check for collisions with player ball
  // Then draw to screen
  for (int i = 0; i < NUM_TARGET_SHAPES; i++) {
    Shape *shape = _targetShapes[i];
    shape->draw(_display);
    
    //Serial.println("shape->getName(): " + shape->getName());
    //Serial.println("shape->toString(): " + shape->toString());

    // If the target shape overlaps with playerball, fill it
    if(shape->overlaps(_playerBall)){
      shape->setDrawFill(true);
    }else{
      shape->setDrawFill(false);
    }
    shape->draw(_display);
  }

  // draw the player ball to the screen
  _playerBall.draw(_display);

  _display.display();
  _totalFrameCount++;
  
  if(DELAY_LOOP_MS > 0){
    delay(DELAY_LOOP_MS);
  }
}
