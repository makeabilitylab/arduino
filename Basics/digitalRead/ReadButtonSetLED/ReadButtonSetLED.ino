/*
 * This example reads in a button input on digital I/O Pin 2 and turns on/off
 * the embedded LED on the Arduino (defaults to LED_BUILTIN) accordingly. By
 * default, the code assumes a pull-down resistor configuration (when button
 * is not pressed, Pin 2 is LOW; when button is pressed Pin 2 is HIGH). If you've 
 * wired a pull-up resistor configuration, set IS_RESISTOR_CONFIGURATION_PULLDOWN
 * to false. With a pull-up resistor, when the button is not pressed, Pin 2 is HIGH
 * and when the button is pressed Pin 2 is LOW.
 * 
 * See: 
 * 
 * By Jon Froehlich
 * @jonfroehlich
 * http://makeabilitylab.io
 * 
 */

// change this if your button is wired on a different digital I/O pin
const int BUTTON_INPUT_PIN = 2;

// change this if you want to use an external LED on a different pin
const int LED_OUTPUT_PIN = LED_BUILTIN; 

// change this to false if you wired up a pull-up configuration
const boolean IS_RESISTOR_CONFIGURATION_PULLDOWN = false; 

void setup() {
  pinMode(BUTTON_INPUT_PIN, INPUT_PULLUP);
  pinMode(LED_OUTPUT_PIN, OUTPUT);
  Serial.begin(9600); // used for debugging
}

void loop() {

  // read the button value. It will be HIGH when pressed and
  // LOW when not pressed
  int buttonVal = digitalRead(BUTTON_INPUT_PIN);
  int ledVal = buttonVal;

  // Check if we're in a pull-down or pull-up resistor configuration
  if(IS_RESISTOR_CONFIGURATION_PULLDOWN == false){
    // If we're here, we're in a pull-up resistor configuration
    // so the ledVal is the opposite of buttonVal (since buttonVal is HIGH
    // when the button is *not* pressed)
    ledVal = !buttonVal;
  }
  
  digitalWrite(LED_OUTPUT_PIN, ledVal);

  // Write out the button value and the LED value to Serial for debugging
  Serial.print(buttonVal); 
  Serial.print(", ");
  Serial.println(ledVal);
  
  // Check for new input every 50ms (~20 times a sec)
  delay(50);
}
