/*
 * Allows user to set the blinking frequency (on analog input A0) without using delay
 * 
 * By Jon E. Froehlich
 * @jonfroehlich
 * http://makeabilitylab.io
 *
 */

const int LED_OUTPUT_PIN = 3;
const int FREQ_ANALOG_IN_PIN = A0;
const int MAX_FREQ = 20; // max freq at which to blink (in Hz)
const int MIN_FREQ = 1; // min freq at which to blink (in Hz)
const int MAX_ANALOG_VAL = 1024;

// Generally, you should use "unsigned long" for variables that hold time
// The value will quickly become too large for an int to store
unsigned long _lastToggledTimestampMs = 0; // tracks the last time the LED was updated
int _ledState = LOW; // will toggle between LOW and HIGH
int _blinkFreq = 0; 

// The setup function runs once when you press reset or power the board
void setup() {
  // Because pins 0 - 13 can either be input or output, we must specify
  // how we're using the pin by using pinMode. In this case, we want to
  // control an LED, so set the pin to OUTPUT
  pinMode(LED_OUTPUT_PIN, OUTPUT);

  Serial.begin(9600);
}

// The loop function runs over and over again forever
void loop() {

  int analogFreqIn = analogRead(FREQ_ANALOG_IN_PIN);
  _blinkFreq = map(analogFreqIn, 0, MAX_ANALOG_VAL, MIN_FREQ, MAX_FREQ + 1);

  float wavePeriodSecs = 1 / (float)_blinkFreq;
  unsigned long halfPeriodMs = (wavePeriodSecs / 2) * 1000; // divide period by half and scale to milliseconds
  
  // Get the current time since the Arduino started our program (in ms)
  unsigned long currentTimestampMs = millis();
  
  // Check to see if the time between now and the last time we toggled
  // the LED has exceeded our blink interval. If so, toggled the LED
  if (currentTimestampMs - _lastToggledTimestampMs >= halfPeriodMs) {
    _ledState = _ledState == HIGH ? LOW : HIGH; // toggle the LED
    _lastToggledTimestampMs = currentTimestampMs; // store current time as toggle time
    digitalWrite(LED_OUTPUT_PIN, _ledState); // output the LED state
  }  

  Serial.print("Analog in: ");
  Serial.print(analogFreqIn);
  Serial.print(" Freq: ");
  Serial.print(_blinkFreq);
  Serial.print(" Wave period (secs): ");
  Serial.print(wavePeriodSecs);
  Serial.print(" Half period (ms): ");
  Serial.println(halfPeriodMs);
}
