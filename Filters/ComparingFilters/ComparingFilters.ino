/*
 * Compares common signal filtering algorithms 
 * 
 * This code requires the Median Filter 2 library from Luis Llamas:
 * https://github.com/warhog/Arduino-MedianFilter and the MovingAverageFilter.hpp 
 * class from the MakeabilityLab_Arduino_Library 
 * 
 * For the MEdian Filter 2 library:
 *
 * 1. Tools -> Manage Libraries
 * 2. Search for median filter
 * 3. Select MedianFilterLib2 by Luis Llamas
 * 4. Click the 'Install' button
 * 
 * For the MovingAverageFilter library:
 * 
 * 1. Get it from here: https://github.com/makeabilitylab/arduino/tree/master/MakeabilityLab_Arduino_Library
 * 2. Move the entire folder to the Arduino libraries folder on your computer.
 *   - On my Windows box, this is C:\Users\jonfr\Documents\Arduino\libraries
 *   - On Mac, it's: /Users/jonf/Documents/Arduino/libraries
 * 3. Then include the relevant libraries via #include <libraryname.h> or <libraryname.hpp>
 * 
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
MovingAverageFilter _movingAverageFilter10(10);

MedianFilter2<int> _movingMedianFilter5(5);
MedianFilter2<int> _movingMedianFilter10(10);

const float ALPHA0_5 = 0.5f;
double _ewma0_5 = 0;   // the EWMA for 0.5 alpha

void setup() {
  Serial.begin(9600);

  int sensorVal = analogRead(SENSOR_INPUT_PIN); 
  _ewma0_5 = sensorVal;
}

void loop() {

  // Read the potentiometer value
  int sensorVal = analogRead(SENSOR_INPUT_PIN);

  // Traditional moving average filter
  _movingAverageFilter5.add(sensorVal);
  _movingAverageFilter10.add(sensorVal);

  // Exponentially weighted moving average
  _ewma0_5 = (ALPHA0_5 * sensorVal) + (1 - ALPHA0_5) * _ewma0_5;

  // Median filter
  int median5 = _movingMedianFilter5.AddValue(sensorVal);
  int median10 = _movingMedianFilter10.AddValue(sensorVal);

  // print the sensor value and the smoothed values
  // best to visualize these in the serial plotter tool
  Serial.print("AnalogIn:");
  Serial.print(sensorVal);
  Serial.print(", MovingAverage5:");
  Serial.print(_movingAverageFilter5.getAverage());
  Serial.print(", MovingMedian5:");
  Serial.print(median5);
  Serial.print(", MovingAverage10:");
  Serial.print(_movingAverageFilter10.getAverage());
  Serial.print(", MovingMedian10:");
  Serial.print(median10);
  Serial.print(", MovingMedian10:");
  Serial.print(median10);
  Serial.print(", EWMA0.5:");  
  Serial.println(_ewma0_5); 

  delay(50);
}
