
// See: https://learn.adafruit.com/adafruit-arduino-lesson-14-servo-motors

// https://www.arduino.cc/reference/en/libraries/servo/
#include <Servo.h> 

const int POTENTIOMETER_INPUT_PIN = A0;  
const int SERVO_OUTPUT_PIN = 9;
const int MAX_ANALOG_VAL = 1023;
Servo _servo; 
 
void setup() 
{ 
  _servo.attach(SERVO_OUTPUT_PIN);  
} 
 
void loop() 
{ 
  int potVal = analogRead(POTENTIOMETER_INPUT_PIN); 
  int servoAngle = map(potVal, 0, MAX_ANALOG_VAL, 1, 179);
  _servo.write(servoAngle);  
} 
