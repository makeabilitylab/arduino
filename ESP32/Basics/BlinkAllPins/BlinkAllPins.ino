/**
 * This example blinks all pins simultaneously using digitalWrite.
 *
 * See lesson: https://makeabilitylab.github.io/physcomp/esp32/led-blink.html
 * 
 * Adafruit Huzzah32
 * There are 21 GPIO pins on the ESP32; however, on the Huzzah32, pins 34 (A2), 39 (A3), 36 (A4) 
 * are not output-capable and should not work by design. We still include them here for completeness
 * 
 * Adafruit ESP32-S3
 * There are 20 digital GPIO pins exposed on the ESP32-S3 Feather. All are output-capable.
 * Pins start at GPIO18 (A0, top-left) going down the left side, then GPIO3 (SDA, bottom-right)
 * going up the right side. See: https://learn.adafruit.com/adafruit-esp32-s3-feather/pinouts
 * With all pins HIGH, I measure about 150 mA being drawn over USB.
 *
 * By Jon E. Froehlich
 * @jonfroehlich
 * http://makeabilitylab.io
 *
 */

const int DELAY_MS = 1000; // blink delay in MS

// Uncomment the board you are using
#define BOARD_ESP32S3_FEATHER
// #define BOARD_HUZZAH32

#ifdef BOARD_HUZZAH32
  // Pins start with 26 at A0 and switch to other side of board at 13 (A12)
  // According to the Huzzah32 docs, 34 (A2), 39 (A3), 36 (A4) are not output-capable so should not work by design 
  // See: https://learn.adafruit.com/adafruit-huzzah32-esp32-feather/pinouts
  // But we've included them here anyway for completeness (and to double check!)
  const int GPIO_PINS[] = {26, 25, 34, 39, 36, 4, 5, 18, 19, 16, 17, 21, 13, 12, 27, 33, 15, 32, 14, 22, 23};
#elif defined(BOARD_ESP32S3_FEATHER)
  const int GPIO_PINS[] = {18, 17, 16, 15, 14, 8, 36, 35, 37, 38, 39, 3, 4, 5, 6, 9, 10, 11, 12, 13};
#endif

int _numGpioPins = 0;

void setup() {
  Serial.begin(9600);

  _numGpioPins = sizeof(GPIO_PINS) / sizeof(GPIO_PINS[0]);
  for(int i = 0; i < _numGpioPins; i++){
    pinMode(GPIO_PINS[i], OUTPUT);
  }
  Serial.print("Num of GPIO pins: ");
  Serial.println(_numGpioPins);
}

void loop() {
  Serial.println("All pins HIGH");
  for(int i = 0; i < _numGpioPins; i++){
    digitalWrite(GPIO_PINS[i], HIGH);
  }
  delay(DELAY_MS);

  Serial.println("All pins LOW");
  for(int i = 0; i < _numGpioPins; i++){
    digitalWrite(GPIO_PINS[i], LOW);
  }
  delay(DELAY_MS);
}

/**
 * Sets the given pin HIGH or LOW and prints the value to Serial
 */
void setLedState(int pin, int value){
  digitalWrite(pin, value);
}
