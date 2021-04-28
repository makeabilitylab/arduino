/*
 * Example of smoothing input on A0 using a moving average filter.  
 *
 * This example is based on:
 * https://www.arduino.cc/en/Tutorial/BuiltInExamples/Smoothing
 *
 * By Jon E. Froehlich
 * @jonfroehlich
 * http://makeabilitylab.io
 * 
 * For a walkthrough and circuit diagram, see:
 * https://makeabilitylab.github.io/physcomp/arduino/potentiometers.html
 * 
 * 
 */

// The Arduino Uno ADC is 10 bits (thus, 0 - 1023 values)
#define MAX_ANALOG_INPUT_VAL 1023

const int LED_OUTPUT_PIN = LED_BUILTIN;
const int SENSOR_INPUT_PIN = A0;

// Define the number of samples to keep track of. The higher the number, the
// more the readings will be smoothed, but the slower the output will respond to
// the input. Using a constant rather than a normal variable lets us use this
// value to determine the size of the readings array.
const int SMOOTHING_WINDOW_SIZE = 10; // 10 samples

int _samples[SMOOTHING_WINDOW_SIZE];  // the readings from the analog input
int _curReadIndex = 0;                // the index of the current reading
int _sampleTotal = 0;                 // the running total
int _sampleAvg = 0;                   // the average

void setup() {
  pinMode(LED_OUTPUT_PIN, OUTPUT);
  Serial.begin(9600);

  // initialize all the readings to 0:
  for (int i = 0; i < SMOOTHING_WINDOW_SIZE; i++) {
    _samples[i] = 0;
  }
}

void loop() {

  // subtract the last reading:
  _sampleTotal = _sampleTotal - _samples[_curReadIndex];
  
  // read the sensor value
  int sensorVal = analogRead(SENSOR_INPUT_PIN);
  _samples[_curReadIndex] = sensorVal;
  
  // add the reading to the total:
  _sampleTotal = _sampleTotal + _samples[_curReadIndex];
  
  // advance to the next position in the array:
  _curReadIndex = _curReadIndex + 1;

  // if we're at the end of the array...
  if (_curReadIndex >= SMOOTHING_WINDOW_SIZE) {
    // ...wrap around to the beginning:
    _curReadIndex = 0;
  }

  // calculate the average:
  _sampleAvg = _sampleTotal / SMOOTHING_WINDOW_SIZE;
  
  // set the LED brightness to the smoothed value
  int ledVal = map(_sampleAvg, 0, MAX_ANALOG_INPUT_VAL, 0, 255);
  analogWrite(LED_OUTPUT_PIN, ledVal);

  // print the raw pot value and the converted led value
  Serial.print("AnalogIn:");
  Serial.print(sensorVal);
  Serial.print(", ");
  Serial.print("Smoothed:");
  Serial.println(_sampleAvg);


  delay(50); // read samples at ~20Hz (once every 50ms)
}
