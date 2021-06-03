
/**
 * Control a servo motor angle with a potentiometer input
 * 
 * Based on:
 * https://learn.adafruit.com/adafruit-arduino-lesson-14-servo-motors
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
  // Read pot value
  int potVal = analogRead(POTENTIOMETER_INPUT_PIN); 

  // Servo motor can move between 0 - 180 degrees
  int servoAngle = map(potVal, 0, MAX_ANALOG_VAL, 0, 180);

  // Set servo angle
  _servo.write(servoAngle);  
} 
