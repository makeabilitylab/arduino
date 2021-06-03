
/**
 * Control a servo motor angle with a potentiometer input and output
 * the angle on an OLED display
 * 
 * Based on:
 * https://learn.adafruit.com/adafruit-arduino-lesson-14-servo-motors
 * 
 * See the Arduino servo library:
 * https://www.arduino.cc/reference/en/libraries/servo/
 * 
 * For our OLED tutorials, see:
 * https://makeabilitylab.github.io/physcomp/advancedio/oled.html
 * 
 * By Jon E. Froehlich
 * @jonfroehlich
 * http://makeabilitylab.io
 *
 */
#include <Servo.h> 

// Includes for OLED
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET     4 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 _display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

const int POTENTIOMETER_INPUT_PIN = A0;  
const int SERVO_OUTPUT_PIN = 9;
const int MAX_ANALOG_VAL = 1023;
Servo _servo; 
 
void setup() 
{ 
  Serial.begin(9600);
  
  _servo.attach(SERVO_OUTPUT_PIN);  

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!_display.begin(SSD1306_SWITCHCAPVCC, 0x3D)) { // Address 0x3D for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }

  _display.clearDisplay();      
  _display.setTextColor(SSD1306_WHITE); // Draw white text
  _display.setCursor(0, 0);     // Start at top-left corner
} 
 
void loop() 
{ 
  int potVal = analogRead(POTENTIOMETER_INPUT_PIN); 
  int servoAngle = map(potVal, 0, MAX_ANALOG_VAL, 0, 180);
  _servo.write(servoAngle); 

  // Display the angle of the servo on the OLED
  _display.clearDisplay(); 
  _display.setTextSize(4);
  int16_t x1, y1;
  uint16_t textWidth, textHeight;
  String strAngle = (String)servoAngle;
  _display.getTextBounds(strAngle, 0, 0, &x1, &y1, &textWidth, &textHeight);
  uint16_t yText = _display.height() / 2 - textHeight / 2;
  uint16_t xText = _display.width() / 2 - textWidth / 2;
  _display.setCursor(xText, yText);
  _display.print(strAngle);
  _display.display();

  delay(10);
} 
