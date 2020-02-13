
/*
 * TODO
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
#include <Keyboard.h>

const int BUTTON_UP_PIN = 2;
const int BUTTON_RIGHT_PIN = 3;
const int BUTTON_DOWN_PIN = 4;
const int BUTTON_LEFT_PIN = 5;
const int BUTTON_SPACEBAR_PIN = 6;

void setup() {
  pinMode(BUTTON_UP_PIN, INPUT_PULLUP);
  pinMode(BUTTON_RIGHT_PIN, INPUT_PULLUP);
  pinMode(BUTTON_DOWN_PIN, INPUT_PULLUP);
  pinMode(BUTTON_LEFT_PIN, INPUT_PULLUP);
  pinMode(BUTTON_SPACEBAR_PIN, INPUT_PULLUP);

  // Turn on serial for debugging
  Serial.begin(9600); 
  Keyboard.begin();
}

void loop() {
  int buttonUpVal = digitalRead(BUTTON_UP_PIN);
  int buttonRightVal = digitalRead(BUTTON_RIGHT_PIN);
  int buttonDownVal = digitalRead(BUTTON_DOWN_PIN);
  int buttonLeftVal = digitalRead(BUTTON_LEFT_PIN);
  int buttonSpaceBarVal = digitalRead(BUTTON_SPACEBAR_PIN);
  int numButtonsPressed = 0;

  // List of non-alphanumerica keys:
  //  - https://www.arduino.cc/en/Reference/KeyboardModifiers
  if(buttonUpVal == LOW){
    Keyboard.press(KEY_UP_ARROW);
    Serial.print("UP: Pressed\t");
    numButtonsPressed++;
  }

  if(buttonRightVal == LOW){
    Keyboard.press(KEY_RIGHT_ARROW);
    Serial.print("RIGHT: Pressed\t");
    numButtonsPressed++;
  }

  if(buttonDownVal == LOW){
    Keyboard.press(KEY_DOWN_ARROW);
    Serial.print("DOWN: Pressed\t");
    numButtonsPressed++;
  }

  if(buttonLeftVal == LOW){
    Keyboard.press(KEY_LEFT_ARROW);
    Serial.print("LEFT: Pressed");
    numButtonsPressed++;
  }

  if(buttonSpaceBarVal == LOW){
    Keyboard.press(' ');
    Serial.print("SPACE BAR: Pressed\t");
    numButtonsPressed++;
  }
  
  delay(50);
  Keyboard.releaseAll();

  if(numButtonsPressed > 0){
    Serial.println();
  }
}
