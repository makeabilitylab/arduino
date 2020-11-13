/**
 * This example receives an integer value (as a text string) delimited by a return character '\n',
 * converts the text into an integer, and uses this to set the "brightness" of the red LEDs
 * on the NeoPixels.
 * 
 * While this example will work with any Serial program, I wrote it to demonstrate the
 * Serial API for web browsers (currently only works with Chrome). This new API allows 
 * developers to write JavaScript code in a web browser to read/write data over the serial
 * port, including to devices like Arduino.
 * 
 * Circuit Playground API reference: 
 * https://caternuson.github.io/Adafruit_CircuitPlayground/
 * 
 * Web Serial API:
 * https://web.dev/serial/
 * 
 * By Jon E. Froehlich
 * @jonfroehlich
 * http://makeabilitylab.io
 * 
 */
#include <Adafruit_CircuitPlayground.h>

const int NUM_NEO_PIXELS = 10;

void setup() {
  CircuitPlayground.begin();
  //  CircuitPlayground.clearPixels();
}

void loop() {
  if(Serial.available() > 0){
    String s1 = Serial.readStringUntil('\n');// s1 is String type variable.
    Serial.println(s1);//display same received Data back in serial monitor.
    int val = s1.toInt();
    for(int i = 0; i < NUM_NEO_PIXELS; i++){
      CircuitPlayground.setPixelColor(i, val, 0, 0);
    }
  }

  delay(10);
}
