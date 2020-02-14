
/* Converts analog input on A0 and A1 to control the x and y movements of the mouse
 * Uses digital input on pins 2, 3, and 4 for mouse press and two controllable
 * keyboard presses (currently assigned to the key 'a' and 'b' respectively to
 * mimic the original NES8 controllers).
 * 
 * The last digital input on pin 5 is used to toggle through three joystick states:
 *   1. OFF: the joystick is off and does not control the mouse
 *   2. MOUSE: the joystick is used to control the mouse
 *   3. KEYBOARD: the joystick is used to control UP, DOWN, LEFT, and RIGHT arrow keys
 * 
 * Originally written for the Adafruit 2-Axis Joystick (https://www.adafruit.com/product/512)
 * with the select button (press down on joystick for select) but should work with any other analog 
 * inputs tied to A0 and A1 to control the x and y movements of the mouse, respectively.
 * 
 * This sketch will work with any 32u4- or SAMD-based boards like the Arduino
 * Leondardo, Esplora, Zero, Due, which can appear as a native mouse and/or keyboard
 * when connected to the computer via USB.
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

// The joystick can be off or can control the mouse or keyboard
// For the keyboard, the UP movement on the joystick is mapped to the
// UP ARROW on keyboard, the RIGHT movement on the joystick is mapped
// to the RIGHT ARROW on the keyboard, etc.
enum JoystickMode{
  OFF,
  MOUSE,
  KEYBOARD 
};

enum JoystickMode joystickMode = OFF;

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

boolean isUpKeyPressed = false;
boolean isRightKeyPressed = false;
boolean isDownKeyPressed = false;
boolean isLeftKeyPressed = false;

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

JoystickMode getNextJoystickMode(){
  switch(joystickMode){
    case OFF:
      Serial.println("** Switching joystick to control MOUSE **");
      return MOUSE;
    case MOUSE:
      Serial.println("** Switching joystick to control KEYBOARD **");
      return KEYBOARD;
    default:
      Serial.println("** Switching joystick to OFF **");
      return OFF;
  }
}

void loop() {
  int buttonJoystickSelVal = digitalRead(BUTTON_JOYSTICK_SEL_PIN);
  int buttonMouseToggleVal = digitalRead(BUTTON_MOUSE_TOGGLE_PIN);

  // Check to see if we should activate the mouse
  if(buttonMouseToggleVal != prevButtonMouseToggleVal){
    if(buttonMouseToggleVal == LOW){
      joystickMode = getNextJoystickMode();
    }
  }

  /** HANDLE JOYSTICK INPUT **/
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

  if(joystickMode == MOUSE){
    Mouse.move(xMouse, yMouse, 0);
  }else if(joystickMode == KEYBOARD){
    // If we're here, we want to convert the joystick position
    // as keyboard input. So, moving up on the joystick, will
    // send an UP ARROW key to the computer. Moving left
    // on the joystick, will send a LEFT ARROW key, etc.
    if(xMouse < 0 && isLeftKeyPressed == false){
      Keyboard.press(KEY_LEFT_ARROW);
      isLeftKeyPressed = true;
    }else if(xMouse >= 0 && isLeftKeyPressed == true){
      Keyboard.release(KEY_LEFT_ARROW);
      isLeftKeyPressed = false;
    }
    
    if(xMouse > 0 && isRightKeyPressed == false){
      Keyboard.press(KEY_RIGHT_ARROW);
      isRightKeyPressed = true;
    }else if(xMouse <= 0 && isRightKeyPressed == true){
      Keyboard.release(KEY_RIGHT_ARROW);
      isRightKeyPressed = false;
    }
  
    if(yMouse < 0 && isUpKeyPressed == false){
      Keyboard.press(KEY_UP_ARROW);
      isUpKeyPressed = true;
    }else if(yMouse >= 0 && isUpKeyPressed == true){
      Keyboard.release(KEY_UP_ARROW);
      isUpKeyPressed = false;
    }
  
    if(yMouse > 0 && isDownKeyPressed == false){
      Keyboard.press(KEY_DOWN_ARROW);
      isDownKeyPressed = true;
    }else if(yMouse <= 0 && isDownKeyPressed == true){
      Keyboard.release(KEY_DOWN_ARROW);
      isDownKeyPressed = false;
    }
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
