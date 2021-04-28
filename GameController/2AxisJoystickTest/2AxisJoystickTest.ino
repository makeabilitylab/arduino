const int JOYSTICK_UPDOWN_PIN = A1;
const int JOYSTICK_LEFTRIGHT_PIN = A0;

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

void setup() {

  // Turn on serial for debugging
  Serial.begin(9600); 
}

void loop() {

  int joystickUpDownVal = analogRead(JOYSTICK_UPDOWN_PIN);
  int joystickLeftRightVal = analogRead(JOYSTICK_LEFTRIGHT_PIN);

  Serial.print("joystickLeftRightVal: ");
  Serial.print(joystickLeftRightVal);
  Serial.print(" joystickUpDownVal: ");
  Serial.println(joystickUpDownVal);

  // If hooked up on the breadboard, we often have to 
  // install the joystick in a different orientation than
  // with the Y up direction facing up. The code below
  // handles the different orientations
  if(joystickYDir == RIGHT){
    int tmpX = joystickLeftRightVal;
    joystickLeftRightVal = MAX_ANALOG_VAL - joystickUpDownVal;
    joystickUpDownVal = tmpX;
  }else if(joystickYDir == DOWN){
    joystickUpDownVal = MAX_ANALOG_VAL - joystickUpDownVal;
    joystickLeftRightVal = MAX_ANALOG_VAL - joystickLeftRightVal;
  }else if(joystickYDir == LEFT){
    int tmpX = joystickLeftRightVal;
    joystickLeftRightVal = joystickUpDownVal;
    joystickUpDownVal = MAX_ANALOG_VAL - tmpX;
  }

  Serial.print("joystickYDir: ");
  Serial.print(joystickYDir);
  Serial.print("joystickLeftRightVal: ");
  Serial.print(joystickLeftRightVal);
  Serial.print(" joystickUpDownVal: ");
  Serial.println(joystickUpDownVal);
  Serial.println();

  int yDistFromCenter = joystickUpDownVal - JOYSTICK_CENTER_VALUE;
  int xDistFromCenter = joystickLeftRightVal - JOYSTICK_CENTER_VALUE;
  
//  Serial.print("xDistFromCenter: ");
//  Serial.print(xDistFromCenter);    
//  Serial.print(" yDistFromCenter: ");
//  Serial.println(yDistFromCenter);   
  
  delay(50);
}
