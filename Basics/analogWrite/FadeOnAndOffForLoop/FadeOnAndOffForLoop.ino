/*
 * Fades Pin 3 on and off using analogWrite
 * 
 * By Jon E. Froehlich
 * @jonfroehlich
 * http://makeabilitylab.io
 * 
 * Step-by-step tutorial:
 * https://makeabilitylab.github.io/physcomp/arduino/led-fade
 *  
 * Adapted from the official Arduino Fading example:
 * http://www.arduino.cc/en/Tutorial/Fading
 */
const int LED_OUTPUT_PIN = 3;

void setup() {
  // set the LED pin to an output
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
