/**
 * Simple example that animates an increasingly large circle on screen
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

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET     4 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 _display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

const int MAX_ANALOG_VOLTAGE = 4095;
const int JOYSTICK_UPDOWN_INPUT_PIN = A5;
const int JOYSTICK_LEFTRIGHT_INPUT_PIN = A0;
const int JOYSTICK_CIRCLE_RADIUS = 2;

// For ESP32 compatibility
//#ifndef max
//#define max(a,b) (((a) > (b)) ? (a) : (b))
//#endif

void setup() {
  Serial.begin(9600);

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!_display.begin(SSD1306_SWITCHCAPVCC, 0x3D)) { // Address 0x3D for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }
  
  _display.clearDisplay();
  _display.setRotation(3);
}

void loop() {
  // Clear the buffer
  _display.clearDisplay();

  int outerCircleRadius = (min(_display.width(), _display.height()) / 2) - 1;
  int outerCircleDiameter = outerCircleRadius * 2;
  int outerX = _display.width() / 2;
  int outerY = _display.height() / 2;
  _display.drawCircle(outerX, outerY, outerCircleRadius, SSD1306_WHITE);

  int upDownVal = analogRead(JOYSTICK_UPDOWN_INPUT_PIN);
  delay(1);
  int leftRightVal = analogRead(JOYSTICK_LEFTRIGHT_INPUT_PIN);
  
  int joystickY = map(upDownVal, 0, MAX_ANALOG_VOLTAGE, 0, outerCircleRadius);
  int joystickX = map(leftRightVal, 0, MAX_ANALOG_VOLTAGE, 0, outerCircleRadius);
  _display.fillCircle(outerX + , joystickY, JOYSTICK_CIRCLE_RADIUS, SSD1306_WHITE); 

  // draw text at x,y position
  int16_t x1, y1;
  uint16_t textWidth, textHeight;
  _display.setTextSize(1);    
  _display.setTextColor(SSD1306_WHITE); // Draw white text  
  String strRawVals = (String)upDownVal + ", " + leftRightVal;
  _display.getTextBounds(strRawVals, 0, 0, &x1, &y1, &textWidth, &textHeight);
//  uint16_t yText = _display.height() / 2 - textHeight / 2;
//  uint16_t xText = _display.width() / 2 - textWidth / 2;
  uint16_t yText = joystickY;
  uint16_t xText = joystickX + JOYSTICK_CIRCLE_RADIUS + 1;
  _display.setCursor(xText, yText);
  _display.print("Hello!");

  // Also do this with serial.print
  // Print X min/max 
  // Print Y min/max
  // Print cur X, Y (and dodge edges on printing)
  
  _display.display();
  
  delay(5);
}
