/*
 * Example of smoothing input on A0 using a moving average filter.  
 * 
 * Requires the Median Filter 2 library from Luis Llamas:
 * https://github.com/warhog/Arduino-MedianFilter
 * 
 * You can install this in the Arduino IDE by going to:
 * 1. Tools -> Manage Libraries
 * 2. Search for median filter
 * 3. Select MedianFilterLib2 by Luis Llamas
 * 4. Click the 'Install' button
 *
 * By Jon E. Froehlich
 * @jonfroehlich
 * http://makeabilitylab.io
 * 
 */
#include <MovingAverageFilter.hpp>
#include <MedianFilterLib2.h>

// The Arduino Uno ADC is 10 bits (thus, 0 - 1023 values)
#define MAX_ANALOG_INPUT_VAL 1023

const int SENSOR_INPUT_PIN = A0;

MovingAverageFilter _movingAverageFilter5(5);

MedianFilter2<int> _movingMedianFilter5(5);
//
//MovingAverageFilter _movingAverageFilter10(10);
//MovingAverageFilter _movingAverageFilter20(20);

void setup() {
  Serial.begin(9600);
}

void loop() {

  // Read the potentiometer value
  int sensorVal = analogRead(SENSOR_INPUT_PIN);

  // Get the smoothed version using a moving average filter
  _movingAverageFilter5.add(sensorVal);

  int median5 = _movingMedianFilter5.AddValue(sensorVal);

  // print the sensor value and the smoothed values
  // best to visualize these in the serial plotter tool
  Serial.print("AnalogIn:");
  Serial.print(sensorVal);
  Serial.print(", ");
  Serial.print("MovingAverage5:");
  Serial.print(_movingAverageFilter5.getAverage());
  Serial.print(", ");
  Serial.print("MovingMedian5:");
  Serial.println(median5);

  delay(50);
}
