/*
 * This example shows how to fade an LED on Pin 3 using the analogWrite() function.
 * The fade goes in one direction (from off to a fade on) and then repeats (off to fade on)
 * It's a slightly simpler version of FadeOn (because doesn't use constants)
 * 
 * The analogWrite() function uses PWM, so if you want to change the pin you're
 * using, be sure to use another PWM capable pin. On the Arduino Uno, the PWM pins
 * are identified with a "~" sign, like ~3, ~5, ~6, ~9, ~10 and ~11. 
 * 
 * By Jon Froehlich
 * http://makeabilitylab.io
 * Adapted from http://www.arduino.cc/en/Tutorial/Fade
 * 
 */

int _curBrightness = 0;   // how bright the LED is (between 0 - 255)

void setup() {
  pinMode(3, OUTPUT);
}

void loop() {

  // set the brightness of the LED pin
  analogWrite(3, _curBrightness);

  // change the brightness for next time through the loop
  _curBrightness = _curBrightness + 1;

  // check to see if we are beyond the max val (255)
  // and if we are, reset the brightness to 0 (which is off)
  if (_curBrightness > 255){
    _curBrightness = 0;
  }
  
  // wait for 30 milliseconds to see the dimming effect
  delay(30);
}
