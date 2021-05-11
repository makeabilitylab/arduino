
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

Circle _circle(50, 30, 4, true);

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

  _circle.setLocation(_display.width() / 2, _display.height() / 2);
}

void loop() {

  // clear display, prepare for next render
  _display.clearDisplay();

  // Read the accel
  //_lis3dh.read(); 

  // Use Adafruit Sensor Library to get a normalized reading
  // https://learn.adafruit.com/using-the-adafruit-unified-sensor-driver/how-does-it-work
  sensors_event_t event;
  _lis3dh.getEvent(&event);

  // Display the results (acceleration is measured in m/s^2) 
  //Serial.println((String)"SENSOR EVENT\tX: " + event.acceleration.x + " Y: " + event.acceleration.y + " Z: " + event.acceleration.z + " m/s^2");

  // Draw values to screen for x, y
  _display.setCursor(0, 0);
  _display.print("X: ");
  _display.print(event.acceleration.x);
  _display.print(" Y: ");
  _display.print(event.acceleration.y);

  // set new circle location based on accel
  int newX = _circle.getX() + -event.acceleration.x;
  int newY = _circle.getY() + event.acceleration.y;

  _circle.setLocation(newX, newY);

  _circle.forceInside(0, 0, _display.width(), _display.height());
  
  _circle.draw(_display);
  
  
  // Render buffer to screen
  _display.display();
 
  if(DELAY_LOOP_MS > 0){
    delay(DELAY_LOOP_MS);
  }
}
