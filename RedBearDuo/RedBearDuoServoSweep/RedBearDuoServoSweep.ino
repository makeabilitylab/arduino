/*
 * This example shows how to use a servo motor with the RedBear Duo by slowly oscillating the motor
 * back and forth
 * 
 * For CSE590, we are using the Tower PRo SG92R micro servo that can rotate
 * approximately 180 degrees (90 in each direction). See: https://www.adafruit.com/product/169
 * 
 * By Jon Froehlich for CSE590
 * http://makeabilitylab.io
 * 
 * Other resources: 
 *  - https://www.arduino.cc/en/Tutorial/Sweep
 *  - https://learn.adafruit.com/adafruit-arduino-lesson-14-servo-motors
 */

/* 
 * IMPORTANT: When working with the RedBear Duo, you must have this line of
 * code at the top of your program. The default state is SYSTEM_MODE(AUTOMATIC);
 * however, this puts the RedBear Duo in a special cloud-based mode that we 
 * are not using. For our purposes, set SYSTEM_MODE(SEMI_AUTOMATIC) or
 * SYSTEM_MODE(MANUAL). See https://docs.particle.io/reference/firmware/photon/#system-modes
 */
SYSTEM_MODE(MANUAL); 
 
const int SERVO_OUTPUT_PIN = D2;
const int DELAY_MS = 50;

int _stepAmount = 1; // the amount to change the angle of servo on each pass
int _minAngle = 0;
int _maxAngle = 180;
int _curAngle = 0;
Servo _servo;  
 
void setup() 
{ 
  _servo.attach(SERVO_OUTPUT_PIN);
  Serial.begin(9600); 
} 
 
 
void loop() 
{ 
  // set the current angle
  _servo.write(_curAngle);
  Serial.println(_curAngle);

  // update the angle for next time the loop
  _curAngle += _stepAmount;

  // reverse the direction of the angle (as necessary)
  if (_curAngle <= _minAngle || _curAngle >= _maxAngle) {
    _stepAmount = -_stepAmount;
  }

  // wait for 30 milliseconds to see the dimming effect
  delay(DELAY_MS);
}
