/*
 * Example of smoothing input on A0 using a moving average filter.  
 * Uses the MovingAverageFilter.hpp class from the MakeabilityLab_Arduino_Library
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
const int POT_INPUT_PIN = A0;

MovingAverageFilter _movingAverageFilter(10);

void setup() {
  pinMode(LED_OUTPUT_PIN, OUTPUT);
  Serial.begin(9600);
}

void loop() {

  // Read the potentiometer value
  int potVal = analogRead(POT_INPUT_PIN);

  // Get the smoothed version using a moving average filter
  _movingAverageFilter.add(potVal);
  int smoothedVal = _movingAverageFilter.getAverage();
 
  int ledVal = map(smoothedVal, 0, MAX_ANALOG_INPUT_VAL, 0, 255);

  // print the raw pot value and the converted led value
  Serial.print("AnalogIn:");
  Serial.print(potVal);
  Serial.print(", ");
  Serial.print("Smoothed:");
  Serial.println(smoothedVal);

  // write out the LED value. 
  analogWrite(LED_OUTPUT_PIN, ledVal);

  delay(50);
}
