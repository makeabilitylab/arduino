/*
 * This example reads in a resistive sensor value from A0 using a voltage-divider circuit and 
 * proportionally sets the brightness of an LED (hooked up to D0) and plays a frequency tone
 * on D1.
 * 
 * By Jon Froehlich for CSE590
 * http://makeabilitylab.io
 */

/* 
 * IMPORTANT: When working with the RedBear Duo, you must have this line of
 * code at the top of your program. The default state is SYSTEM_MODE(AUTOMATIC);
 * however, this puts the RedBear Duo in a special cloud-based mode that we 
 * are not using. For our purposes, set SYSTEM_MODE(SEMI_AUTOMATIC) or
 * SYSTEM_MODE(MANUAL). See https://docs.particle.io/reference/firmware/photon/#system-modes
 */
SYSTEM_MODE(MANUAL); 

const int LED_OUTPUT_PIN = D0;
const int SOUND_OUTPUT_PIN = D1;
const int RESISTIVE_SENSOR_INPUT_PIN = A0;

// Change these depending on the resistive sensor that you are using
// and the value of the R2 sensor in your voltage divider
const int MIN_SENSOR_VAL = 1200; 
const int MAX_SENSOR_VAL = 3700; 

void setup() {
  pinMode(LED_OUTPUT_PIN, OUTPUT);
  pinMode(SOUND_OUTPUT_PIN, OUTPUT);
  pinMode(RESISTIVE_SENSOR_INPUT_PIN, INPUT);
  Serial.begin(9600);
}

void loop() {

  // Read the sensor value
  int sensorVal = analogRead(RESISTIVE_SENSOR_INPUT_PIN);

  // Remap the value for output. 
  int ledVal = map(sensorVal, MIN_SENSOR_VAL, MAX_SENSOR_VAL, 0, 255);

  // The map function does not constrain output outside of the provided range
  // so, we need to make sure that things are within range for the led
  ledVal = constrain(ledVal, 0, 255);

  // Print the raw force sensor value and the converted led value so we can
  // see them in Serial Monitor and Serial Plotter
  Serial.print(sensorVal);
  Serial.print(",");
  Serial.println(ledVal);

  // Write out the LED value. 
  analogWrite(LED_OUTPUT_PIN, ledVal);

  // Write out freq to sound
  // The lowest tone we can generate is 31Hz, so if sensor val is less
  // than that, let's just turn off the piezo buzzer
  if (sensorVal >= 31){
    tone(SOUND_OUTPUT_PIN, sensorVal);
  }else{
    noTone(SOUND_OUTPUT_PIN);
  }

  delay(100);
}




