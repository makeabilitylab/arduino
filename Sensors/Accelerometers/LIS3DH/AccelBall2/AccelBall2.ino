
/**
 *  Simple example of using accelerometer to move ball
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
 *  See: 
 *  - https://natureofcode.com/book/chapter-2-forces/#chapter02_section2
 *  - https://www.youtube.com/watch?v=MkXoQVWRDJs
 *
 */

#include <Wire.h>
#include <SPI.h>

// For accelerometer
#include <Adafruit_LIS3DH.h>
#include <Adafruit_Sensor.h>

// For graphics libraries
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include <Shape.hpp>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET     4 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 _display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Used for LIS3DH hardware & software SPI
#define LIS3DH_CS 10
Adafruit_LIS3DH _lis3dh = Adafruit_LIS3DH();

const int DELAY_LOOP_MS = 100; // change to slow down how often to read and graph value
const int MIN_ACCEL_VAL = -32768;
const int MAX_ACCEL_VAL = 32767;

const int MAX_SPEED_X = 3; // pixels/sec
const int MAX_SPEED_Y = 3; // pixels/sec

Circle _circleNaive(50, 30, 4, true);
Circle _circlePhysics(50, 30, 4, false);

float _xVelocity = 0;
float _yVelocity = 0;

float _x = 0;
float _y = 0;

long _lastDrawTimestamp = 0;

const int MILLISECONDS_IN_SEC = 1000;
const long MICROSECONDS_IN_SEC = 1000 * (long)MILLISECONDS_IN_SEC;

template <class T>
class ShapeF {

  protected:
    T _x;
    T _y;
    T _width;
    T _height;
    bool _drawFill = DEFAULT_DRAW_FILL;
    bool _drawBoundingBox = false;

  public:
    const bool DEFAULT_DRAW_FILL = false;

    ShapeF(T x, T y, T width, T height)
      : ShapeF(x, y, width, height, DEFAULT_DRAW_FILL){
        // purposefully empty
      }

    ShapeF(T x, T y, T width, T height, bool drawFillOn) {
      _x = x;
      _y = y;
      _width = width;
      _height = height;
      _drawFill = drawFillOn;
    }
};

template <typename T>
class RectangleF : public ShapeF<T>{
  public: 
    RectangleF(T x, T y, T width, T height)
        : ShapeF<T>(x, y, width, height, true){
          // purposefully empty
        }
};

ShapeF<float> _shape(2, 3, 10, 15);

RectangleF<float> _rect(2.2, 3.3, 10.1, 15.2);

using RectangleF2 = RectangleF<float>;

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
  delay(250);

  _display.println("Initializing accelerometer...");
  if (!_lis3dh.begin(0x18)) {   // change this to 0x19 for alternative i2c address
    Serial.println("Couldn't start");
    while (1) yield();
  }
  Serial.println("LIS3DH found!");

  _lis3dh.setRange(LIS3DH_RANGE_2_G);   // 2, 4, 8 or 16 G!

  Serial.print("Range = ");
  Serial.print(2 << _lis3dh.getRange());
  Serial.println("G");

  _circleNaive.setLocation(_display.width() / 2, _display.height() / 2);
  _circlePhysics.setLocation(_display.width() / 2, _display.height() / 2);
  _x = _circlePhysics.getX();
  _y = _circlePhysics.getY();
  _lastDrawTimestamp = micros();
}

void loop() {

  float deltaTimeInSecs = (micros() - _lastDrawTimestamp) / (float)MICROSECONDS_IN_SEC;
  _lastDrawTimestamp = micros();
  
  // clear display, prepare for next render
  _display.clearDisplay();

  // Use Adafruit Sensor Library to get a normalized reading
  // https://learn.adafruit.com/using-the-adafruit-unified-sensor-driver/how-does-it-work
  sensors_event_t event;
  _lis3dh.getEvent(&event);

  // Calculate new speed
  _xVelocity += (event.acceleration.x * deltaTimeInSecs);
  _yVelocity += (event.acceleration.y * deltaTimeInSecs);

//  if(_xVelocity < MAX_SPEED_X){
//    _xVelocity = -MAX_SPEED_X;
//  }else if(_xVelocity > MAX_SPEED_X){
//    _xVelocity = MAX_SPEED_X;
//  }
//
//  if(_yVelocity < MAX_SPEED_Y){
//    _yVelocity = -MAX_SPEED_Y;
//  }else if(_yVelocity > MAX_SPEED_Y){
//    _yVelocity = MAX_SPEED_Y;
//  }

  // Calc distance travelled in that time
  float xDistance = _xVelocity * deltaTimeInSecs;
  float yDistance = _yVelocity * deltaTimeInSecs;

  // Draw values to screen for x, y
  _display.setCursor(0, 0);
  _display.print("ACCEL X: ");
  _display.print(event.acceleration.x);
  _display.print(" Y: ");
  _display.print(event.acceleration.y);

  _display.setCursor(0, 9);
  _display.print("SPEED X: ");
  _display.print(_xVelocity);
  _display.print(" Y: ");
  _display.print(_yVelocity);

  _display.setCursor(0, 18);
  _display.print("DIST X: ");
  _display.print(xDistance);
  _display.print(" Y: ");
  _display.print(yDistance);

  //Add to position
  _x -= xDistance; 
  _y += yDistance;

  if(_x <= 0){
    _xVelocity = 0;
    _x = 0;
  }else if(_x >= _display.width()){
    _xVelocity = 0;
    _x = _display.width() - _circlePhysics.getWidth();
  }

  if(_y <= 0){
    _yVelocity = 0;
    _y = 0;
  }else if(_y >= _display.height()){
    _yVelocity = 0;
    _y = _display.height() - _circlePhysics.getHeight();
  }

  _circlePhysics.setLocation(_x, _y);
  _circlePhysics.draw(_display);

  // set new circle location based on accel
  int newX = _circleNaive.getX() + -event.acceleration.x;
  int newY = _circleNaive.getY() + event.acceleration.y;

  _circleNaive.setLocation(newX, newY);
  _circleNaive.forceInside(0, 0, _display.width(), _display.height());
  _circleNaive.draw(_display);
  
  // Render buffer to screen
  _display.display();
 
//  if(DELAY_LOOP_MS > 0){
//    delay(DELAY_LOOP_MS);
//  }
}
