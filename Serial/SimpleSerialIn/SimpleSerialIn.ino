/**
 * Receives a string off of the serial port and writes this value
 * out to the OUTPUT_PIN.
 * 
 * By Jon E. Froehlich
 * @jonfroehlich
 * http://makeabilitylab.io
 * 
 */

const int DELAY_MS = 5;
const int OUTPUT_PIN = LED_BUILTIN;

void setup() {
  Serial.begin(9600);
  pinMode(OUTPUT_PIN, OUTPUT);
}

void loop() {
  // Check to see if there is any incoming serial data
  if(Serial.available() > 0){
    // If we're here, then serial data has been received
    // Read data off the serial port until we get to the endline delimiter ('\n')
    // Store all of this data into a string
    String rcvdSerialData = Serial.readStringUntil('\n'); 

    // Convert string data into an integer
    int ledValue = rcvdSerialData.toInt();

    // Ensure value is between 0 and 255 (our maximum output values)
    ledValue = constrain(ledValue, 0, 255);
    analogWrite(OUTPUT_PIN, ledValue);
    
    // Just for debugging, echo the data back on serial
    Serial.print("Arduino received: '");
    Serial.print(rcvdSerialData);
    Serial.println("'");
  }

  delay(DELAY_MS);
}
