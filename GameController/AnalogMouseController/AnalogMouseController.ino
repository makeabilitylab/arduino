
/* Converts analog input on A0 and A1 to control the x and y movements of the mouse
 * The mouse starts in off mode. You can enable it by pressing a toggle button hooked 
 * up to pin 2 (with a pull-up configuration).
 * 
 * Run this sketch on your Arduino and then play around with the p5js drawing sketch:
 * https://editor.p5js.org/jonfroehlich/sketches/MSGdVYUle
 * 
 * This sketch will work with any 32u4- or SAMD-based boards like the Arduino
 * Leondardo, Esplora, Zero, Due, which can appear as a native mouse and/or keyboard
 * when connected to the computer via USB.
 *
 * TODO: 
 *  - Add in support for mouse wheel
 * 
 * By Jon Froehlich
 * @jonfroehlich
 * http://makeabilitylab.io
 */
#include <Mouse.h> // https://www.arduino.cc/reference/en/language/functions/usb/mouse/

// Analog in pins
const int ANALOG_X_PIN = A0;
const int ANALOG_Y_PIN = A1;
const int MAX_ANALOG_VAL = 1023;
const int ANALOG_CENTER_VALUE = int(MAX_ANALOG_VAL / 2);
const int JOYSTICK_MOVEMENT_THRESHOLD = 10;

// Sets the overall mouse sensitivity based on analog values 
// a higher value will move the mouse more with
const int MAX_MOUSE_MOVE_VAL = 30; 

// Digital I/O pins
const int BUTTON_MOUSE_TOGGLE_PIN = 2;
const int MOUSE_ON_LED_PIN = 13; 

const boolean isDebugModeOn = false;

boolean isMouseActive = false;
int prevMouseToggleVal = HIGH;

void setup() {
  pinMode(BUTTON_MOUSE_TOGGLE_PIN, INPUT_PULLUP);
  pinMode(MOUSE_ON_LED_PIN, OUTPUT);
  
  // Turn on serial for debugging
  Serial.begin(9600);

  // Start with the mouse off
  activateMouse(false); 
}

void activateMouse(boolean turnMouseOn){
  if(turnMouseOn){
    Serial.println("*** Activating mouse! ***"); 
    digitalWrite(MOUSE_ON_LED_PIN, HIGH);
    Mouse.begin();
  }else{
    Serial.println("*** Deactivating mouse! ***");
    digitalWrite(MOUSE_ON_LED_PIN, LOW);
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

  // Move mouse based on analog input values on analog input
  if(isMouseActive){
    int analogX = analogRead(ANALOG_X_PIN);
    int analogY = analogRead(ANALOG_Y_PIN);

    // There are many ways that you may want to convert the incoming
    // analog values to mouse movement. This is just one way
    int yDistFromCenter = analogX - ANALOG_CENTER_VALUE;
    int xDistFromCenter = analogY - ANALOG_CENTER_VALUE;
    int yMouse = 0, xMouse = 0;
      
    if(abs(xDistFromCenter) > JOYSTICK_MOVEMENT_THRESHOLD){
      xMouse = map(analogX, 0, MAX_ANALOG_VAL, -MAX_MOUSE_MOVE_VAL, MAX_MOUSE_MOVE_VAL);
    }

    if(abs(yDistFromCenter) > JOYSTICK_MOVEMENT_THRESHOLD){
      yMouse = map(analogY, 0, MAX_ANALOG_VAL, MAX_MOUSE_MOVE_VAL, -MAX_MOUSE_MOVE_VAL);
    }

    Mouse.move(xMouse, yMouse, 0);

    if(isDebugModeOn){
      // print values to serial if we are in debug mode
      Serial.print("xMouse");
      Serial.print(xMouse);
      Serial.print(", yMouse");
      Serial.println(yMouse);
    }
  }
  
  delay(50);
  prevMouseToggleVal = mouseToggleVal;
}
