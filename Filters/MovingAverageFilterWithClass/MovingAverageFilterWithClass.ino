/*
 * Example of smoothing input on A0 using a moving average filter.  
 * 
 * This code requires the MovingAverageFilter.hpp class from the MakeabilityLab_Arduino_Library
 * 
 * To install and use this library:
 * 1. Get it from here: https://github.com/makeabilitylab/arduino/tree/master/MakeabilityLab_Arduino_Library
 * 2. Move the entire folder to the Arduino libraries folder on your computer.
 *   - On my Windows box, this is C:\Users\jonfr\Documents\Arduino\libraries
 *   - On Mac, it's: /Users/jonf/Documents/Arduino/libraries
 * 3. Then include the relevant libraries via #include <libraryname.h> or <libraryname.hpp>
 *
 * By Jon E. Froehlich
 * @jonfroehlich
 * http://makeabilitylab.io
 * 
 */

#include <MovingAverageFilter.hpp>

// The Arduino Uno ADC is 10 bits (thus, 0 - 1023 values)
#define MAX_ANALOG_INPUT_VAL 1023

const int LED_OUTPUT_PIN = LED_BUILTIN;
const int SENSOR_INPUT_PIN = A0;

MovingAverageFilter _movingAverageFilter(10);

void setup() {
  pinMode(LED_OUTPUT_PIN, OUTPUT);
  Serial.begin(9600);
}

void loop() {

  // Read the sensor value
  int sensorVal = analogRead(SENSOR_INPUT_PIN);

  // Get the smoothed version using a moving average filter
  _movingAverageFilter.add(sensorVal);
  int smoothedVal = _movingAverageFilter.getAverage();

  // write out the LED value. 
  int ledVal = map(smoothedVal, 0, MAX_ANALOG_INPUT_VAL, 0, 255);
  analogWrite(LED_OUTPUT_PIN, ledVal);

  // print the raw pot value and the converted led value
  Serial.print("AnalogIn:");
  Serial.print(sensorVal);
  Serial.print(", ");
  Serial.print("MovingAverageFilter-WindowSize10:");
  Serial.println(smoothedVal);

  

  delay(50);
}
