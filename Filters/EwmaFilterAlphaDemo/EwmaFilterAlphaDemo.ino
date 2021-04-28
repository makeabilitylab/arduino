/*
 * Example of smoothing input on A0 using an exponential moving average filter
 *
 * By Jon E. Froehlich
 * @jonfroehlich
 * http://makeabilitylab.io
 * 
 */

// The Arduino Uno ADC is 10 bits (thus, 0 - 1023 values)
#define MAX_ANALOG_INPUT_VAL 1023

const int SENSOR_INPUT_PIN = A0;

const float ALPHA0_01 = 0.01f;
const float ALPHA0_1 = 0.1f;
const float ALPHA0_5 = 0.5f;

double _ewma0_01 = 0;  // the EWMA for 0.01 alpha
double _ewma0_1 = 0;   // the EWMA for 0.1 alpha
double _ewma0_5 = 0;   // the EWMA for 0.5 alpha

void setup()
{
  Serial.begin(9600); // for printing values to console
  int sensorVal = analogRead(SENSOR_INPUT_PIN); 
  _ewma0_01 = sensorVal;
  _ewma0_1 = sensorVal;
  _ewma0_5 = sensorVal;
}

void loop()
{
  int sensorVal = analogRead(A0); // returns 0 - 1023 (due to 10 bit ADC)
  
  // Apply the EWMA formula 
  _ewma0_01 = (ALPHA0_01 * sensorVal) + (1 - ALPHA0_01) * _ewma0_01;
  _ewma0_1 = (ALPHA0_1 * sensorVal) + (1 - ALPHA0_1) * _ewma0_1;
  _ewma0_5 = (ALPHA0_5 * sensorVal) + (1 - ALPHA0_5) * _ewma0_5;

  // Print out from least smoothed to most smoothed
  Serial.print("Raw:");
  Serial.print(sensorVal);
  Serial.print(", EWMA0.5:");  
  Serial.print(_ewma0_5);      
  Serial.print(", EWMA0.1:");  
  Serial.print(_ewma0_1);
  Serial.print(", EWMA0.01:");  
  Serial.println(_ewma0_01);  
  
  delay(50); // Reading new values at ~20Hz
}
