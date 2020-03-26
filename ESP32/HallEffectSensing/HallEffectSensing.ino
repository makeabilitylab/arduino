/*
 * The ESP32 has a built-in Hall Effect sensor for sensing the presence of a magnetic field.
 * This is a relatively 'noisy' sensor without LNA (Low Noise Amplification) and it's not 
 * super clear why ExpressIf included it into the ESP32 (some interesting speculations online).
 * 
 * Test the code by putting a magnet on or close to the ESP32 chip
 * 
 * The underlying sensing code, accessible via a hallRead() call, is here:
 * https://github.com/espressif/arduino-esp32/blob/master/cores/esp32/esp32-hal-adc.c
 * 
 * Note: typically, we include a linear hall effect sensor in our course hardware kits, commonly
 * this TI DRV5055 Ratiometric Linear Hall Effect Sensor:
 * https://www.mouser.com/ProductDetail/Texas-Instruments/DRV5055A1QLPGM, which is quite sensitive
 * and not particulalry noisy. The DRV5055 output changes linearly with an applied magnetic field.
 * 
 * By Jon E. Froehlich
 * @jonfroehlich
 * http://makeabilitylab.io
 * 
 * Circuit: None
 * Uses Serial to print magnetic values
 * 
 * Adapted from the official ESP32 example:
 * https://github.com/espressif/arduino-esp32/tree/master/libraries/ESP32/examples/HallSensor
 * 
 * Smoothing from:
 * https://www.arduino.cc/en/tutorial/smoothing
 */

// Define the number of samples to keep track of. The higher the number, the
// more the readings will be smoothed, but the slower the output will respond to
// the input. Using a constant rather than a normal variable lets us use this
// value to determine the size of the readings array.
const int SMOOTHING_WINDOW_SIZE = 10;

int _buffer[SMOOTHING_WINDOW_SIZE];      // the readings from the hall effect sensor
int _bufferIndex = 0;          // the index of the current reading
int _total = 0;                // the running total

void setup() {
  Serial.begin(9600);
}

void loop() {
  int hallVal = hallRead();

  // subtract the last reading:
  _total = _total - _buffer[_bufferIndex];
  
  // read from the sensor:
  _buffer[_bufferIndex] = hallVal;
  
  // add the reading to the total:
  _total = _total + _buffer[_bufferIndex];
  
  // advance to the next position in the array:
  _bufferIndex++;

  // if we're at the end of the array...
  if (_bufferIndex >= SMOOTHING_WINDOW_SIZE) {
    // ...wrap around to the beginning:
    _bufferIndex = 0;
  }

  // calculate the average:
  float average = _total / (float)SMOOTHING_WINDOW_SIZE;
  
  // print raw and smoothed
  Serial.print(hallVal);
  Serial.print(',');
  Serial.println(average);

  delay(30);
}
