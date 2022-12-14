
// Need to explicitly include TinyUSB to user Serial on the nRF52840
// https://github.com/adafruit/Adafruit_nRF52_Arduino/issues/653
#include "Adafruit_TinyUSB.h"

void setup() {
  Serial.begin(9600);
  pinMode(LED_BUILTIN, OUTPUT);
}

void loop() {
  Serial.print(".");
  digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(1000);                       // wait for a second
  digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
  delay(1000);                       // wait for a second
}