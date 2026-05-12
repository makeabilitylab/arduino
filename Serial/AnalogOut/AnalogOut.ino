/**
 * Reads in an analog value on A0 and sends a normalized value [0, 1] inclusive
 * over the serial port with 4 decimal point precision.
 * 
 * By Jon E. Froehlich
 * @jonfroehlich
 * http://makeabilitylab.io
 * 
 */

const int DELAY_MS = 100;

// Preprocessor defines for cross-platform portability
#if defined(ESP32)
  const int MAX_ANALOG_VAL = 4095;  // ESP32 has a 12-bit ADC (0-4095)
  const int ANALOG_INPUT_PIN = A5;  // Use A5 on ESP32
#else
  const int MAX_ANALOG_VAL = 1023;  // AVR Arduinos have a 10-bit ADC (0-1023)
  const int ANALOG_INPUT_PIN = A0;  // Use A0 on Arduino
#endif

int _lastAnalogVal = -1;

// If false, only sends new data when the new analog value does not
// equal the last analog value. If true, always sends the data
boolean _alwaysSendData = true; 

void setup() {
  Serial.begin(115200);
}

void loop() {

  // Get the new analog value
  int analogVal = analogRead(ANALOG_INPUT_PIN);

  // If the analog value has changed, send a new one over serial
  if(_alwaysSendData || _lastAnalogVal != analogVal){
    float sizeFrac = analogVal / (float)MAX_ANALOG_VAL;
    Serial.println(sizeFrac, 4); // 4 decimal point precision
  }

  _lastAnalogVal = analogVal;
  delay(DELAY_MS);
}
