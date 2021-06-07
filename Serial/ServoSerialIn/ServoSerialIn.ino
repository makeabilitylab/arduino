
/**
 * Control a servo motor angle with serial input. The serial stream can either
 * be an integer between 0 - 180 (inclusive) or a floating point between [0, 1] (inclusive).
 * The latter will be converted to an angle between 0 - 180
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

const int SERVO_OUTPUT_PIN = 9;
const int MAX_ANALOG_VAL = 1023;

const int MIN_SERVO_ANGLE = 0;
const int MAX_SERVO_ANGLE = 180;

Servo _servo; 
int _serialServoAngle = -1;
 
void setup() 
{ 
  Serial.begin(115200);
  
  _servo.attach(SERVO_OUTPUT_PIN);  
} 
 
void loop() 
{ 
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
    }else{
      _serialServoAngle = rcvdSerialData.toInt();
    }

    _serialServoAngle = constrain(_serialServoAngle, MIN_SERVO_ANGLE, MAX_SERVO_ANGLE);

    // Echo back data
    Serial.print("# Arduino Received: '");
    Serial.print(rcvdSerialData);
    Serial.print("' Converted to: ");
    Serial.println(_serialServoAngle);

    // Write out new servo angle
    _servo.write(_serialServoAngle);
  }
} 
