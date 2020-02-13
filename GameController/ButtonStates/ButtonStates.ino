
/*
 * Prints out the button states to serial monitor
 * 
 * By Jon Froehlich
 * @jonfroehlich
 * http://makeabilitylab.io
 */

const int BUTTON_UP_PIN = 2;
const int BUTTON_RIGHT_PIN = 3;
const int BUTTON_DOWN_PIN = 4;
const int BUTTON_LEFT_PIN = 5;

void setup() {
  pinMode(BUTTON_UP_PIN, INPUT_PULLUP);
  pinMode(BUTTON_RIGHT_PIN, INPUT_PULLUP);
  pinMode(BUTTON_DOWN_PIN, INPUT_PULLUP);
  pinMode(BUTTON_LEFT_PIN, INPUT_PULLUP);

  // Turn on serial for debugging
  Serial.begin(9600); 
}

void loop() {
  int buttonUpVal = digitalRead(BUTTON_UP_PIN);
  int buttonRightVal = digitalRead(BUTTON_RIGHT_PIN);
  int buttonDownVal = digitalRead(BUTTON_DOWN_PIN);
  int buttonLeftVal = digitalRead(BUTTON_LEFT_PIN);

  printButtonState("Up", buttonUpVal);
  printButtonState("Right", buttonRightVal);
  printButtonState("Down", buttonDownVal);
  printButtonState("Left", buttonLeftVal);
  Serial.println();
  
  delay(400);
}

void printButtonState(const char *str, int buttonVal){
  Serial.print(str);
  if(buttonVal == LOW){
    Serial.print(": Pressed");
  }else{
    Serial.print(": Not Pressed");
  }
  Serial.print("\t");
}
