/*
 * This example shows how to use the HC-SR04 Ultrasonic Sensor, which we purchased
 * from Sparkfun: https://www.sparkfun.com/products/13959 for your kits. The
 * sensor readings are smoothed using a sliding window average:
 * https://www.arduino.cc/en/Tutorial/Smoothing
 * 
 *  Hardware Connections:
 *  Arduino | HC-SR04 
 *  -------------------
 *    5V    |   VCC     
 *    7     |   Trig     
 *    8     |   Echo     
 *    GND   |   GND
 * 
 * Originally by Sparkfun Aug 3, 2016
 * https://github.com/sparkfun/HC-SR04_UltrasonicSensor
 * 
 * Modified by Jon Froehlich May 8, 2018 and Feb 12, 2019
 * @jonfroehlich
 * http://makeabilitylab.io
 * 
 * See also:
 *  - http://www.instructables.com/id/Simple-Arduino-and-HC-SR04-Example/
 *  - https://www.arduino.cc/en/Tutorial/Ping
 *  - https://howtomechatronics.com/tutorials/arduino/ultrasonic-sensor-hc-sr04/
 */

// Pins
const int TRIG_PIN = 7;
const int ECHO_PIN = 8;

// Anything over 400 cm (23200 us pulse) is "out of range"
const unsigned int MAX_DIST = 23200;

const int SMOOTHING_BUFFER_SIZE = 10;
int _smoothingBuffer[SMOOTHING_BUFFER_SIZE];
int _smoothingIndex = 0;
int _sensorTotal = 0;
float _sensorAvg = 0;

void setup() {

  // The Trigger pin will tell the sensor to range find
  pinMode(ECHO_PIN, INPUT);
  pinMode(TRIG_PIN, OUTPUT);
  digitalWrite(TRIG_PIN, LOW);

  // We'll use the serial monitor to view the sensor output
  Serial.begin(9600);

  // initialize all the readings to 0:
  for (int i = 0; i < SMOOTHING_BUFFER_SIZE; i++) {
    _smoothingBuffer[i] = 0;
  }
}

void loop() {

  unsigned long t1;
  unsigned long t2;
  unsigned long pulse_width;

  // Hold the trigger pin high for at least 10 us
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  // Wait for pulse on echo pin
  while ( digitalRead(ECHO_PIN) == 0 );

  // Measure how long the echo pin was held high (pulse width)
  // Note: the micros() counter will overflow after ~70 min
  // TODO: We probably need to check for a timeout here just in case
  // the ECHO_PIN never goes HIGH... so like
  // while ( digitalRead(ECHO_PIN) == 1 && micros() - t1 < threshold);
  t1 = micros();
  while ( digitalRead(ECHO_PIN) == 1);
  t2 = micros();
  pulse_width = t2 - t1;

  // Calculate distance in centimeters and inches. The constants
  // are found in the datasheet, and calculated from the assumed speed 
  // of sound in air at sea level (~340 m/s).
  // Datasheet: https://cdn.sparkfun.com/datasheets/Sensors/Proximity/HCSR04.pdf
  float distanceCm = pulse_width / 58.0;
  float distanceInches = pulse_width / 148.0;

  // Print out results
//  if ( pulse_width > MAX_DIST ) {
//    Serial.println("Out of range");
//  } else {
//    Serial.print(distanceCm);
//    Serial.print(" cm \t");
//    Serial.print(distanceInches);
//    Serial.println(" in");
//  }

  // subtract the last reading:
  _sensorTotal = _sensorTotal - _smoothingBuffer[_smoothingIndex];
  
  // add in new sensor value
  _smoothingBuffer[_smoothingIndex] = distanceCm;
  
  // add the reading to the total:
  _sensorTotal = _sensorTotal + _smoothingBuffer[_smoothingIndex];
  
  // advance to the next position in the array:
  _smoothingIndex = _smoothingIndex + 1;

  // if we're at the end of the array...
  if (_smoothingIndex >= SMOOTHING_BUFFER_SIZE) {
    // ...wrap around to the beginning:
    _smoothingIndex = 0;
  }

  // calculate the average:
  _sensorAvg = _sensorTotal / SMOOTHING_BUFFER_SIZE;

  if ( pulse_width < MAX_DIST ){
//    Serial.print(distanceCm);
//    Serial.print(" cm,");
//    Serial.print(_sensorAvg);
//    Serial.println(" cm,");

    float distanceFraction = constrain(_sensorAvg/200.0f, 0, 1);
    Serial.println(distanceFraction);
  }
  
  // The HC-SR04 datasheet recommends waiting at least 60ms before next measurement
  // in order to prevent accidentally noise between trigger and echo
  // See: https://cdn.sparkfun.com/datasheets/Sensors/Proximity/HCSR04.pdf
  delay(60);
}
