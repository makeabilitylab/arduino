/**
 * Receives a string off of the serial port and writes it back
 * over serial for debugging purposes
 * 
 * By Jon E. Froehlich
 * @jonfroehlich
 * http://makeabilitylab.io
 * 
 */

const long BAUD_RATE = 115200;
void setup() {
  Serial.begin(BAUD_RATE);
}

void loop() {
  // Check to see if there is any incoming serial data
  if(Serial.available() > 0){
    // If we're here, then serial data has been received
    // Read data off the serial port until we get to the endline delimiter ('\n')
    // Store all of this data into a string
    String rcvdSerialData = Serial.readStringUntil('\n'); 
    
    // Just for debugging, echo the data back on serial
    Serial.print("Arduino received: '");
    Serial.print(rcvdSerialData);
    Serial.println("'");
  }
}
