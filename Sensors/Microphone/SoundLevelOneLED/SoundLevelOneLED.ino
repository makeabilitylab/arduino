/*
 * Simple VUMeter using an electret microphone (with built-in amp) on A0
 * and an LED for output on Pin 3
 * 
 * By Jon E. Froehlich
 * @jonfroehlich
 * http://makeabilitylab.io
 * 
 */
const int LED_OUTPUT_PIN = 3;

// 1024 with 5V, 676 with 3.3V on Arduino Uno or Leonardo
const int MAX_ANALOG_IN = 676; 
const int MAX_ANALOG_OUT = 255;
const int MIC_INPUT_PIN = A0;

int _maxSoundLevel = 0;

void setup() {
  pinMode(LED_OUTPUT_PIN, OUTPUT);
  pinMode(MIC_INPUT_PIN, INPUT);
}

void loop() {  

  // Read in current sound level from microphone
  int soundLevel = analogRead(MIC_INPUT_PIN);

  // Since microphone works peak-to-peak from 0V to the reference voltage (either 3.3V or 5V)
  // we only look at half of the amplitude from MAX_ANALOG_IN/2 to MAX_ANALOG_IN
  int ledVal = map(soundLevel, MAX_ANALOG_IN/2, MAX_ANALOG_IN, 0, MAX_ANALOG_OUT);
  ledVal = constrain(ledVal, 0, MAX_ANALOG_OUT);

  // Set the LED brightness to current waveform amplitude
  analogWrite(LED_OUTPUT_PIN, ledVal);
}
