/**
 * This example reads all of the analog input pins on the Huzzah32 and prints
 * their values to Serial. The official Adafruit Huzzah32 docs state that
 * ADC#1 only works with WiFi has started (bottom of page):
 * https://learn.adafruit.com/adafruit-huzzah32-esp32-feather/pinouts
 * 
 * However, we used this program to show that this is not true.
 * 
 * By Jon E. Froehlich
 * @jonfroehlich
 * http://makeabilitylab.io
 * 
 * See our ESP32 guide here:
 * https://makeabilitylab.github.io/physcomp/esp32/
 * 
 */

const int GPIO_PINS[] = {26, 25, 4, 5, 18, 19, 16, 17, 21, 13, 12, 27, 33, 15, 32, 14, 22, 23};

// For some reason, A12 is not defined. So, using raw literal 13 instead
// ADC#1 includes A2, A3, A4, A7, A9
// ADC#2 includes A0, A1, A5, A6, A8, A10, A11, and A12
const int ANALOG_INPUT_PINS[] = { A0, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, A11, 13 };

int _numAnalogInputPins = 0;
const boolean PRETTY_PRINT = false;

void setup() {
  Serial.begin(9600);

  _numAnalogInputPins = sizeof(ANALOG_INPUT_PINS) / sizeof(ANALOG_INPUT_PINS[0]);
  for(int i = 0; i < _numAnalogInputPins; i++){
    int pin = ANALOG_INPUT_PINS[i];
    pinMode(pin, INPUT);
  }
}

void loop() {
  for(int i = 0; i < _numAnalogInputPins; i++){
    int pin = ANALOG_INPUT_PINS[i];
    int analogVal = analogRead(pin);
    if(PRETTY_PRINT){
      Serial.print((String)"Pin " + pin + " = " + analogVal + ", ");
    }else{
      Serial.print(analogVal);
      Serial.print(", ");
    }  
  }
  Serial.println();
  delay(10);
}
