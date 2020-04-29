
const int GPIO_PINS[] = {26, 25, 4, 5, 18, 19, 16, 17, 21, 13, 12, 27, 33, 15, 32, 14, 22, 23};

// For some reason, A12 is not defined. So, using raw literal 13 instead
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
