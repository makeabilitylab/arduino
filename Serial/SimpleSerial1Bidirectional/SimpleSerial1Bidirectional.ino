/**
 * A simple Serial1 bidirectional demo. Reads a digital input and
 * writes (as a byte) either 0 or 1 to Serial1 accordingly.
 * Similarly, listens on Serial1 for either 0 or 1 and lights
 * up an LED accordingly.
 *
 * Partially remixed from:
 * https://docs.arduino.cc/learn/communication/uart/#serial-usb-examples
 * 
 * By Jon E. Froehlich
 * @jonfroehlich
 * http://makeabilitylab.io
 * 
 */

const int DELAY_MS = 5;
const int LED_OUTPUT_PIN = 3;
const int BUTTON_INPUT_PIN = A0;

byte _lastByteSent = 0;

void setup() {
  Serial.begin(9600);    // Initialize the Serial monitor for debugging
  Serial1.begin(9600);   // Initialize Serial1 for receiving data from RX pin
  pinMode(LED_OUTPUT_PIN, OUTPUT);
  pinMode(BUTTON_INPUT_PIN, INPUT_PULLUP);
}

void loop() {

  // In pull-up configuration, so let's flip digitalRead so that
  // HIGH is still button down and LOW is button up.
  byte buttonVal = !digitalRead(BUTTON_INPUT_PIN);

  if(_lastByteSent != buttonVal){
    // Send the buttonVal: 0 for not pressed; 1 for pressed
    Serial1.write(buttonVal);

    // For debugging
    Serial.print("Sent over Serial1: ");
    Serial.println(buttonVal);

    // Just give some debug feedback about sending data
    digitalWrite(LED_BUILTIN, HIGH);
    delay(200);
  }else{
    digitalWrite(LED_BUILTIN, LOW);
  }
  _lastByteSent = buttonVal; // Save last byte sent

  // Check to see if there is any incoming serial data. We are transmitting
  // in binary encoding (rather than text). 
  // See: https://makeabilitylab.github.io/physcomp/communication/serial-intro.html#why-use-binary-vs-ascii
  while (Serial1.available() > 0) {
    byte receivedData = Serial1.read();   // read one byte from serial buffer
    
    if (receivedData == 1) {
      digitalWrite(LED_OUTPUT_PIN, HIGH); // switch LED On
    }
    else if (receivedData == 0) {
      digitalWrite(LED_OUTPUT_PIN, LOW);  // switch LED Off
    }

    Serial.print("Received over Serial1: ");
    Serial.println(receivedData);
  }

  delay(DELAY_MS);
}