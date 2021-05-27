
/**
 *  Read in two analog inputs, translate them to X,Y float positions
 *  between [0, 1] inclusive, and send over Serial. 
 *
 *  By Jon E. Froehlich
 *  @jonfroehlich
 *  http://makeabilitylab.io
 *
 */

const int X_ANALOG_INPUT_PIN = A0;
const int Y_ANALOG_INPUT_PIN = A1;

// On the Arduino Uno/Leonardo, there is a 10-bit ADC so the
// maximum analog value is 1023. On other microcontrollers, like the ESP32,
// there is a 12-bit ADC, so the maximum analog value is 4095
const int MAX_ANALOG_VAL = 1023; 

const long BAUD_RATE = 115200;
void setup() {
  Serial.begin(BAUD_RATE);
}

void loop() {

  // Read the analog values
  int xAnalogVal = analogRead(X_ANALOG_INPUT_PIN);
  int yAnalogVal = analogRead(Y_ANALOG_INPUT_PIN);

  // Calculate normalized x,y location
  float xNormalized = xAnalogVal / (float)MAX_ANALOG_VAL;
  float yNormalized = yAnalogVal / (float)MAX_ANALOG_VAL;

  // Transmit over serial as comma-separated string
  Serial.print(xNormalized, 4);
  Serial.print(", ");
  Serial.println(yNormalized, 4);
  
  delay(10);
}
