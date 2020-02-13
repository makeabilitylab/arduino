
/*
 * Repeatedly sends keyboard press if a button is held down
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
#include <Keyboard.h> // https://www.arduino.cc/reference/en/language/functions/usb/keyboard/
#include <Mouse.h> // https://www.arduino.cc/reference/en/language/functions/usb/mouse/

const int BUTTON_UP_PIN = 2;
const int BUTTON_RIGHT_PIN = 3;
const int BUTTON_DOWN_PIN = 4;
const int BUTTON_LEFT_PIN = 5;
const int BUTTON_SPACEBAR_PIN = 6;

const int JOYSTICK_UPDOWN_PIN = A0;
const int JOYSTICK_LEFTRIGHT_PIN = A1;
const boolean JOYSTICK_FLIP_XY = true;

const int MAX_ANALOG_VAL = 1023;
const int JOYSTICK_CENTER_VALUE = int(MAX_ANALOG_VAL / 2);
const int JOYSTICK_MOVEMENT_THRESHOLD = 10;

// Sets the overall mouse sensitivity based on joystick movement
// a higher value will move the mouse more with joystick movement
const int MAX_MOUSE_MOVE_VAL = 30; 

void setup() {
  pinMode(BUTTON_UP_PIN, INPUT_PULLUP);
  pinMode(BUTTON_RIGHT_PIN, INPUT_PULLUP);
  pinMode(BUTTON_DOWN_PIN, INPUT_PULLUP);
  pinMode(BUTTON_LEFT_PIN, INPUT_PULLUP);
  pinMode(BUTTON_SPACEBAR_PIN, INPUT_PULLUP);

  // Turn on serial for debugging
  Serial.begin(9600); 
  Keyboard.begin();
  Mouse.begin();
}

void loop() {
  int buttonUpVal = digitalRead(BUTTON_UP_PIN);
  int buttonRightVal = digitalRead(BUTTON_RIGHT_PIN);
  int buttonDownVal = digitalRead(BUTTON_DOWN_PIN);
  int buttonLeftVal = digitalRead(BUTTON_LEFT_PIN);
  int buttonSpaceBarVal = digitalRead(BUTTON_SPACEBAR_PIN);
  int numButtonsPressed = 0;

  /** HANDLE JOYSTICK INPUT AS MOUSE **/
  int joystickUpDownVal = analogRead(JOYSTICK_UPDOWN_PIN);
  int joystickLeftRightVal = analogRead(JOYSTICK_LEFTRIGHT_PIN);

  // If hooked up on the breadboard, we typically have to
  // flip the axes to get everything to fit and work together
  if(JOYSTICK_FLIP_XY == true){
    int tmpXY = joystickLeftRightVal;
    joystickLeftRightVal = joystickUpDownVal;
    joystickUpDownVal = MAX_ANALOG_VAL - tmpXY;
  }
  
  Serial.print("joystickUpDownVal: ");
  Serial.print(joystickUpDownVal);
  Serial.print(" joystickLeftRightVal: ");
  Serial.println(joystickLeftRightVal);

  int yDistFromCenter = joystickUpDownVal - JOYSTICK_CENTER_VALUE;
  int xDistFromCenter = joystickLeftRightVal - JOYSTICK_CENTER_VALUE;
  Serial.print("yDistFromCenter: ");
  Serial.print(yDistFromCenter);
  Serial.print(" xDistFromCenter: ");
  Serial.println(xDistFromCenter);    

  int yMouse = 0, xMouse = 0;
  if(abs(yDistFromCenter) > JOYSTICK_MOVEMENT_THRESHOLD){
    yMouse = map(joystickUpDownVal, 0, MAX_ANALOG_VAL, MAX_MOUSE_MOVE_VAL, -MAX_MOUSE_MOVE_VAL);
  }
  
  if(abs(xDistFromCenter) > JOYSTICK_MOVEMENT_THRESHOLD){
    xMouse = map(joystickLeftRightVal, 0, MAX_ANALOG_VAL, -MAX_MOUSE_MOVE_VAL, MAX_MOUSE_MOVE_VAL);
  }

  Mouse.move(xMouse, yMouse, 0);        

  /** HANDLE BUTTON INPUT AS KEYBOARD **/

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
  
  
  Keyboard.releaseAll();
  delay(50);

  if(numButtonsPressed > 0){
    Serial.println();
  }
}
