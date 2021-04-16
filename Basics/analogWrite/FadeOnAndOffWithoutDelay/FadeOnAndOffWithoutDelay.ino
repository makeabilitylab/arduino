/*
 * This example shows how to fade an LED on pin 3 using the analogWrite() function.
 * The fade goes both directions (fades on and then fades off).
 * 
 * This example uses no embedded loops and no delays.
 * 
 * The analogWrite() function uses PWM, so if you want to change the pin you're
 * using, be sure to use another PWM capable pin. On the Arduino Uno, the PWM pins
 * are identified with a "~" sign, like ~3, ~5, ~6, ~9, ~10 and ~11. 
 * 
 * By Jon Froehlich
 * http://makeabilitylab.io
 * Adapted from http://www.arduino.cc/en/Tutorial/Fade
 */

const int LED_OUTPUT_PIN = 3;
const int MIN_BRIGHTNESS = 0; // the min brightness
const int MAX_BRIGHTNESS = 255; // the max analog out value on Uno, Leonardo, etc. is 255 (8-bit max)
const int TIME_ON_EACH_FADE_VAL_MS = 30; // the time on each fade value in milliseconds

int _fadeAmount = 5;      // the amount to fade the LED by on each step
int _curBrightness = 0;   // how bright the LED is

unsigned long _fadeValueUpdatedTimestampMs = 0; // tracks the last time the LED was updated

// The setup function runs once when you press reset or power the board
void setup() {
  // set the LED pin to as an output
  pinMode(LED_OUTPUT_PIN, OUTPUT);
  Serial.begin(9600); // for using Serial.println
}

// The loop function runs over and over again forever
void loop() {

  // Get the current time since the Arduino started our program (in ms)
  unsigned long currentTimestampMs = millis();

  // Check to see if the time between now and the last time we toggled
  // the LED has exceeded our blink interval. If so, toggled the LED
  if (currentTimestampMs - _fadeValueUpdatedTimestampMs >= TIME_ON_EACH_FADE_VAL_MS) {
  
    // set the brightness of the LED pin
    analogWrite(LED_OUTPUT_PIN, _curBrightness);
    _fadeValueUpdatedTimestampMs = currentTimestampMs; // store current time as toggle time
    Serial.println(_curBrightness); // print out current brightness

    // change the brightness for next time through the loop
    _curBrightness = _curBrightness + _fadeAmount;
  
    // reverse the direction of the fading at the end of each fade direction
    if (_curBrightness <= MIN_BRIGHTNESS || _curBrightness >= MAX_BRIGHTNESS) {
      _fadeAmount = -_fadeAmount; // reverses fade direction
    }
  }
}
