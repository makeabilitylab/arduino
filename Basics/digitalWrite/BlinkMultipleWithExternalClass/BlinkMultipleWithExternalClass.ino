/*
 * Demonstrates how to use simple state tracking with timestamps to support
 * blinking multiple LEDs at different rates.
 * 
 * By Jon E. Froehlich
 * @jonfroehlich
 * http://makeabilitylab.io
 * 
 * For a walkthrough and circuit diagram, see:
 * https://makeabilitylab.github.io/physcomp/arduino/led-blink3
 * 
 * Adapted from the official Arduino Blink example:
 *   File -> Examples -> 02. Digital -> BlinkWithoutDelay
 *   
 * Also available here:
 *   http://www.arduino.cc/en/Tutorial/BlinkWithoutDelay
 */

#include "Blinker.h"

Blinker _led1Blinker(2, 200);
Blinker _led2Blinker(5, 333);
Blinker _led3Blinker(9, 1111);

// The setup function runs once when you press reset or power the board
void setup() {
  // empty 
}

// The loop function runs over and over again forever
void loop() {
  _led1Blinker.update();
  _led2Blinker.update();
  _led3Blinker.update();
}
