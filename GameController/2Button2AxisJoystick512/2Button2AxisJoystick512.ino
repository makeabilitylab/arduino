
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
#include <Keyboard.h> // https://www.arduino.cc/reference/en/language/functions/usb/keyboard/
#include <Mouse.h> // https://www.arduino.cc/reference/en/language/functions/usb/mouse/

// Digital in pins
const int BUTTON_JOYSTICK_SEL_PIN = 2;
const int BUTTON_A_PIN = 3;
const int BUTTON_B_PIN = 4;
const int BUTTON_MOUSE_TOGGLE_PIN = 5;

// Analog in pins
const int JOYSTICK_XOUT_PIN = A0;
const int JOYSTICK_YOUT_PIN = A1;
const boolean JOYSTICK_FLIP_XY = true;

const int MAX_ANALOG_VAL = 1023;
const int JOYSTICK_CENTER_VALUE = int(MAX_ANALOG_VAL / 2);
const int JOYSTICK_MOVEMENT_THRESHOLD = 10;

// Sets the overall mouse sensitivity based on joystick movement
// a higher value will move the mouse more with joystick movement
const int MAX_MOUSE_MOVE_VAL = 30; 

boolean isAButtonPressed = false;
boolean isBButtonPressed = false;
boolean isJoystickSelPressed = false;
int prevJoystickSelPressedVal = HIGH;

boolean isMouseActive = false;
int prevButtonMouseToggleVal = HIGH;

void setup() {
  pinMode(BUTTON_JOYSTICK_SEL_PIN, INPUT_PULLUP);
  pinMode(BUTTON_A_PIN, INPUT_PULLUP);
  pinMode(BUTTON_B_PIN, INPUT_PULLUP);
  pinMode(BUTTON_MOUSE_TOGGLE_PIN, INPUT_PULLUP);

  // Turn on serial for debugging
  Serial.begin(9600); 

  // Activate mouse and keyboard
  Keyboard.begin();
  //Mouse.begin();
}

void loop() {
  int buttonJoystickSelVal = digitalRead(BUTTON_JOYSTICK_SEL_PIN);
  int buttonAVal = digitalRead(BUTTON_A_PIN);
  int buttonBVal = digitalRead(BUTTON_B_PIN);
  int buttonMouseToggleVal = digitalRead(BUTTON_MOUSE_TOGGLE_PIN);

  if(buttonMouseToggleVal != prevButtonMouseToggleVal){
    if(buttonMouseToggleVal == LOW && isMouseActive == false){
      Serial.println("*** Activating mouse! ***");
      isMouseActive = true;
      Mouse.begin();
    }else if(buttonMouseToggleVal == LOW && isMouseActive == true){
      Serial.println("*** Deactivating mouse! ***");
      isMouseActive = false;
      Mouse.end();
    }
  }

  /** HANDLE JOYSTICK INPUT AS MOUSE **/
  int joystickXVal = analogRead(JOYSTICK_XOUT_PIN);
  int joystickYVal = analogRead(JOYSTICK_YOUT_PIN);

  // If hooked up on the breadboard, we typically have to
  // flip the axes to get everything to fit and work together
  if(JOYSTICK_FLIP_XY == true){
    int tmpX = joystickXVal;
    joystickXVal = joystickYVal;
    joystickYVal = MAX_ANALOG_VAL - tmpX;
  }

  Serial.print("joystickXVal: ");
  Serial.print(joystickXVal);
  Serial.print(" joystickYVal: ");
  Serial.println(joystickYVal);
 
  int yDistFromCenter = joystickYVal - JOYSTICK_CENTER_VALUE;
  int xDistFromCenter = joystickXVal - JOYSTICK_CENTER_VALUE;
 
  Serial.print(" xDistFromCenter: ");
  Serial.print(xDistFromCenter);  
  Serial.print("yDistFromCenter: ");
  Serial.println(yDistFromCenter);  

  int yMouse = 0, xMouse = 0;
  if(abs(yDistFromCenter) > JOYSTICK_MOVEMENT_THRESHOLD){
    yMouse = map(joystickYVal, 0, MAX_ANALOG_VAL, MAX_MOUSE_MOVE_VAL, -MAX_MOUSE_MOVE_VAL);
  }
  
  if(abs(xDistFromCenter) > JOYSTICK_MOVEMENT_THRESHOLD){
    xMouse = map(joystickXVal, 0, MAX_ANALOG_VAL, -MAX_MOUSE_MOVE_VAL, MAX_MOUSE_MOVE_VAL);
  }

  if(isMouseActive){
    Mouse.move(xMouse, yMouse, 0);
  }       

  // TODO: investigate whether this joystick selection is using pull up or down
  // resistor...
  if(prevJoystickSelPressedVal != buttonJoystickSelVal){
    if(buttonJoystickSelVal == LOW && isJoystickSelPressed == false){ 
      isJoystickSelPressed = true;
      Mouse.press();
    }else if(buttonJoystickSelVal == LOW && isJoystickSelPressed == true){
      isJoystickSelPressed = false;
      Mouse.release();
    }
  }

  /** HANDLE BUTTON INPUT AS KEYBOARD **/
  int numButtonsPressed = 0;

  // List of non-alphanumerica keys:
  //  - https://www.arduino.cc/en/Reference/KeyboardModifiers
  if(buttonAVal == LOW){
    isAButtonPressed = true;
    Keyboard.press('a');
    Serial.print("'a': Pressed\t");
    numButtonsPressed++;
  }else if(isAButtonPressed == true && buttonAVal == HIGH){
    Keyboard.release('a');
    isAButtonPressed = false;
  }

  if(buttonBVal == LOW){
    isBButtonPressed = true;
    Keyboard.press('b');
    Serial.print("'b': Pressed\t");
    numButtonsPressed++;
  }else if(isBButtonPressed == true && buttonBVal == HIGH){
    Keyboard.release('b');
    isBButtonPressed = false;
  }
  
  delay(50);

  if(numButtonsPressed > 0){
    Serial.println();
  }

  prevButtonMouseToggleVal = buttonMouseToggleVal;
  prevJoystickSelPressedVal = buttonJoystickSelVal;
  
}
