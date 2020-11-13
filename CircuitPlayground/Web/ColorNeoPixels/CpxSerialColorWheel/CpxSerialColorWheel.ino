/*
 * Library reference: https://caternuson.github.io/Adafruit_CircuitPlayground/
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
    // CircuitPlayground.setPixelColor(0, 128,   0,   0);
    // CircuitPlayground.setPixelColor(1, val,   0,   0);
    int colorWheelVal = CircuitPlayground.colorWheel(val);
    Serial.println(colorWheelVal);
    for(int i = 0; i < NUM_NEO_PIXELS; i++){
      CircuitPlayground.setPixelColor(i, colorWheelVal);
    }
  }

  delay(10);
}
