/**
 * Control a servo motor angle with either a potentiometer input or serial input, 
 * which is selectable by button input on Pin 4. If serial is selected, the
 * built-in LED turns on. Outputs the servo angles and which input modes are 
 * selected on the OLED using i2c.
 * 
 * The serial stream can either be an integer between 0 - 180 (inclusive) or a floating point 
 * between [0, 1] (inclusive). The latter will be converted to an angle between 0 - 180
 * Each value must be terminated by end of line.
 * 
 * See also our non-OLED version:
 * https://github.com/makeabilitylab/arduino/tree/master/Serial/ServoPotWithSerialIn
 * 
 * See the Arduino servo library:
 * https://www.arduino.cc/reference/en/libraries/servo/
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

const int INPUT_MODE_SELECTION_PIN = 4;
const int MODE_INDICATOR_LED_PIN = LED_BUILTIN;
const int POTENTIOMETER_INPUT_PIN = A0;  
const int SERVO_OUTPUT_PIN = 9;
const int MAX_ANALOG_VAL = 1023;

const int MIN_SERVO_ANGLE = 0;
const int MAX_SERVO_ANGLE = 180;

Servo _servo; 

enum ServoInputMode {
  POTENTIOMETER_INPUT,
  SERIAL_INPUT,
  NUM_INPUT_MODES
};

ServoInputMode _servoInputMode = SERIAL_INPUT;
int _lastModeSelectionButtonVal = HIGH;
int _serialServoAngle = MIN_SERVO_ANGLE;
 
void setup() 
{ 
  Serial.begin(115200);
  pinMode(INPUT_MODE_SELECTION_PIN, INPUT_PULLUP);
  pinMode(MODE_INDICATOR_LED_PIN, OUTPUT);
  
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
  // Check input mode for servo motor position: either potentiometer or serial
  int modeSelectionButtonVal = digitalRead(INPUT_MODE_SELECTION_PIN);
  if(modeSelectionButtonVal == LOW && modeSelectionButtonVal != _lastModeSelectionButtonVal){
    _servoInputMode = (ServoInputMode)(_servoInputMode + 1);

    if(_servoInputMode >= NUM_INPUT_MODES){
      _servoInputMode = POTENTIOMETER_INPUT;
    }
  }
  _lastModeSelectionButtonVal = modeSelectionButtonVal;

  // Set indicator light
  if(_servoInputMode == SERIAL_INPUT){
    digitalWrite(MODE_INDICATOR_LED_PIN, HIGH);
  }else{
    digitalWrite(MODE_INDICATOR_LED_PIN, LOW);
  }

  // Check if serial data exists, if so read it in
  if(Serial.available() > 0){
    // Read data off the serial port until we get to the endline delimiter ('\n')
    // Store all of this data into a string
    String rcvdSerialData = Serial.readStringUntil('\n'); 

    // We accept either integers between 0 and 180 or floats. Floats must always
    // have a period.
    int indexOfDecimal = rcvdSerialData.indexOf('.');
    if(indexOfDecimal != -1){
      float serialServoAngleF = rcvdSerialData.toFloat();
      _serialServoAngle = MIN_SERVO_ANGLE + (int)(serialServoAngleF * (MAX_SERVO_ANGLE - MIN_SERVO_ANGLE));
      //_serialServoAngle = (MAX_SERVO_ANGLE - _serialServoAngle) + MIN_SERVO_ANGLE; // reverse it
    }else{
      _serialServoAngle = rcvdSerialData.toInt();
    }

    _serialServoAngle = constrain(_serialServoAngle, MIN_SERVO_ANGLE, MAX_SERVO_ANGLE);

    // Echo back data
    Serial.print("# Arduino Received: '");
    Serial.print(rcvdSerialData);
    Serial.print("' Converted to: ");
    Serial.println(_serialServoAngle);
  }
  
  // Read pot value and convert to servo angle
  int potVal = analogRead(POTENTIOMETER_INPUT_PIN); 
  int potServoAngle = map(potVal, 0, MAX_ANALOG_VAL, MIN_SERVO_ANGLE, MAX_SERVO_ANGLE);

  // Set servo angle either based on pot value or serial value
  if(_servoInputMode == POTENTIOMETER_INPUT){
    _servo.write(potServoAngle);  
  }else{
    _servo.write(_serialServoAngle);  
  }

  // Print out data to the OLED
  _display.clearDisplay();
  drawServoAngle(0, "POT", potServoAngle, _servoInputMode == POTENTIOMETER_INPUT);
  drawServoAngle(_display.width() / 2, "SERIAL", 
        _serialServoAngle, _servoInputMode == SERIAL_INPUT);

  // draw dotted line between POT and SERIAL readings
  const int xScreenMidpoint = _display.width() / 2;
  for(int y = 0; y < _display.height(); y += 3){
    _display.drawPixel(xScreenMidpoint, y, SSD1306_WHITE);
  }
   
  _display.display();  
} 

void drawServoAngle(int xLeft, String strText, int servoAngle, bool isSelected){
  int16_t x1, y1;
  uint16_t textWidth, textHeight;
  
  _display.setTextSize(1);
  int halfScreenSize = _display.width() / 2;
  _display.getTextBounds(strText, 0, 0, &x1, &y1, &textWidth, &textHeight);
  int xText = xLeft + halfScreenSize / 2 - textWidth / 2;
  int yText = 8;
  _display.setCursor(xText, yText);
  _display.print(strText);

  const int bbMargin = 2;
  if(isSelected){
    const int roundedRectRadius = 2;
    _display.drawRoundRect(xText - bbMargin - 1, yText - bbMargin, 
              textWidth + 2 * bbMargin, textHeight + 2 * bbMargin, 
              roundedRectRadius, SSD1306_WHITE);
  }

  _display.setTextSize(3);
  yText = yText + textHeight + 7;
  String strAngle = (String)servoAngle;
  _display.getTextBounds(strAngle, 0, 0, &x1, &y1, &textWidth, &textHeight);
  xText = xLeft + halfScreenSize / 2 - textWidth / 2;
  _display.setCursor(xText, yText);
  _display.print(strAngle);
}
