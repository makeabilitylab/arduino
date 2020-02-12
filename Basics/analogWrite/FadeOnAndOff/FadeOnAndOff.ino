/*
 * This example shows how to fade an LED on pin 3 using the analogWrite() function.
 * The fade goes both directions (fades on and then fades off)
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

int _fadeAmount = 5;      // the amount to fade the LED by on each step
int _curBrightness = 0;   // how bright the LED is

// The setup function runs once when you press reset or power the board
void setup() {
  // set the LED pin to as an output
  pinMode(LED_OUTPUT_PIN, OUTPUT);
  Serial.begin(9600); // for using Serial.println
}

// The loop function runs over and over again forever
void loop() {
  
  // set the brightness of the LED pin
  analogWrite(LED_OUTPUT_PIN, _curBrightness);
  Serial.println(_curBrightness); // print out current brightness

  // change the brightness for next time through the loop
  _curBrightness = _curBrightness + _fadeAmount;

  // reverse the direction of the fading at the end of each fade direction
  if (_curBrightness <= MIN_BRIGHTNESS || _curBrightness >= MAX_BRIGHTNESS) {
    _fadeAmount = -_fadeAmount; // reverses fade direction
  }
  
  // wait for 30 milliseconds to see the dimming effect
  delay(30);
}

/*
 * While correct, the following code is an inferior implementation of the fade program, 
 * which is why it's commented out. I included it for educational purposes.
 * 
 * Can you think of why?
 * 
 * Answer: generally, we want to keep the loop() function as fast as possible
 * as it will increase the perceived responsiveness of our program. While
 * it doesn't matter in this case--because this code is so simple--generally,
 * your Arduino code will be performing read operations (e.g., digitalRead, digitalWrite). 
 * If you're stuck in these long 'for loops' with lots of delays, you are going to
 * negatively impact your sampling rate of your read calls. 
 */
//void loop(){
//  for(int i = 0; i <= 255; i += _fadeAmount){
//    analogWrite(LED_OUTPUT_PIN, i);
//    delay(30);
//  }
//
//  for(int i = 255; i >= 0; i -= _fadeAmount){
//    analogWrite(LED_OUTPUT_PIN, i);
//    delay(30);
//  }
//}
