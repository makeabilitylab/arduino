
/**
 * A basic test of the Adafruit illuminated pushbutton:
 * https://www.adafruit.com/product/1439
 * 
 * Wiring:
 * 
 * - The positive (+) pin on button hooked to output pin 3 on Arduino
 * - The negative (-) pin on button hooked to GND
 * - One side of the button switch hooked to GND
 * - The other side of the button switch hooked to pin 2 on Arduino
 * 
 * By Jon E. Froehlich
 * @jonfroehlich
 * http://makeabilitylab.io
 *
 */

const int ILLUMINATED_BUTTON_INPUT_PIN = 2;
const int ILLUMINATED_BUTTON_OUTPUT_PIN = 3;

void setup() {
  pinMode(ILLUMINATED_BUTTON_INPUT_PIN, INPUT_PULLUP);
  pinMode(ILLUMINATED_BUTTON_OUTPUT_PIN, OUTPUT);
}

void loop() {
  int buttonVal = digitalRead(ILLUMINATED_BUTTON_INPUT_PIN);
  if(buttonVal == LOW){
    digitalWrite(ILLUMINATED_BUTTON_OUTPUT_PIN, HIGH);
  }else{
    digitalWrite(ILLUMINATED_BUTTON_OUTPUT_PIN, LOW);
  }
}
