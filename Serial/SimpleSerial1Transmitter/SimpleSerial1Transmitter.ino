/**
 * Reads in an analog value off of ANALOG_INPUT_PIN and transmits it
 * via Serial1. Also turns sets brightness of LED on LED_OUTPUT_PIN
 * accordingly.
 * 
 * By Jon E. Froehlich
 * @jonfroehlich
 * http://makeabilitylab.io
 * 
 */

const int DELAY_MS = 5;
const int ANALOG_INPUT_PIN = A0;
const int LED_OUTPUT_PIN = 3;
int _lastOutVal = -1;

void setup() {
  Serial.begin(9600);    // Initialize the Serial monitor for debugging
  Serial1.begin(9600);   // Initialize Serial1 for receiving data from RX pin
  
  digitalWrite(LED_BUILTIN, LOW); // Use LED_BUILTIN to communicate sending status

  pinMode(ANALOG_INPUT_PIN, INPUT); // analog input
  pinMode(LED_OUTPUT_PIN, OUTPUT);  // set LED brightness based on analog in
}

void loop() {
  int analogVal = analogRead(ANALOG_INPUT_PIN);
  int outVal = map(analogVal, 0, 1023, 0, 255);

  // We only send data when the analog input value has changed
  if(_lastOutVal != outVal){

    // Send the data out of the TX pin using Serial
    Serial1.println(outVal);

    // Set the local LED pin value
    analogWrite(LED_OUTPUT_PIN, outVal);

    // Turn on built-in LED for data send feedback. TX light should
    // also turn on.
    digitalWrite(LED_BUILTIN, HIGH); 
    delay(100); // just delay here so we can see LED_BUILTIN light up
  }else{
    digitalWrite(LED_BUILTIN, LOW);
  }

  Serial.print("Sent over Serial1: ");
  Serial.println(outVal);

  delay(DELAY_MS);

  // Save the last output value
  _lastOutVal = outVal;
}