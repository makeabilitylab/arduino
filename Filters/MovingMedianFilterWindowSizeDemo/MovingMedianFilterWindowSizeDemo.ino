/*
 * Example of smoothing input on A0 using a moving median filter of various window sizes.  
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
#include <MedianFilterLib2.h>

// The Arduino Uno ADC is 10 bits (thus, 0 - 1023 values)
#define MAX_ANALOG_INPUT_VAL 1023

const int SENSOR_INPUT_PIN = A0;

MedianFilter2<int> _movingMedianFilter5(5);
MedianFilter2<int> _movingMedianFilter10(10);
MedianFilter2<int> _movingMedianFilter20(20);

void setup() {
  Serial.begin(9600);
}

void loop() {

  // Read the sensor value
  int sensorVal = analogRead(SENSOR_INPUT_PIN);

  // Get value
  int median5 = _movingMedianFilter5.AddValue(sensorVal);
  int median10 = _movingMedianFilter10.AddValue(sensorVal);
  int median20 = _movingMedianFilter20.AddValue(sensorVal);

  // print the sensor value and the smoothed values
  // best to visualize these in the serial plotter tool
  Serial.print("AnalogIn:");
  Serial.print(sensorVal);
  Serial.print(", MovingMedian5:");
  Serial.print(median5);
  Serial.print(", MovingMedian10:");
  Serial.print(median10);
  Serial.print(", MovingMedian20:");
  Serial.println(median20);

  delay(50);
}
