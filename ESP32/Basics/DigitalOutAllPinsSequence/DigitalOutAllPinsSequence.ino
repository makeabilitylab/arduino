/**
 * This example blinks all pins in sequence using digitalWrite
 * 
 * There are 21 GPIO pins on the ESP32; however, on the Huzzah32, pins 34 (A2), 39 (A3), 36 (A4) 
 * are not output-capable and should not work by design. We still include them here for completeness
 * 
 * By Jon E. Froehlich
 * @jonfroehlich
 * http://makeabilitylab.io
 *
 */

const int DELAY_MS = 1000; // blink delay in MS

// Pins start with 26 at A0 and switch to other side of board at 13 (A12)
// According to the Huzzah32 docs, 34 (A2), 39 (A3), 36 (A4) are not output-capable so should not work by design 
// See: https://learn.adafruit.com/adafruit-huzzah32-esp32-feather/pinouts
// But we've included them here anyway for completeness (and to double check!)
const int GPIO_PINS[] = {26, 25, 34, 39, 36, 4, 5, 18, 19, 16, 17, 21, 13, 12, 27, 33, 15, 32, 14, 22, 23};

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
  // put your main code here, to run repeatedly:
  for(int i = 0; i < _numGpioPins; i++){
    int pin = GPIO_PINS[i];

    Serial.print("Pin ");
    Serial.print(pin);
    Serial.print(" ");
    Serial.println("HIGH");
    
    setLedState(pin, HIGH);
    delay(DELAY_MS);
    setLedState(pin, LOW);
  }
}

/**
 * Sets the given pin HIGH or LOW and prints the value to Serial
 */
void setLedState(int pin, int value){
  digitalWrite(pin, value);
}
