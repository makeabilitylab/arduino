/*
  Smoothing for RedBear Duo

  Reads repeatedly from an analog input, calculating a running average and
  printing it to the computer. Keeps WINDOW_SIZE number of readings in an array 
  and continually averages them.

  The circuit:
  - analog sensor (potentiometer will do) attached to analog input 0

  created 22 Apr 2007
  by David A. Mellis  <dam@mellis.org>
  modified 9 Apr 2012
  by Tom Igoe
  modified 30 Apr 2018
  by Jon Froehlich

  This example code is in the public domain.

  http://www.arduino.cc/en/Tutorial/Smoothing
*/

/* 
 * IMPORTANT: When working with the RedBear Duo, you must have this line of
 * code at the top of your program. The default state is SYSTEM_MODE(AUTOMATIC);
 * however, this puts the RedBear Duo in a special cloud-based mode that we 
 * are not using. For our purposes, set SYSTEM_MODE(SEMI_AUTOMATIC) or
 * SYSTEM_MODE(MANUAL). See https://docs.particle.io/reference/firmware/photon/#system-modes
 */
SYSTEM_MODE(MANUAL); 

// Define the smoothing window size
const int SMOOTHING_WINDOW_SIZE = 10;
const int ANALOG_INPUT_PIN = A0;

int _readings[SMOOTHING_WINDOW_SIZE];      // the readings from the analog input
int _readIndex = 0;              // the index of the current reading
int _total = 0;                  // the running total
int _average = 0;                // the average


void setup() {

  pinMode(ANALOG_INPUT_PIN, INPUT);
  
  // initialize serial communication with computer
  Serial.begin(9600);
  
  // initialize all the readings to 0
  for (int i = 0; i < SMOOTHING_WINDOW_SIZE; i++) {
    _readings[i] = 0;
  }
}

void loop() {
  // subtract the last reading
  _total = _total - _readings[_readIndex];
  
  // read from the sensor
  int curReading = analogRead(ANALOG_INPUT_PIN);
  _readings[_readIndex] = curReading;
  
  // add the reading to the total
  _total = _total + _readings[_readIndex];
  
  // advance to the next position in the array
  _readIndex = _readIndex + 1;

  // if we're at the end of the array...
  if (_readIndex >= SMOOTHING_WINDOW_SIZE) {
    // ...wrap around to the beginning:
    _readIndex = 0;
  }

  // calculate the average
  _average = _total / SMOOTHING_WINDOW_SIZE;
  
  // send it to the computer as ASCII digits
  Serial.print(curReading);
  Serial.print(",");
  Serial.println(_average);
  
  delay(50); // delay in between reads for stability
}
