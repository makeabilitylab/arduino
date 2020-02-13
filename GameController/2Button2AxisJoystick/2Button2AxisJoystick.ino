
/*
 * TODO
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
#include <Keyboard.h> // https://www.arduino.cc/reference/en/language/functions/usb/keyboard/
#include <Mouse.h> // https://www.arduino.cc/reference/en/language/functions/usb/mouse/

// Digital in pins
const int BUTTON_JOYSTICK_SEL_PIN = 2;
const int BUTTON_1_PIN = 3;
const int BUTTON_2_PIN = 4;
const int BUTTON_MOUSE_TOGGLE_PIN = 5;

// Analog in pins
const int JOYSTICK_XOUT_PIN = A0;
const int JOYSTICK_YOUT_PIN = A1;
const boolean JOYSTICK_FLIP_XY = false;

// The joysticks orientation with respect to the user
// We need this because sometimes we have to place a joystick
// upside down, etc. in our designs
enum JoystickYDirection {
  UP,
  RIGHT,
  DOWN,
  LEFT
};

enum JoystickYDirection joystickYDir = RIGHT;

const int MAX_ANALOG_VAL = 1023;
const int JOYSTICK_CENTER_VALUE = int(MAX_ANALOG_VAL / 2);
const int JOYSTICK_MOVEMENT_THRESHOLD = 10;

// Sets the overall mouse sensitivity based on joystick movement
// a higher value will move the mouse more with joystick movement
const int MAX_MOUSE_MOVE_VAL = 30; 

boolean isButton1Pressed = false;
const char button1Char = 'a';

boolean isButton2Pressed = false;
const char button2Char = 'b';

boolean isJoystickSelPressed = false;
int prevJoystickSelPressedVal = HIGH;

boolean isMouseActive = false;
int prevButtonMouseToggleVal = HIGH;

void setup() {
  pinMode(BUTTON_JOYSTICK_SEL_PIN, INPUT_PULLUP);
  pinMode(BUTTON_1_PIN, INPUT_PULLUP);
  pinMode(BUTTON_2_PIN, INPUT_PULLUP);
  pinMode(BUTTON_MOUSE_TOGGLE_PIN, INPUT_PULLUP);

  // Turn on serial for debugging
  Serial.begin(9600); 

  // Activate mouse and keyboard
  Keyboard.begin();
  activateMouse(true);
}

void activateMouse(boolean turnMouseOn){
  if(turnMouseOn){
    Serial.println("*** Activating mouse! ***"); 
    Mouse.begin();
  }else{
    Serial.println("*** Deactivating mouse! ***");
    isMouseActive = false;
    Mouse.end();
  }

  isMouseActive = turnMouseOn;
}

void loop() {
  int buttonJoystickSelVal = digitalRead(BUTTON_JOYSTICK_SEL_PIN);
  int buttonMouseToggleVal = digitalRead(BUTTON_MOUSE_TOGGLE_PIN);

  // Check to see if we should activate the mouse
  if(buttonMouseToggleVal != prevButtonMouseToggleVal){
    if(buttonMouseToggleVal == LOW && isMouseActive == false){
      activateMouse(true);
    }else if(buttonMouseToggleVal == LOW && isMouseActive == true){
      activateMouse(false);
    }
  }

  /** HANDLE JOYSTICK INPUT AS MOUSE **/
  int joystickXVal = analogRead(JOYSTICK_XOUT_PIN);
  int joystickYVal = analogRead(JOYSTICK_YOUT_PIN);

  // If hooked up on the breadboard, we often have to 
  // install the joystick in a different orientation than
  // with the Y up direction facing up. The code below
  // handles the different orientations
  if(joystickYDir == RIGHT){
    int tmpX = joystickXVal;
    joystickXVal = joystickYVal;
    joystickYVal = MAX_ANALOG_VAL - tmpX;
  }else if(joystickYDir == DOWN){
    joystickYVal = MAX_ANALOG_VAL - joystickYVal;
    joystickXVal = MAX_ANALOG_VAL - joystickXVal;
  }else if(joystickYDir == LEFT){
    int tmpX = joystickXVal;
    joystickXVal = MAX_ANALOG_VAL - joystickYVal;
    joystickYVal = tmpX;
  }

  Serial.print("joystickXVal: ");
  Serial.print(joystickXVal);
  Serial.print(" joystickYVal: ");
  Serial.println(joystickYVal);

  // Check to see if the joystick position has moved a certain
  // threshold amount beyond its center point for each axis
  // If so, calculate the movement values respectively
  int yDistFromCenter = joystickYVal - JOYSTICK_CENTER_VALUE;
  int xDistFromCenter = joystickXVal - JOYSTICK_CENTER_VALUE;
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

  // Handle the joystick press as a mouse press
  if(prevJoystickSelPressedVal != buttonJoystickSelVal){
    if(buttonJoystickSelVal == LOW){ 
      Mouse.press();
      Serial.println("** MOUSE PRESS **");
    }else if(buttonJoystickSelVal == HIGH){
      Mouse.release();
      Serial.println("** MOUSE RELEASE **");
    }
  }

  /** HANDLE BUTTON INPUT AS KEYBOARD **/
  int button1Val = digitalRead(BUTTON_1_PIN);
  int button2Val = digitalRead(BUTTON_2_PIN);
  int numButtonsPressed = 0;

  // List of non-alphanumerica keys:
  //  - https://www.arduino.cc/en/Reference/KeyboardModifiers
  if(button1Val == LOW){
    isButton1Pressed = true;
    Keyboard.press(button1Char);
    Serial.print("'");
    Serial.print(button1Char);
    Serial.print("': Pressed\t");
    numButtonsPressed++;
  }else if(isButton1Pressed == true && button1Val == HIGH){
    Keyboard.release(button1Char);
    isButton1Pressed = false;
  }

  if(button2Val == LOW){
    isButton2Pressed = true;
    Serial.print("'");
    Serial.print(button2Char);
    Serial.print("': Pressed\t");
    numButtonsPressed++;
  }else if(isButton2Pressed == true && button2Val == HIGH){
    Keyboard.release(button2Char);
    isButton2Pressed = false;
  }
  
  delay(50);

  if(numButtonsPressed > 0){
    Serial.println();
  }

  prevButtonMouseToggleVal = buttonMouseToggleVal;
  prevJoystickSelPressedVal = buttonJoystickSelVal;
  
}
