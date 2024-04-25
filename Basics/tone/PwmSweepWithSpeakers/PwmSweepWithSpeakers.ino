/**
 * Uses a PWM sweep to demonstrate that the analogOut has a fixed freq
 * but different duty cycle; so the speaker pitch is always the same!
 * This is why we have to use tone, which has a fixed 50% duty cycle
 * but allows you to control the freq.
 * 
 * By Jon E. Froehlich
 * @jonfroehlich
 * http://makeabilitylab.io
 * 
 */

// Change this depending on where you put your speaker
// It needs to be hooked up to a PWM pin. We assume Leonardo for this
// demonstration
const int SPEAKER_OUTPUT_PIN = 5;

// The Leonardo has 7 PWM pins most of which run at 490 Hz
// but two (3 and 11) run at 980 Hz
const int NUM_PWM_PINS = 7;
const int PWM_PINS_ARDUINO_LEONARDO [] =  {  3,   5,   6,   9,  10,  11,  13};
const int PWM_FREQS_ARDUINO_LEONARDO [] = {980, 490, 490, 490, 490, 980, 490};

// If you're running on the Uno, you should use these instead
const int PWM_PINS_ARDUINO_UNO [] =  {  3,   5,   6,   9,  10,  11};
const int PWM_FREQS_ARDUINO_UNO [] = {490, 980, 980, 490, 490, 490, 490};

const int MAX_ANALOG_OUT = 255;
int _indexOfSpeakerOutputPin = -1;

// For PWM sweep
int _analogOutVal = 0;
int _analogOutStep = 10;
const int DELAY_BETWEEN_STEPS = 500; // in milliseconds

void setup() {
  
  // For printing info to screen
  Serial.begin(9600);

  // Setup tone output pin
  pinMode(SPEAKER_OUTPUT_PIN, OUTPUT);

  for(int i = 0; i < NUM_PWM_PINS; i++){
    if(SPEAKER_OUTPUT_PIN == PWM_PINS_ARDUINO_LEONARDO[i]){
      _indexOfSpeakerOutputPin = i;
    }
  }
}

void loop() {
  
  analogWrite(SPEAKER_OUTPUT_PIN, _analogOutVal);

  // Loop through different analog vals
  _analogOutVal += _analogOutStep;
  _analogOutVal = constrain(_analogOutVal, 0, MAX_ANALOG_OUT);
  if(_analogOutStep < 0 && _analogOutVal <= 0){
    _analogOutVal = 0;
    _analogOutStep *= -1;
  }else if(_analogOutStep > 0 && _analogOutVal >= MAX_ANALOG_OUT){
    _analogOutVal = MAX_ANALOG_OUT;
    _analogOutStep *= -1;
  }

  float dutyCycle = _analogOutVal / (float)MAX_ANALOG_OUT * 100;
  String strFreqAndDutyCycle = (String)"Freq: " + PWM_FREQS_ARDUINO_LEONARDO[_indexOfSpeakerOutputPin] +
                               " Hz; Duty cycle: " + String(dutyCycle, 0) + "% duty cycle";
  Serial.println(strFreqAndDutyCycle);

  delay(DELAY_BETWEEN_STEPS);
}

