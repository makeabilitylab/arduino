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

const int POT_INPUT_PIN = A0;

MovingAverageFilter _movingAverageFilter5(5);
MovingAverageFilter _movingAverageFilter10(10);
MovingAverageFilter _movingAverageFilter20(20);

void setup() {
  Serial.begin(9600);
}

void loop() {

  // Read the potentiometer value
  int potVal = analogRead(POT_INPUT_PIN);

  // Get the smoothed version using a moving average filter
  _movingAverageFilter5.add(potVal);
  _movingAverageFilter10.add(potVal);
  _movingAverageFilter20.add(potVal);

  // print the raw pot value and the smoothed values
  // best to visualize these in the serial plotter tool
  Serial.print("AnalogIn:");
  Serial.print(potVal);
  Serial.print(", ");
  Serial.print("Smoothed5:");
  Serial.print(_movingAverageFilter5.getAverage());
  Serial.print(", ");
  Serial.print("Smoothed10:");
  Serial.print(_movingAverageFilter10.getAverage());
  Serial.print(", ");
  Serial.print("Smoothed20:");
  Serial.println(_movingAverageFilter20.getAverage());

  delay(50);
}
