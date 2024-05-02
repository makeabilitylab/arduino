
/* Converts analog input on A0 and A1 to control the x and y movements of the mouse
 * The mouse starts in off mode. You can enable it by pressing a toggle button hooked 
 * up to pin 12 (with a pull-up configuration). When the mouse is turned on,
 * we also set pin 13 HIGH for feedback to the user. 
 *
 * There is also a button hooked up to pin 8 that simulates a left mouse press/release
 * 
 * Run this sketch on your Arduino and then play around with the p5js drawing sketch:
 * https://editor.p5js.org/jonfroehlich/sketches/MSGdVYUle
 *
 * Or try out: https://jacksonpollock.org/
 * 
 * This sketch will work with any 32u4- or SAMD-based boards like the Arduino
 * Leondardo, Esplora, Zero, Due, which can appear as a native mouse and/or keyboard
 * when connected to the computer via USB.
 *
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
const int MOUSE_MOVEMENT_THRESHOLD = 10; // distance from rest position

// Sets the overall mouse sensitivity based on analog values 
// a higher value will move the mouse more with
const int MAX_MOUSE_MOVE_VAL = 30; 

// Digital I/O pins
const int BUTTON_MOUSE_TOGGLE_PIN = 12;
const int MOUSE_ON_LED_PIN = 13; 
const int BUTTON_MOUSE_CLICK_PIN = 8;

const boolean isDebugModeOn = true;

boolean isMouseActive = false;
int prevMouseToggleVal = HIGH;

void setup() {
  pinMode(BUTTON_MOUSE_TOGGLE_PIN, INPUT_PULLUP);
  pinMode(BUTTON_MOUSE_CLICK_PIN, INPUT_PULLUP);
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

    // Mouse.begin() must be called to start emulating a mouse
    // https://www.arduino.cc/reference/en/language/functions/usb/mouse/mousebegin/
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
  prevMouseToggleVal = mouseToggleVal;

  // Move mouse based on analog input values on analog input
  int analogX = analogRead(ANALOG_X_PIN);
  int analogY = analogRead(ANALOG_Y_PIN);

  if(isDebugModeOn){
    Serial.println((String)"analogX: " + analogX + " analogY: " + analogY);
  }

  // There are many ways that you may want to convert the incoming
  // analog values to mouse movement. This is just one way
  int xDistFromCenter = analogX - ANALOG_CENTER_VALUE;
  int yDistFromCenter = analogY - ANALOG_CENTER_VALUE;
  int yMouse = 0, xMouse = 0;

  // Check the distance from the center position of the analog input  
  // If it's beyond MOUSE_MOVEMENT_THRESHOLD, then calc mouse movement
  if(abs(xDistFromCenter) > MOUSE_MOVEMENT_THRESHOLD){
    xMouse = map(analogX, 0, MAX_ANALOG_VAL, -MAX_MOUSE_MOVE_VAL, MAX_MOUSE_MOVE_VAL);
  }

  if(abs(yDistFromCenter) > MOUSE_MOVEMENT_THRESHOLD){
    yMouse = map(analogY, 0, MAX_ANALOG_VAL, -MAX_MOUSE_MOVE_VAL, MAX_MOUSE_MOVE_VAL);
  }

  // Read mouse button
  int mouseClickBtn = digitalRead(BUTTON_MOUSE_CLICK_PIN);

  if(isDebugModeOn){
    // print values to serial if we are in debug mode
    Serial.print("MouseActive: ");
    isMouseActive ? Serial.print("true") : Serial.print("false");
    
    // Analog input distance from center
    Serial.print(" xDistFromCenter: ");
    Serial.print(xDistFromCenter);
    Serial.print(", yDistFromCenter: ");
    Serial.print(yDistFromCenter);

    // mouse movement
    Serial.print(" xMouse: ");
    Serial.print(xMouse);
    Serial.print(", yMouse: ");
    Serial.println(yMouse);

    // mouse pressed
    Serial.print(" mouseClickBtn: ");
    Serial.print(mouseClickBtn);
    Serial.print(" Mouse.isPressed(): ");
    Serial.print(Mouse.isPressed());
  }
  
  if(isMouseActive){
    // Mouse move is always relative 
    Mouse.move(xMouse, yMouse, 0);

    if (!Mouse.isPressed() && mouseClickBtn == LOW) { // pull-up input
      Mouse.press();
    }else if(Mouse.isPressed() && mouseClickBtn == HIGH){
      Mouse.release();
    }
  }

  delay(50); // Only send mouse updates ~20 times/sec
}
