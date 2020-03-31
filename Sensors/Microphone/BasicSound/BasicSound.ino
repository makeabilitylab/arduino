// Based in part on https://learn.adafruit.com/adafruit-microphone-amplifier-breakout/measuring-sound-levels
// TODO: add comments


const int LED_OUTPUT_PIN = 3;
const int MAX_ANALOG_IN = 676; //1024 with 5V, 676 with 3.3V on Arduino Uno
const int MAX_ANALOG_OUT = 255;

float _samplesPerSecond = 0;
unsigned long _numSamples = 0;
unsigned long _lastTimeStamp = 0;

int _maxSoundLevel = 0; // the maximum sound level read

// Because we are reading sound levels so quickly, it's useful
// to *fade* the LED down from sound peaks (much like a typical VUMeter)
// So, we capture max values with _fadeLedVal and then decrement
// each time _fadeTimeMs threshold is passed
int _fadeLedVal = 0;
unsigned long _fadeTimeMs = 1; // amount of time to stay on a particular LED output val
unsigned long _fadeTimestamp = 0; // last fade decrement timestamp

#define DEBUG 0
#define INCLUDE_SAMPLING_RATE 0 //include sampling rate in debug Serial output

void setup() {
  // the fastest supported baud rate is 115200
  // https://www.arduino.cc/en/Reference/SoftwareSerialBegin
#if DEBUG
  Serial.begin(115200);
#endif  
  
  //_lastTimeStamp = millis();
  pinMode(LED_OUTPUT_PIN, OUTPUT);

  _lastTimeStamp = millis();
}

void loop() {  
  
  int soundLevel = analogRead(A0);
  int ledVal = map(soundLevel, MAX_ANALOG_IN/2, MAX_ANALOG_IN, 0, MAX_ANALOG_OUT);
  ledVal = constrain(ledVal, 0, MAX_ANALOG_OUT);

  // set the new fadeLedVal if ledVal is bigger
  if(ledVal >= _fadeLedVal){
    _fadeLedVal = ledVal;
    _fadeTimestamp = millis();
  }

  // decrement the fade if we surprassed the _fadeTimeMs threshold
  if(millis() - _fadeTimestamp > _fadeTimeMs){
    _fadeLedVal--;
    _fadeTimestamp = millis();
  }

  // Write out the fade val
  analogWrite(LED_OUTPUT_PIN, _fadeLedVal);
  
  // The rest of this is just debugging
#if DEBUG
  _numSamples++;

  unsigned long timeDiff = millis() - _lastTimeStamp;
  if(timeDiff > 1000){
    _samplesPerSecond = _numSamples / (float)((timeDiff)/1000.0);
    _lastTimeStamp = millis();
    _numSamples = 0;
  }

  if(_maxSoundLevel < soundLevel){
    _maxSoundLevel = soundLevel;
  }

  #if INCLUDE_SAMPLING_RATE
  Serial.print(_samplesPerSecond);
  Serial.print(", ");
  #endif
  Serial.print(soundLevel);
  Serial.print(", ");
  Serial.print(ledVal);
  Serial.print(", ");
  Serial.println(_fadeLedVal);
#endif 
}
