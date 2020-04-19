/*
 * This example reads in a force-sensitive resistor value (from A0 using a voltage-divider 
 * circuit) and uses it to play a tone on a connected piezo buzzer and set the brightness 
 * of an LED. 
 * 
 * By Jon E. Froehlich
 * @jonfroehlich
 * http://makeabilitylab.io
 * 
 * For a walkthrough and circuit diagram, see:
 * https://makeabilitylab.github.io/physcomp/arduino/force-sensitive-resistors.html
 * 
 */

const int OUTPUT_LED_PIN = LED_BUILTIN;
const int OUTPUT_PIEZO_PIN = 2;
const int INPUT_FSR_PIN = A0;
const int DELAY_MS = 20; // how often to read from the sensor input

void setup() {
  pinMode(OUTPUT_LED_PIN, OUTPUT);
  pinMode(OUTPUT_PIEZO_PIN, OUTPUT);
  pinMode(INPUT_FSR_PIN, INPUT);
  Serial.begin(9600);
}

void loop() {

  // Read the force-sensitive resistor value
  int fsrVal = analogRead(INPUT_FSR_PIN);

  // Remap the value for output. 
  int ledVal = map(fsrVal, 0, 1023, 0, 255);
  int freq = map(fsrVal, 0, 1023, 50, 1500); // change min/max freq here

  // only play sound if the user is pressing on the FSR
  if(fsrVal > 0){
    tone(OUTPUT_PIEZO_PIN, freq);
  }else{
    noTone(OUTPUT_PIEZO_PIN);
  }

  // Print the raw sensor value and the converted led value (e,g., for Serial Plotter)
  Serial.print(fsrVal);
  Serial.print(",");
  Serial.println(ledVal);
  Serial.print(",");
  Serial.println(freq);

  // Write out the LED value. 
  analogWrite(OUTPUT_LED_PIN, ledVal);

  delay(DELAY_MS);
}
