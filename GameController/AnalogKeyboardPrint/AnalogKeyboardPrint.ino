
/* Sends the analog input on A0 and A1 as comma separated keyboard values, which can be read
 * in by a program running on the computer (including p5js). For example, if A0 is 283
 * and A1 is 768, this sketch will "press" the following keys in sequence:
 *   2
 *   8
 *   3
 *   ,
 *   7
 *   6
 *   8
 *   \n 
 * Which can then be parsed by a computer program running on your laptop/computer.
 * 
 * Note: the keyboard starts in OFF mode so as not to write random text all over the place.
 * You can enable it by pressing a toggle button hooked up to pin 2 (with a pull-up configuration).
 * 
 * As an example, run this sketch on your Arduino, then check out the 'Analog Keyboard' 
 * p5js sketch: 
 *  - https://editor.p5js.org/jonfroehlich/sketches/f060jgy7
 * 
 * This sketch will work with any 32u4- or SAMD-based boards like the Arduino
 * Leondardo, Esplora, Zero, Due, which can appear as a native mouse and/or keyboard
 * when connected to the computer via USB.

 * 
 * By Jon Froehlich
 * @jonfroehlich
 * http://makeabilitylab.io
 */
#include <Keyboard.h> // https://www.arduino.cc/reference/en/language/functions/usb/keyboard/518,519

// Analog in pins
const int ANALOG_X_PIN = A0;
const int ANALOG_Y_PIN = A1;
const int MAX_ANALOG_VAL = 1023;

// Digital I/O pins
const int BUTTON_TOGGLE_KEYBOARD_PIN = 2;
const int KEYBOARD_ON_LED_PIN = 13;

boolean isKeyboardActive = false;
int prevKeyboardToggleVal = HIGH;

void setup() {
  pinMode(BUTTON_TOGGLE_KEYBOARD_PIN, INPUT_PULLUP);
  pinMode(KEYBOARD_ON_LED_PIN, OUTPUT);
  
  // Turn on serial for debugging
  Serial.begin(9600);

  // Importantly, we need to start with the keyboard off
  // So that it doesn't immediately start to inadvertently
  // type text everywhere :)
  activateKeyboard(false); 
}

void activateKeyboard(boolean turnKeyboardOn){
  if(turnKeyboardOn){
    Serial.println("*** Activating keyboard! ***"); 
    digitalWrite(KEYBOARD_ON_LED_PIN, HIGH);
    Keyboard.begin();
  }else{
    Serial.println("*** Deactivating keyboard! ***");
    digitalWrite(KEYBOARD_ON_LED_PIN, LOW);
    Keyboard.end();
  }

  isKeyboardActive = turnKeyboardOn;
}

void loop() {
 
  int keyboardToggleVal = digitalRead(BUTTON_TOGGLE_KEYBOARD_PIN);
  if(keyboardToggleVal != prevKeyboardToggleVal){
    if(keyboardToggleVal == LOW){ // button pressed
      activateKeyboard(!isKeyboardActive);
    }
  }

  if(isKeyboardActive){
    int xVal = analogRead(ANALOG_X_PIN);
    int yVal = analogRead(ANALOG_Y_PIN);
    Keyboard.print(xVal);
    Keyboard.print(",");
    Keyboard.println(yVal);

    // for debugging
    Serial.print(xVal);
    Serial.print(",");
    Serial.println(yVal);
  }
  
  delay(100);
  prevKeyboardToggleVal = keyboardToggleVal;
}
