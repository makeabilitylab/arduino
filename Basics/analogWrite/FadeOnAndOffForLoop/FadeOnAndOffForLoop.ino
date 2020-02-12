const int LED_OUTPUT_PIN = 3;

void setup() {
  // set the LED pin to as an output
  pinMode(LED_OUTPUT_PIN, OUTPUT);
}

void loop(){
  // fade on
  for(int i = 0; i <= 255; i += 1){
    analogWrite(LED_OUTPUT_PIN, i);
    delay(30);
  }

  //fade off
  for(int i = 255; i >= 0; i -= 1){
    analogWrite(LED_OUTPUT_PIN, i);
    delay(30);
  }
}
