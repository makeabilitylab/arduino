/**
 * Basic button as keyboard input example. Sends "The joy of making and learning!" via
 * Keyboard.println(). 
 *
 * See:
 * https://www.arduino.cc/reference/en/language/functions/usb/keyboard/keyboardprintln/
 *   
 * By Jon E. Froehlich
 * @jonfroehlich
 * http://makeabilitylab.io
 *
 */

 #include <Keyboard.h> // https://www.arduino.cc/reference/en/language/functions/usb/keyboard/
                                                      
const int BUTTON_INPUT_PIN = 2;
const int LED_STATUS_PIN = LED_BUILTIN; // Turn on LED when we send data
int _prevBtnVal = HIGH; // by default, btn pin is HIGH due to pull-up

void setup() {
  Keyboard.begin();
  pinMode(BUTTON_INPUT_PIN, INPUT_PULLUP); // internal pull config
}

void loop() {
  int btnVal = digitalRead(BUTTON_INPUT_PIN);
  if(_prevBtnVal != btnVal && btnVal == LOW){ // internal-pull configuration
    Keyboard.println("The joy of making and learning! :-)");
    digitalWrite(LED_STATUS_PIN, HIGH);
  }else if(_prevBtnVal != btnVal && btnVal == HIGH){ // transitioned up
    digitalWrite(LED_STATUS_PIN, LOW);
  }
  _prevBtnVal = btnVal;
  delay(50); // small delay, no reason to check faster than 20 Hz
}