/**
 * This example reads in an analog value on A6 (GPIO 14) and uses it to set a PWM duty cycle output
 * on GPIO 21
 * 
 * Note: The ESP32 has two ADC chips. The official Espressif docs state that ADC2 (attached to GPIOs 
 * 0, 2, 4, 12 - 15 and 25 - 27) is only usable when the WiFi has *not started*. The Adafruit Huzzah32 
 * docs state that ADC1 can only be used when the WiFi *has* been started. We believe the Huzzah 32 docs 
 * are wrong.
 * 
 * Official Espressif ESP32 ADC docs:
 * https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/peripherals/adc.html
 * 
 * Official Adafruit Huzzah32 docs:
 * https://learn.adafruit.com/adafruit-huzzah32-esp32-feather/pinouts
 * 
 * By Jon E. Froehlich
 * @jonfroehlich
 * http://makeabilitylab.io
 * 
 * Circuit diagram and walkthrough here:
 * https://makeabilitylab.github.io/physcomp/esp32/pot-fade
 * 
 */

const int LED_OUTPUT_PIN = 21;
const int POTENTIOMETER_INPUT_PIN = A6; // GPIO 14
const int MAX_ANALOG_VAL = 4095;

const int PWM_CHANNEL = 0; // ESP32 has 16 channels which can generate 16 independent waveforms
const int PWM_FREQ = 500;  // Recall that Arduino Uno is ~490 Hz. Official ESP32 example uses 5,000Hz
const int PWM_RESOLUTION = 8; // We'll use same resolution as Uno (8 bits, 0-255) but ESP32 can go up to 16 bits 

// The max duty cycle value based on PWM resolution (will be 255 if resolution is 8 bits)
const int MAX_DUTY_CYCLE = (int)(pow(2, PWM_RESOLUTION) - 1);

void setup() {
  Serial.begin(9600);

  pinMode(LED_OUTPUT_PIN, OUTPUT);
  pinMode(POTENTIOMETER_INPUT_PIN, INPUT);

  // Setup the PWM waveform and attach our LED output pin
  ledcSetup(PWM_CHANNEL, PWM_FREQ, PWM_RESOLUTION);
  ledcAttachPin(LED_OUTPUT_PIN, PWM_CHANNEL);
}

void loop() {
  int potVal = analogRead(POTENTIOMETER_INPUT_PIN);
  int dutyCycle = map(potVal, 0, MAX_ANALOG_VAL, 0, MAX_DUTY_CYCLE);
  ledcWrite(PWM_CHANNEL, dutyCycle);

  Serial.println((String)potVal + ", " + dutyCycle);
  
  delay(10);
}
