
/**
 * Basic button as a space bar example. Press button, sends a SPACE BAR
 * via Arduino keyboard emulation.
 *
 * Tutorials here: https://makeabilitylab.github.io/physcomp
 *   
 * By Jon E. Froehlich
 * @jonfroehlich
 * http://makeabilitylab.io
 *
 */

 #include <Keyboard.h> // https://www.arduino.cc/reference/en/language/functions/usb/keyboard/

                                                                
const int BUTTON_INPUT_PIN = 2;
bool _btnPressed = false;

void setup() {
  Serial.begin(9600); // for debugging
  Keyboard.begin();
  pinMode(BUTTON_INPUT_PIN, INPUT_PULLUP);
}

void loop() {
  int btnVal = digitalRead(BUTTON_INPUT_PIN);
  if(btnVal == LOW){ // internal-pull configuration
    _btnPressed = true;
    Keyboard.press(' ');
    Serial.println("Spacebar PRESSED!");
  }else{ // if btnVal is HIGH, its not pressed
    if(_btnPressed){ // if prev pressed, release
      _btnPressed = false;
      Keyboard.release(' ');
      Serial.println("Spacebar RELEASED!");
    }
  }
}
