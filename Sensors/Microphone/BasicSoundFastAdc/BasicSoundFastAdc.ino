
/* 
 * This code extends BasicSound.ino by experimenting with FASTADC to speed up 
 * analogRead calls; however, FASTADC is disabled by default and the code works 
 * fine without it.
 * 
 * The other difference with BasicSound.ino is that we implemented a VUMeter fade 
 * so the maximum sensed microphone value is output to the mic and then faded downwards
 * until a new max is sensed.
 * 
 * Re: FASTADC
 * Some of my experiments on the Arduino Leonardo and using a pocket oscillocope measuring
 * the I/O frequency (square wave) on Pin 2:
 *  With FASTADC 1 and DEBUG 0, 6.5 kHz
 *  With FASTADC 0 and DEBUG 0, 2.93 kHz
 *  With FASTADC 1 and DEBUG 1, 1.53 kHz
 *  With FASTADC 0 and DEBUG 1, 1.19 kHz
 *  With FASTADC 0 and DEBUG 1 and INCLUDE_SAMPLING_RATE 1, 610 Hz
 *  
 * So, clearly printing to serial slows down sampling significantly
 *
 * Based in part on:
 *  - https://learn.adafruit.com/adafruit-microphone-amplifier-breakout/measuring-sound-levels
 *  - FASTADC code from: https://forum.arduino.cc/index.php?topic=6549.0
 */

// Set FASTADC to 1 if you want to speed up analogRead
// Not necessary for a simple volume meter, however.
#define FASTADC 0 

// defines for setting and clearing register bits
// This is relevant only if FASTADC is enabled
#ifndef cbi
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#endif
#ifndef sbi
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
#endif

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

const int DEBUG_LED_OUTPUT_PIN = 2;
boolean _debugLedVal = true;

#define DEBUG 0
#define INCLUDE_SAMPLING_RATE 0 //include sampling rate in debug Serial output

void setup() {
  // the fastest supported baud rate is 115200
  // https://www.arduino.cc/en/Reference/SoftwareSerialBegin
#if DEBUG
  Serial.begin(115200);
#endif  

  #if FASTADC
   // set prescale to 16
   sbi(ADCSRA,ADPS2) ;
   cbi(ADCSRA,ADPS1) ;
   cbi(ADCSRA,ADPS0) ;
  #endif
  
  //_lastTimeStamp = millis();
  pinMode(LED_OUTPUT_PIN, OUTPUT);
  pinMode(DEBUG_LED_OUTPUT_PIN, OUTPUT);

  _lastTimeStamp = millis();
}

void loop() {  

  digitalWrite(DEBUG_LED_OUTPUT_PIN, _debugLedVal);
  _debugLedVal = !_debugLedVal;
  
  
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
