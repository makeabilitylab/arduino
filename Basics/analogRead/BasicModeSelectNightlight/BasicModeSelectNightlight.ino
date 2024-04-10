/*
 * A simple example to demonstrate using buttons to support
 * different interactive modes.
 *
 * Mode 0: Fades the red LED automatically. The speed is set by
 *         the _fadeStep variable (larger is faster)
 * Mode 1: Sets the red LED brightness based on the
 *         potentiometer position
 *
 * To give feedback to the user when they press the mode select
 * button, we also turn on/off the LED_BUILTIN accordingly
 *
 * Tinkercad version: https://www.tinkercad.com/things/0zPPP84smHP-basic-mode-select-nightlight
 * 
 * By Jon Froehlich
 * @jonfroehlich
 * http://makeabilitylab.io
 * 
 */

enum MODE{
  FADE,
  POTENTIOMETER,
  NUM_MODES
};

const int BUTTON_SELECT_MODE_PIN = 2;
const int LED_PIN = 11;
const int POTENTIOMETER_PIN = A0;
const int MIN_ANALOG_INPUT = 0;
const int MAX_ANALOG_INPUT = 1023;
enum MODE _mode = FADE;

const int MAX_LED_VAL = 255;
const int MIN_LED_VAL = 0;
int _fadeVal = 0;
int _fadeStep = 5;
int _prevButtonVal = HIGH; // pull-up resistor config

void setup()
{
  // Turn on serial for debugging
  Serial.begin(9600); 
  
  // Setup pin modes
  pinMode(BUTTON_SELECT_MODE_PIN, INPUT_PULLUP);
  pinMode(POTENTIOMETER_PIN, INPUT);
  pinMode(LED_PIN, OUTPUT);
  printMode();
}

void loop()
{
  // Check mode select button state and increment
  // mode accordingly
  int buttonVal = digitalRead(BUTTON_SELECT_MODE_PIN);
  
  // Turn on built-in LED when button pressed 
  // to give feedback to user
  digitalWrite(LED_BUILTIN, !buttonVal);
  
  // Button is LOW when pressed
  if(buttonVal == LOW && buttonVal != _prevButtonVal){ 
    
    // Increment the modes
  	_mode = (MODE)(_mode + 1);
    if(_mode >= NUM_MODES){
      _mode = FADE; 
    }
    printMode();
    
    delay(50); // simple debounce
  }
  
  // Now set the LED according to the mode
  if(_mode == FADE){ // Fade mode
  	analogWrite(LED_PIN, _fadeVal);	
    _fadeVal += _fadeStep;
    if(_fadeVal > MAX_LED_VAL){
      _fadeVal = MAX_LED_VAL;
      _fadeStep *= -1; // reverse fade
    }else if(_fadeVal < MIN_LED_VAL){
      _fadeVal = 0;
      _fadeStep *= -1; // reverse fade
    }
  }else if(_mode == POTENTIOMETER){ // Potentiometer mode
    int potVal = analogRead(POTENTIOMETER_PIN);
    int ledVal = map(potVal, MIN_ANALOG_INPUT, MAX_ANALOG_INPUT, MAX_LED_VAL, MIN_LED_VAL);
    analogWrite(LED_PIN, ledVal);
  }
  
  _prevButtonVal = buttonVal;
  delay(10);
}

void printMode(){
  Serial.print("Now in mode: ");
  //Serial.print(_mode);
  switch(_mode){
  	case FADE:
      Serial.print("FADE automatically fades the LED with step size: ");
      Serial.println(_fadeStep);
      break;
    case POTENTIOMETER:
      Serial.print("POTENTIOMETER sets the LED based on the pot value: ");
      Serial.println(analogRead(POTENTIOMETER_PIN));
      break;
    default:
      Serial.println("In an unknown mode, we should not be here! :)");
      break;
  }
}