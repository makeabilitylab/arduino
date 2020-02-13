
/*
 * TODO
 * 
 * This sketch is written for the 32u4 or SAMD based boards like the Arduino
 * Leondardo, Esplora, Zero, Due, which can appear as a native mouse and/or keyboard
 * when connected to the computer via USB.
 * 
 * Originally written for the Adafruit 2-Axis Joystick (https://www.adafruit.com/product/512)
 * with the select button (press down on joystick for select) but should work with any other analog 
 * inputs tied to A0 and A1 to control the x and y movements of the mouse, respectively.
 * 
 * References
 *  - https://www.arduino.cc/en/Reference.MouseKeyboard
 *  - https://www.arduino.cc/reference/en/language/functions/usb/keyboard/keyboardpress/
 *  - https://www.arduino.cc/en/Reference/KeyboardModifiers
 * 
 * By Jon Froehlich
 * @jonfroehlich
 * http://makeabilitylab.io
 */
#include <Keyboard.h> // https://www.arduino.cc/reference/en/language/functions/usb/keyboard/518,519

// Analog in pins
const int ANALOG_X_PIN = A0;
const int ANALOG_Y_PIN = A1;
const int KEYBOARD_ON_PIN = A6;

const int MAX_ANALOG_VAL = 1023;

const int BUTTON_TOGGLE_KEYBOARD_PIN = 5;
boolean isKeyboardActive = false;
int prevKeyboardToggleVal = HIGH;

void setup() {
  pinMode(BUTTON_TOGGLE_KEYBOARD_PIN, INPUT_PULLUP);
  pinMode(KEYBOARD_ON_PIN, OUTPUT);
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
    Keyboard.begin();
  }else{
    Serial.println("*** Deactivating keyboard! ***");
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
