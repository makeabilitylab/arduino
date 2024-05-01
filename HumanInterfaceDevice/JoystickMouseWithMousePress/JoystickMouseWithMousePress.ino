
/* Converts analog input on A0 and A1 to control the x and y movements of the mouse
 * Also sends a mouse press when BUTTON_MOUSE_PRESS_PIN goes LOW (configured
 * with pulldown resistor)
 *
 * The mouse starts in off mode. You can enable it by pressing a toggle button hooked 
 * up to pin 12 (with a pull-up configuration). When the mouse is turned on,
 * we also set pin 13 HIGH for feedback to the user.

 * Originally written for the Adafruit 2-Axis Joystick (https://www.adafruit.com/product/512)
 * with the select button (press down on joystick for select) but should work with any other analog 
 * inputs tied to A0 and A1 to control the x and y movements of the mouse.
 * 
 * This sketch will work with any 32u4- or SAMD-based boards like the Arduino
 * Leondardo, Esplora, Zero, Due, which can appear as a native mouse and/or keyboard
 * when connected to the computer via USB.
 * 
 * By Jon Froehlich
 * @jonfroehlich
 * http://makeabilitylab.io
 */
#include <Mouse.h> // https://www.arduino.cc/reference/en/language/functions/usb/mouse/

// Digital pins
const int BUTTON_MOUSE_TOGGLE_PIN = 12;
const int MOUSE_STATUS_LED_PIN = 13;
const int BUTTON_MOUSE_PRESS_PIN = 2;

// Analog input joystick pins. Make sure these are set to your pins
const int JOYSTICK_XOUT_PIN = A1;
const int JOYSTICK_YOUT_PIN = A0;

// Set mouse update freq
const int MOUSE_UPDATE_FREQ = 20; // in hz or samples/sec (max is ~1000)
int _sendMouseSampleThresholdInMs = -1; // set automatically based on MOUSE_UPDATE_FREQ
unsigned long _lastSentMouseSampleTimestampInMs = -1;

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
// In addition, if you up the MOUSE_UPDATE_FREQ, mouse will move more
const int MAX_MOUSE_MOVE_VAL = 20; 

boolean isMouseActive = false;
int prevMouseToggleVal = HIGH;

void setup() {
  pinMode(BUTTON_MOUSE_TOGGLE_PIN, INPUT_PULLUP);
  pinMode(BUTTON_MOUSE_PRESS_PIN, INPUT_PULLUP);

 _sendMouseSampleThresholdInMs = (1 / (float)MOUSE_UPDATE_FREQ)*1000;

  // Turn on serial for debugging
  Serial.begin(9600); 
}

void activateMouse(boolean turnMouseOn){
  if(turnMouseOn){
    Serial.println("*** Activating mouse! ***"); 
    digitalWrite(MOUSE_STATUS_LED_PIN, HIGH);

    // Mouse.begin() must be called to start emulating a mouse
    // https://www.arduino.cc/reference/en/language/functions/usb/mouse/mousebegin/
    Mouse.begin();
  }else{
    Serial.println("*** Deactivating mouse! ***");
    digitalWrite(MOUSE_STATUS_LED_PIN, LOW);
    isMouseActive = false;
    Mouse.end();
  }

  isMouseActive = turnMouseOn;
}

void loop() {
  
  // Check mouse toggle button. Activate/deactivate mouse accordingly
  int mouseToggleVal = digitalRead(BUTTON_MOUSE_TOGGLE_PIN);
  if(mouseToggleVal != prevMouseToggleVal){
    if(mouseToggleVal == LOW){ // button pressed
      activateMouse(!isMouseActive);
    }
  }
  prevMouseToggleVal = mouseToggleVal;

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

  // We control how often we send mouse updates via MOUSE_UPDATE_FREQ 
  if(millis() - _lastSentMouseSampleTimestampInMs > _sendMouseSampleThresholdInMs){
    // Mouse.move takes xVal, yVal, and wheel
    // See: https://www.arduino.cc/reference/tr/language/functions/usb/mouse/mousemove/
    Mouse.move(xMouse, yMouse, 0);
    _lastSentMouseSampleTimestampInMs = millis();
    Serial.println((String)"Sent xMouse: " + xMouse + " yMouse: " + yMouse);
  }

  // Read mouse button
  int mousePressBtn = digitalRead(BUTTON_MOUSE_PRESS_PIN);
  if (!Mouse.isPressed() && mousePressBtn == LOW) { // pull-up input
    Mouse.press();
  }else if(Mouse.isPressed() && mousePressBtn == HIGH){
    Mouse.release();
  }

}
