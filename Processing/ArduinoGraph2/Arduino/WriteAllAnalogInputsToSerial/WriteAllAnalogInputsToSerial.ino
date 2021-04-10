/*
 * Sends all analog inputs over the serial port in a comma separated list
 *   
 * By Jon Froehlich
 * http://makeabilitylab.io
 * 
 */

const int TOTAL_ANALOG_INPUTS = 6;
const int MAX_ANALOG_VALUE = 1023;
const int DELAY_INTERVAL = 50;

void setup() {
  Serial.begin(9600); 
}

void loop() {
  // Go through all of the analog pins and print them to serial in a comma separated list
  for(int analogInPin = 0; analogInPin < TOTAL_ANALOG_INPUTS; analogInPin++){
    int rawVal = analogRead(analogInPin);
    float valNormalized = rawVal / (float)MAX_ANALOG_VALUE;
    Serial.print(valNormalized);
    if(analogInPin + 1 < TOTAL_ANALOG_INPUTS){
      Serial.print(",");
    }
  }
  Serial.println();
  delay(DELAY_INTERVAL);
}
