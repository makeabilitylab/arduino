/**
 * TODO
 * 
 * All GPIO pins can be used for PWM on the ESP32. On the Huzzah32, pins 34 (A2), 39 (A3), 36 (A4) 
 * are not output-capable and should not work by design.
 * 
 * By Jon E. Froehlich
 * @jonfroehlich
 * http://makeabilitylab.io
 * 
 * 
 * Official API ref for ledc
 * https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/peripherals/ledc.html
 * 
 * Header file for ledc.h
 * https://github.com/espressif/arduino-esp32/blob/a4305284d085caeddd1190d141710fb6f1c6cbe1/cores/esp32/esp32-hal-ledc.h#L29
 * 
 * Without Arduino library, a bit more complicated:
 * https://github.com/espressif/esp-idf/tree/741960d/examples/peripherals/ledc
 * 
 * Walkthrough and circuit diagram here:
 * https://makeabilitylab.github.io/physcomp/esp32/led-fade
 * 
 */


const int PWM_CHANNEL = 0; // ESP32 has 16 channels which can generate 16 independent waveforms
const int PWM_FREQ = 40 * 10000000;  // Recall that Arduino Uno is ~490 Hz. Official ESP32 example uses 5,000Hz

const int PWM_MIN_FREQ = 500; // 500 Hz
const int PWM_MAX_FREQ = 500 * 10000000; // 500 MHz
const int PWM_FREQ_STEP_MULTIPLER = 10;

// We'll use same resolution as Uno (8 bits, 0-255) but ESP32 can go up to 16 bits 
// Espressif docs seem to suggest that 10-15 bits is most common
// See: https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/peripherals/ledc.html#ledc-api-supported-range-frequency-duty-resolution
// Determined 1-16 bits for resolution here: 
// https://github.com/espressif/arduino-esp32/blob/a4305284d085caeddd1190d141710fb6f1c6cbe1/cores/esp32/esp32-hal-ledc.h
const int PWM_MIN_RESOLUTION = 2; 
const int PWM_MAX_RESOLUTION = 16; 

// The pin numbering on the Huzzah32 is a bit strange so always helps to consult the pin diagram
// See pin diagram here: https://makeabilitylab.github.io/physcomp/esp32/
const int LED_OUTPUT_PIN = LED_BUILTIN;

const int DELAY_MS = 50;    // delay between fade increments
int _curPwmFreq = PWM_MIN_FREQ;
int _curPwmResolution = PWM_MIN_RESOLUTION;

int getMaxDutyCycle(){
  return (int)(pow(2, _curPwmResolution) - 1);
}

void setup() {
  Serial.begin(9600);

  // Unlike traditional Arduino, we do not setup our PWM output pins
  // here with pinMode. Instead, we setup our PWM timer channel and then
  // we'll attach pins to the PWM channel. The cool side effect here, of course,
  // is that it's easy to control multiple pins at once with the same PWM timer
  ledcSetup(PWM_CHANNEL, _curPwmFreq, _curPwmResolution);

  // https://github.com/espressif/arduino-esp32/blob/a4305284d085caeddd1190d141710fb6f1c6cbe1/cores/esp32/esp32-hal-ledc.h
  ledcAttachPin(LED_OUTPUT_PIN, PWM_CHANNEL);
  printOutPwmConfiguration();
  
}

void printOutPwmConfiguration(){
  Serial.println((String)"PWM frequency set to: " + _curPwmFreq + " Hz");
  Serial.print("Duty cycle resolution set to: ");
  Serial.print(_curPwmResolution);
  Serial.print(" bits, so duty cycle range: 0 - ");
  Serial.println(getMaxDutyCycle());
  Serial.println();
  uint32_t ledcReadVal = ledcRead(PWM_CHANNEL);
  double ledcReadFreqVal = ledcReadFreq(PWM_CHANNEL);
  Serial.println((String)"ledcRead: " + ledcReadVal + " ledcReadFreq: " + ledcReadFreqVal);
}

void loop() {

  // fade LED brightness up
  //Serial.print("Fading up channel ");
  //Serial.println(PWM_CHANNEL);
  int maxDutyCycle = getMaxDutyCycle();
  ledcWrite(PWM_CHANNEL, maxDutyCycle);
  Serial.println((String)"Setting duty cycle to: " + maxDutyCycle);
//  int fadeStep = (int)ceil(0.1 * maxDutyCycle);
//  Serial.println((String)"Fade step: " + fadeStep);
//  for(int dutyCycle = 0; dutyCycle <= maxDutyCycle; dutyCycle += fadeStep){   
//    ledcWrite(PWM_CHANNEL, dutyCycle);
//    delay(DELAY_MS);
//  }
//
//  // fade LED brightness down
//  //Serial.print("Fading down channel ");
//  //Serial.println(PWM_CHANNEL);
//  for(int dutyCycle = maxDutyCycle; dutyCycle >= 0; dutyCycle -= fadeStep){
//    ledcWrite(PWM_CHANNEL, dutyCycle);   
//    delay(DELAY_MS);
//  }

  if(_curPwmResolution > PWM_MAX_RESOLUTION){
    _curPwmResolution = PWM_MIN_RESOLUTION;
    _curPwmFreq *= PWM_FREQ_STEP_MULTIPLER;

    if(_curPwmFreq > PWM_MAX_FREQ){
      _curPwmFreq = PWM_MIN_FREQ;
    }
  }
  _curPwmResolution++;
  ledcSetup(PWM_CHANNEL, _curPwmFreq, _curPwmResolution);
  printOutPwmConfiguration();

  delay(2000);
}
