
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
#if defined(ESP32)
  // Install ESP32Servo library by Kevin Harrington via the 
  // Arduino Library Manager (search "ESP32Servo")
  #include <ESP32Servo.h>  
#else
  // Install Servo library by Michael Margolis via
  // the Arduino Library Manager (search "Servo")
  #include <Servo.h>
#endif

const int POTENTIOMETER_INPUT_PIN = A0;  
#if defined(ESP32)
  const int MAX_ANALOG_VAL = 4095; // The ESP32 has a 12-bit ADC
  const int SERVO_OUTPUT_PIN = 13; // Safe pin on ESP32 Huzzah32
#else
  const int MAX_ANALOG_VAL = 1023;
  const int SERVO_OUTPUT_PIN = 9;
#endif
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
