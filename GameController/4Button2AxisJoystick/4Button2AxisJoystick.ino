
/* Converts analog input on A0 and A1 to control the x and y movements of the mouse
 * Uses digital input on pins 2, 3, 4, 5, 6 for UP_ARROW, RIGHT_ARROW, DOWN_ARROW, 
 * LEFT_ARROW, and SPACE BAR respectively on the keyboard. 
 * 
 * Digital input on pin 7 toggles the joystick on and off.
 * 
 * All digital input is assumed to use the Arduino's internal pull-up resistors
 * 
 * Originally written for the Parallax 2-Axis Joystick (https://www.adafruit.com/product/245)
 * but should work with any other analog inputs tied to A0 and A1 to control the x and y movements 
 * of the mouse, respectively.
 * 
 * This sketch is compatible with any 32u4- or SAMD-based boards like the Arduino
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

const int BUTTON_UP_PIN = 2;
const int BUTTON_RIGHT_PIN = 3;
const int BUTTON_DOWN_PIN = 4;
const int BUTTON_LEFT_PIN = 5;
const int BUTTON_SPACEBAR_PIN = 6;
const int BUTTON_MOUSE_TOGGLE_PIN = 7;

const int JOYSTICK_UPDOWN_PIN = A0;
const int JOYSTICK_LEFTRIGHT_PIN = A1;

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

// Sets the overall amount of movement in either X or Y direction
// that will trigger the Arduino board *sending* a Mouse.move()
// command to the computer
const int JOYSTICK_MOVEMENT_THRESHOLD = 12; 

// Sets the overall mouse sensitivity based on joystick movement
// a higher value will move the mouse more with joystick movement
const int MAX_MOUSE_MOVE_VAL = 30; 

boolean isSpaceBarPressed = false;
boolean isUpKeyPressed = false;
boolean isRightKeyPressed = false;
boolean isDownKeyPressed = false;
boolean isLeftKeyPressed = false;
boolean isMouseActive = true;
int prevButtonMouseToggleVal = HIGH;

void setup() {
  pinMode(BUTTON_UP_PIN, INPUT_PULLUP);
  pinMode(BUTTON_RIGHT_PIN, INPUT_PULLUP);
  pinMode(BUTTON_DOWN_PIN, INPUT_PULLUP);
  pinMode(BUTTON_LEFT_PIN, INPUT_PULLUP);
  pinMode(BUTTON_SPACEBAR_PIN, INPUT_PULLUP);
  pinMode(BUTTON_MOUSE_TOGGLE_PIN, INPUT_PULLUP);

  // Turn on serial for debugging
  Serial.begin(9600); 
  Keyboard.begin();
  activateMouse(true);
}

void activateMouse(boolean turnMouseOn){
  if(turnMouseOn){
    Serial.println("*** Activating mouse! ***"); 
    Mouse.begin();
  }else{
    Serial.println("*** Deactivating mouse! ***");
    Mouse.end();
  }

  isMouseActive = turnMouseOn;
}

void loop() {
  int buttonUpVal = digitalRead(BUTTON_UP_PIN);
  int buttonRightVal = digitalRead(BUTTON_RIGHT_PIN);
  int buttonDownVal = digitalRead(BUTTON_DOWN_PIN);
  int buttonLeftVal = digitalRead(BUTTON_LEFT_PIN);
  int buttonSpaceBarVal = digitalRead(BUTTON_SPACEBAR_PIN);
  int buttonMouseToggleVal = digitalRead(BUTTON_MOUSE_TOGGLE_PIN);

  if(buttonMouseToggleVal != prevButtonMouseToggleVal){
    if(buttonMouseToggleVal == LOW && isMouseActive == false){
      activateMouse(true);
    }else if(buttonMouseToggleVal == LOW && isMouseActive == true){
      activateMouse(false);
    }
  }

  /** HANDLE JOYSTICK INPUT AS MOUSE **/
  int joystickUpDownVal = analogRead(JOYSTICK_UPDOWN_PIN);
  int joystickLeftRightVal = analogRead(JOYSTICK_LEFTRIGHT_PIN);

  // If hooked up on the breadboard, we often have to 
  // install the joystick in a different orientation than
  // with the Y up direction facing up. The code below
  // handles the different orientations
  if(joystickYDir == RIGHT){
    int tmpX = joystickLeftRightVal;
    joystickLeftRightVal = joystickUpDownVal;
    joystickUpDownVal = MAX_ANALOG_VAL - tmpX;
  }else if(joystickYDir == DOWN){
    joystickUpDownVal = MAX_ANALOG_VAL - joystickUpDownVal;
    joystickLeftRightVal = MAX_ANALOG_VAL - joystickLeftRightVal;
  }else if(joystickYDir == LEFT){
    int tmpX = joystickLeftRightVal;
    joystickLeftRightVal = MAX_ANALOG_VAL - joystickUpDownVal;
    joystickUpDownVal = tmpX;
  }

  Serial.print("joystickLeftRightVal: ");
  Serial.print(joystickLeftRightVal);
  Serial.print(" joystickUpDownVal: ");
  Serial.println(joystickUpDownVal);

  int yDistFromCenter = joystickUpDownVal - JOYSTICK_CENTER_VALUE;
  int xDistFromCenter = joystickLeftRightVal - JOYSTICK_CENTER_VALUE;
  
  Serial.print("xDistFromCenter: ");
  Serial.print(xDistFromCenter);    
  Serial.print(" yDistFromCenter: ");
  Serial.println(yDistFromCenter);

  int yMouse = 0, xMouse = 0;
  if(abs(yDistFromCenter) > JOYSTICK_MOVEMENT_THRESHOLD){
    yMouse = map(joystickUpDownVal, 0, MAX_ANALOG_VAL, MAX_MOUSE_MOVE_VAL, -MAX_MOUSE_MOVE_VAL);
  }
  
  if(abs(xDistFromCenter) > JOYSTICK_MOVEMENT_THRESHOLD){
    xMouse = map(joystickLeftRightVal, 0, MAX_ANALOG_VAL, -MAX_MOUSE_MOVE_VAL, MAX_MOUSE_MOVE_VAL);
  }

  if(isMouseActive){
    Mouse.move(xMouse, yMouse, 0);
  }        

  /** HANDLE BUTTON INPUT AS KEYBOARD **/
  int numButtonsPressed = 0;
  
  // List of non-alphanumerica keys:
  //  - https://www.arduino.cc/en/Reference/KeyboardModifiers
  if(buttonUpVal == LOW){
    isUpKeyPressed = true;
    Keyboard.press(KEY_UP_ARROW);
    Serial.print("UP: Pressed\t");
    numButtonsPressed++;
  }else if(isUpKeyPressed == true && buttonUpVal == HIGH){
    Keyboard.release(KEY_UP_ARROW);
    isUpKeyPressed = false;
  }

  if(buttonRightVal == LOW){
    isRightKeyPressed = true;
    Keyboard.press(KEY_RIGHT_ARROW);
    Serial.print("RIGHT: Pressed\t");
    numButtonsPressed++;
  }else if(isRightKeyPressed == true && buttonRightVal == HIGH){
    Keyboard.release(KEY_RIGHT_ARROW);
    isRightKeyPressed = false;
  }

  if(buttonDownVal == LOW){
    isDownKeyPressed = true;
    Keyboard.press(KEY_DOWN_ARROW);
    Serial.print("DOWN: Pressed\t");
    numButtonsPressed++;
  }else if(isDownKeyPressed == true && buttonDownVal == HIGH){
    Keyboard.release(KEY_DOWN_ARROW);
    isDownKeyPressed = false;
  }

  if(buttonLeftVal == LOW){
    isLeftKeyPressed = true;
    Keyboard.press(KEY_LEFT_ARROW);
    Serial.print("LEFT: Pressed");
    numButtonsPressed++;
  }else if(isLeftKeyPressed == true && buttonLeftVal == HIGH){
    Keyboard.release(KEY_LEFT_ARROW);
    isLeftKeyPressed = false;
  }

  if(buttonSpaceBarVal == LOW){
    isSpaceBarPressed = true;
    Keyboard.press(' ');
    Serial.print("SPACE BAR: Pressed\t");
    isSpaceBarPressed = true;
    numButtonsPressed++;
  }else if(isSpaceBarPressed == true && buttonSpaceBarVal == HIGH){
    Keyboard.release(' ');
    isSpaceBarPressed = false;
  }
  
  delay(50);

  if(numButtonsPressed > 0){
    Serial.println();
  }
  prevButtonMouseToggleVal = buttonMouseToggleVal;
}
