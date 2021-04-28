/*
 * Example of smoothing input on A0 using a moving median filter.  
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

const int LED_OUTPUT_PIN = LED_BUILTIN;
const int SENSOR_INPUT_PIN = A0;

MedianFilter2<int> _movingMedianFilter5(5);

void setup() {
  Serial.begin(9600);
}

void loop() {

  // Read the sensor value
  int sensorVal = analogRead(SENSOR_INPUT_PIN);

  // Get median filter value
  int median5 = _movingMedianFilter5.AddValue(sensorVal);

  // write out the LED value. 
  int ledVal = map(median5, 0, MAX_ANALOG_INPUT_VAL, 0, 255);
  analogWrite(LED_OUTPUT_PIN, ledVal);

  // print the sensor value and the smoothed values
  // best to visualize these in the serial plotter tool
  Serial.print("AnalogIn:");
  Serial.print(sensorVal);
  Serial.print(", MovingMedian5:");
  Serial.println(median5);

  delay(50);
}
