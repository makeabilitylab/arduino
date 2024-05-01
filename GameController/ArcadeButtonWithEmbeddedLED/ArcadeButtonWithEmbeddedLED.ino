
/**
 * Basic arcade button example. The arcade button fades in/out while
 * waiting to be pressed.
 *
 * Should work with any button with an embedded LED, such as:
 * - Arcade Button with LED - 30mm: https://www.adafruit.com/product/3489
 * - Triangle Arcade buttons: https://www.adafruit.com/product/4190
 * - 16mm illuminated buttons: https://www.adafruit.com/product/1439
 *
 * Tutorials here: https://makeabilitylab.github.io/physcomp
 *   
 * By Jon E. Froehlich
 * @jonfroehlich
 * http://makeabilitylab.io
 *
 */

// Two values change the LED fade speed:
// - The time between fade steps (in milliseconds below)
// - The fade step itself
const int TIME_BETWEEN_FADE_STEP_IN_MS = 8; // change this to change fade speed

const int ARCADE_BUTTON_INPUT_PIN = 2;
const int ARCADE_BUTTON_LED_PIN = 3; // needs to be a PWM pin
const int MAX_FADE_VAL = 100; // should be less than max PWM output
int _curFadeVal = 0;
int _fadeStep = 1; // change this to quicken fade (but will be less smooth so limit to max ~20)
unsigned long _lastFadeTimestampMs = 0;

void setup() {
  Serial.begin(9600); // for debugging
  pinMode(ARCADE_BUTTON_INPUT_PIN, INPUT_PULLUP);
  pinMode(ARCADE_BUTTON_LED_PIN, OUTPUT);
  analogWrite(ARCADE_BUTTON_LED_PIN, _curFadeVal);
  _lastFadeTimestampMs = millis();
}

void loop() {
  
  int btnVal = digitalRead(ARCADE_BUTTON_INPUT_PIN);
  if(btnVal == LOW){ // internal-pull configuration
    digitalWrite(ARCADE_BUTTON_LED_PIN, HIGH); // turn on embedded LED
    Serial.println("BUTTON PRESSED!");

    // reset fade val to lowest so LED turns off when button released
    _curFadeVal = 0;
    if(_fadeStep < 0){
      _fadeStep *= -1;
    }
  }else{
      // If we are here, the button is not pressed, so continue fade loop
      // Check to see if enough time has passed to fade since last fade step
      if(millis() - _lastFadeTimestampMs > TIME_BETWEEN_FADE_STEP_IN_MS){
        analogWrite(ARCADE_BUTTON_LED_PIN, _curFadeVal);
        _lastFadeTimestampMs = millis();

        _curFadeVal += _fadeStep;
        _curFadeVal = constrain(_curFadeVal, 0, MAX_FADE_VAL);
        if(_curFadeVal <= 0 || _curFadeVal >= MAX_FADE_VAL){
          _fadeStep *= -1;
        }     
      }
  }
}
