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

const int SENSOR_INPUT_PIN = A0;

float _ewmaAlpha = 0.1;  // the EWMA alpha value
double _ewma = 0;        // the EWMA result, initialized to zero

void setup()
{
  Serial.begin(9600); // for printing values to console
  _ewma = analogRead(SENSOR_INPUT_PIN);  //set EWMA for index 1
}

void loop()
{
  int sensorVal = analogRead(A0); // returns 0 - 1023 (due to 10 bit ADC)
  
  // Apply the EWMA formula 
  _ewma = (_ewmaAlpha * sensorVal) + (1 - _ewmaAlpha) * _ewma;

  Serial.print(sensorVal);      
  Serial.print(",");  
  Serial.println(_ewma);  
  delay(50); // Reading new values at ~20Hz
}
