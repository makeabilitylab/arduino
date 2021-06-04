/**
 * This example receives an integer value (as a text string) delimited by a return character '\n',
 * converts the text into an integer, and uses this to control a servo motor on pin 9.
 * 
 * It is intended to work with:
 * https://makeabilitylab.github.io/p5js/WebSerial/HandWaveDetector
 * 
 * To learn more about servo motors and Arduino in general, see:
 * https://learn.adafruit.com/adafruit-arduino-lesson-14-servo-motors/overview
 * 
 * While this example will work with any Serial program, I wrote it to demonstrate the
 * Serial API for web browsers (currently only works with Chrome). This new API allows 
 * developers to write JavaScript code in a web browser to read/write data over the serial
 * port, including to devices like Arduino.
 *
 * 
 * By Jon E. Froehlich
 * @jonfroehlich
 * http://makeabilitylab.io
 * 
 */

#include <Servo.h> 

const int LED_PIN = 13; // just use LED pin for helping provide feedback about servo value
const int SERVO_PIN = 9;
const int BAUD_RATE = 9600;
const int DELAY_MS = 5; // set to 0 for no delay between loop calls

const int MIN_INCOMING_ANGLE = 50;
const int MAX_INCOMING_ANGLE = 125;

Servo _servo; 

void setup() {
  Serial.begin(BAUD_RATE);
  _servo.attach(SERVO_PIN);
}

void loop() {
  
  // Check to see if there is any incoming serial data
  if(Serial.available() > 0){
    // If we're here, then serial data has been received
    // Read data off the serial port until we get to the endline delimiter ('\n')
    // Store all of this data into a string
    String rcvdSerialData = Serial.readStringUntil('\n'); 

    // Convert the data into an integer (later versions of HandWaveDetector
    // send over an int rather than float)
    int val = rcvdSerialData.toInt();
    val = constrain(val, MIN_INCOMING_ANGLE, MAX_INCOMING_ANGLE); 

    // Set the LED to a brightness corresponding to angle
    int ledVal = map(val, MIN_INCOMING_ANGLE, MAX_INCOMING_ANGLE, 0, 255);
    analogWrite(LED_PIN, ledVal);
    
    // Now map val to servo angle. From my own experiments,
    // I found that moving the puppet's arm between 40-80 degrees looks the best
    int servoAngle = map(val, MAX_INCOMING_ANGLE, MIN_INCOMING_ANGLE, 30, 80);

    // Set the angle on the servo motor
    _servo.write(servoAngle); 

    // Send some data (for debugging purposes) back on serial
    // This is not necessary but helpful. Then the webpage can
    // display this debug output (if necessary)
    Serial.print("Arduino Received: '");
    Serial.print(rcvdSerialData);
    Serial.print("' Converted to Servo Angle: ");
    Serial.println(servoAngle);
  }

  delay(DELAY_MS);
}
