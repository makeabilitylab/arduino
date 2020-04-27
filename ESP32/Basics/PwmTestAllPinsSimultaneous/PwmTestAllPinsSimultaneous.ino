/**
 * This example fades on and off all pins in sequence using the ESP32's PWM functionality
 * 
 * All GPIO pins can be used for PWM on the ESP32. On the Huzzah32, pins 34 (A2), 39 (A3), 36 (A4) 
 * are not output-capable and should not work by design. We still include them here for completeness
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
 */

const int DELAY_MS = 5;

const int PWM_CHANNEL = 0; // ESP32 has 16 channels which can generate 16 independent waveforms
const int PWM_FREQ = 500; // Recall that Arduino Uno is ~490 Hz. Official ESP32 example uses 5,000Hz

// We'll use same resolution as Uno (8 bits, 0-255) but ESP32 can go up to 15 bits (maybe more?)
// EspressIf docs seem to suggest that 10-15 bits is most common
// See: https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/peripherals/ledc.html#ledc-api-supported-range-frequency-duty-resolution
const int PWM_RESOLUTION = 8; 

// Pins start with 26 at A0 and switch to other side of board at 13 (A12)
// According to the Huzzah32 docs, 34 (A2), 39 (A3), 36 (A4) are not output-capable so should not work by design 
// See: https://learn.adafruit.com/adafruit-huzzah32-esp32-feather/pinouts
// But we've included them here anyway for completeness (and to double check!)
const int GPIO_PINS[] = {26, 25, 34, 39, 36, 4, 5, 18, 19, 16, 17, 21, 13, 12, 27, 33, 15, 32, 14, 22, 23};

int _numGpioPins = 0;
int _ledFadeStep = 5;

void setup() {
  Serial.begin(9600);

  // Unlike traditional Arduino, we do not setup our PWM output pins
  // here with pinMode. Instead, we setup our PWM timer channel and then
  // we'll attach pins to the PWM channel. The cool side effect here, of course,
  // is that it's easy to control multiple pins at once with the same PWM timer
  ledcSetup(PWM_CHANNEL, PWM_FREQ, PWM_RESOLUTION);
  _numGpioPins = sizeof(GPIO_PINS) / sizeof(GPIO_PINS[0]);

  for(int i = 0; i < _numGpioPins; i++){
    int pin = GPIO_PINS[i];
    
    // https://github.com/espressif/arduino-esp32/blob/a4305284d085caeddd1190d141710fb6f1c6cbe1/cores/esp32/esp32-hal-ledc.h
    ledcAttachPin(pin, PWM_CHANNEL);
  }

  Serial.print("Num of GPIO pins: ");
  Serial.println(_numGpioPins);
  Serial.print("Duty cycle resolution set to: ");
  Serial.print(PWM_RESOLUTION);
  Serial.print(" bits, so duty cycle range: 0 - ");
  Serial.println(pow(2, PWM_RESOLUTION) - 1);
}

void loop() {
  // All pins are hooked up to the same channel, so we can
  // fade them all together with a single call to ledcWrite
  fadeChannelOnAndOff(PWM_CHANNEL, PWM_RESOLUTION);
}

/**
 * Fades the specific channel on and off
 */
void fadeChannelOnAndOff(int pwmChannel, int pwmResolution){
  int maxDutyCycle = pow(2, pwmResolution) - 1;
  for(int dutyCycle = 0; dutyCycle <= maxDutyCycle; dutyCycle++){   
    ledcWrite(pwmChannel, dutyCycle);
    delay(DELAY_MS);
  }

  // decrease the LED brightness
  for(int dutyCycle = maxDutyCycle; dutyCycle >= 0; dutyCycle--){
    ledcWrite(pwmChannel, dutyCycle);   
    delay(DELAY_MS);
  }
}
