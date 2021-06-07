
/**
 * Control a servo motor angle with either a potentiometer input or serial input, 
 * which is selectable by button input on Pin 4. If serial is selected, the
 * built-in LED turns on.
 * 
 * The serial stream can either be an integer between 0 - 180 (inclusive) or a floating point 
 * between [0, 1] (inclusive). The latter will be converted to an angle between 0 - 180
 * Each value must be terminated by end of line.
 * 
 * See also our OLED version:
 * https://github.com/makeabilitylab/arduino/tree/master/Serial/ServoPotWithSerialInOLED
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

int _serialServoAngle = -1;
 
void setup() 
{ 
  Serial.begin(115200);
  pinMode(INPUT_MODE_SELECTION_PIN, INPUT_PULLUP);
  pinMode(MODE_INDICATOR_LED_PIN, OUTPUT);
  
  _servo.attach(SERVO_OUTPUT_PIN);  
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
    // Read data off the serial port until we get to the endline delimeter ('\n')
    // Store all of this data into a string
    String rcvdSerialData = Serial.readStringUntil('\n'); 

    // We accept either integers between 0 and 180 or floats. Floats must always
    // have a period.
    int indexOfDecimal = rcvdSerialData.indexOf('.');
    if(indexOfDecimal != -1){
      float serialServoAngleF = rcvdSerialData.toFloat();
      _serialServoAngle = MIN_SERVO_ANGLE + (int)(serialServoAngleF * (MAX_SERVO_ANGLE - MIN_SERVO_ANGLE));
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
  
  // Read pot value and conver to servo angle
  int potVal = analogRead(POTENTIOMETER_INPUT_PIN); 
  int potServoAngle = map(potVal, 0, MAX_ANALOG_VAL, MIN_SERVO_ANGLE, MAX_SERVO_ANGLE);

  // Set servo angle either based on pot value or serial value
  if(_servoInputMode == POTENTIOMETER_INPUT){
    _servo.write(potServoAngle);  
  }else{
    _servo.write(_serialServoAngle);  
  }
} 
