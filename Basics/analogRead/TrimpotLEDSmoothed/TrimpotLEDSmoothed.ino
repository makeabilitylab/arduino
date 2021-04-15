/*
 * This example reads in a potentiometer value (from A0) and sets the brightness 
 * of an LED (hooked up to Pin 3). As the potentiomer readings can be a bit noisy,
 * we take a simple moving average using a SMOOTHING_WINDOW_SIZE of 10 (as the default).
 * If you increase this window size, you will get a smoother signal but at a cost of lag.
 * 
 * The analogWrite() function uses PWM, so if you want to change the pin you're
 * using, be sure to use another PWM capable pin. On the Arduino Uno, the PWM pins
 * are identified with a "~" sign, like ~3, ~5, ~6, ~9, ~10 and ~11. For other Arduino
 * boards, please consult the pin layout data sheet.
 *
 * By Jon E. Froehlich
 * @jonfroehlich
 * http://makeabilitylab.io
 * 
 * For a walkthrough and circuit diagram, see:
 * https://makeabilitylab.github.io/physcomp/arduino/potentiometers.html
 * 
 * Smoothing based on:
 * https://www.arduino.cc/en/Tutorial/BuiltInExamples/Smoothing
 * 
 */

// The Arduino Uno ADC is 10 bits (thus, 0 - 1023 values)
#define MAX_ANALOG_INPUT_VAL 1023

const int LED_OUTPUT_PIN = 3;
const int POT_INPUT_PIN = A0;

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
  
  // read the potentiometer value
  int potVal = analogRead(POT_INPUT_PIN);
  _samples[_curReadIndex] = potVal;
  
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

  
  // the analogRead on the Arduino Uno goes from 0 to 1023. We need to remap
  // this value to the smaller range (0-255) since the analogWrite function can 
  // only write out 0-255 (a byte--2^8). The map function provides a linear
  // mapping to do this (however, a better way would likely be some sort of
  // non-linear mapping given that perceived LED brightness is not linear with current,
  // perhaps logarithmic)
  int ledVal = map(_sampleAvg, 0, MAX_ANALOG_INPUT_VAL, 0, 255);

  // print the raw pot value and the converted led value
  Serial.print("Analog in: ");
  Serial.print(potVal);
  Serial.print(" Smoothed val: ");
  Serial.print(_sampleAvg);
  Serial.print(" LED val: ");
  Serial.print(ledVal);
  Serial.print(" Duty cycle: ");
  Serial.print(ledVal/255.0 * 100, 1);
  Serial.println("%");

  // write out the LED value. This value is translated to voltage by: 
  // voltageVal = max_voltage * val/255 or voltageVal = 3.3V * val/255 in 
  // the case of the RedBear Duo
  analogWrite(LED_OUTPUT_PIN, ledVal);

  delay(50);
}
