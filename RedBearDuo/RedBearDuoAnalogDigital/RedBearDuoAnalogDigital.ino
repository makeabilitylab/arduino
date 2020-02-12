/*
 * Reads in an analog value on A0 and D0 and writes to serial. Used to show the
 * difference between analog and digital reads. For example, try to hook up
 * a potentiometer to A0 and then open Serial Monitor or Serial Plotter to view values
 * 
 * By Jon Froehlich for CSE590
 * http://makeabilitylab.io
 * 
 */

/* 
 * IMPORTANT: When working with the RedBear Duo, you must have this line of
 * code at the top of your program. The default state is SYSTEM_MODE(AUTOMATIC);
 * however, this puts the RedBear Duo in a special cloud-based mode that we 
 * are not using. For our purposes, set SYSTEM_MODE(SEMI_AUTOMATIC) or
 * SYSTEM_MODE(MANUAL). See https://docs.particle.io/reference/firmware/photon/#system-modes
 */
SYSTEM_MODE(MANUAL); 

// The RedBear Duo appears to have a 12-bit ADC so input values can range from
// 0 to 4096 (2^12)
#define MAX_ANALOG_INPUT_VAL 4096

const int ANALOG_INPUT = A0;
const int DIGITAL_INPUT = D0;

void setup() {
  pinMode(DIGITAL_INPUT, INPUT);
  pinMode(ANALOG_INPUT, INPUT);

  Serial.begin(9600);
}

void loop() {
  
  int analogVal = analogRead(ANALOG_INPUT);
  int digitalVal = digitalRead(DIGITAL_INPUT);

  // will scale 0 to 4096 to 0V - 3.3V
  // You could also write analogValAsVoltage = map(analogVal, 0, MAX_ANALOG_INPUT_VAL, 0, 3.3);
  float analogValAsVoltage = (analogVal / (float)MAX_ANALOG_INPUT_VAL) * 3.3;

  // will be either 0V (LOW) or 3.3V (HIGH)
  float digitalValAsVoltage = digitalVal * 3.3; 
  Serial.print(analogValAsVoltage);
  Serial.print(",");
  Serial.print(digitalValAsVoltage);
  Serial.println();

  delay(50);
}


