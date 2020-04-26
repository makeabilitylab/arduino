/*
 * Uses a Hall effect sensors to fade on and off an LED based on the proximity
 * and orientation of the magnet. If the south pole of the magnet is directly
 * perpendicular to the Hall effect sensor's face, the voltage output of the
 * sensor will be 5V. If the north pole is directly perpendicular, the voltage output
 * will be ~0V. If no magnet is present, the voltage output will be Vcc/2 or 5V
 * on the Arduino Uno and Leonardo.
 * 
 * By Jon E. Froehlich
 * @jonfroehlich
 * http://makeabilitylab.io
 * 
 * For a walkthrough and circuit diagram, see:
 * https://makeabilitylab.github.io/physcomp/sensors/hall-effect
 * 
 */

const int LED_OUTPUT_PIN = 3;
const int HALLEFFECT_INPUT_PIN = A0;

void setup() {
  pinMode(LED_OUTPUT_PIN, OUTPUT);
  pinMode(HALLEFFECT_INPUT_PIN, INPUT);
  Serial.begin(9600);
}

void loop() {

  int hallEffectVal = analogRead(HALLEFFECT_INPUT_PIN);

  // Remap the value for output. In my experiments, the minimum input
  // value for when the north part of the magnet was directly in front of 
  // the Hall effect sensor was ~22
  int ledVal = map(hallEffectVal, 22, 1023, 0, 255);

  // Print the raw photocell value and the converted led value (e,g., for Serial 
  // Console and Serial Plotter)
  Serial.print(hallEffectVal);
  Serial.print(",");
  Serial.println(ledVal);

  // Write out the LED value. 
  analogWrite(LED_OUTPUT_PIN, ledVal);

  delay(20); // read at 50Hz
}
