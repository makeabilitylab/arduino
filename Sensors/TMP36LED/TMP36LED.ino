/*
 * This example reads from a TMP36 sensor and changes the LED brightness accordingly
 * (brighter for higher temperatures)
 * 
 * By Jon Froehlich
 * http://makeabilitylab.io
 * 
 * Based on:
 *  - https://learn.adafruit.com/tmp36-temperature-sensor/using-a-temp-sensor
 *  - https://learn.sparkfun.com/tutorials/sik-experiment-guide-for-arduino---v32/experiment-7-reading-a-temperature-sensor
 * 
 */

const int LED_OUTPUT_PIN = 3;
const int TMP36_INPUT_PIN = A0;

// Set the min and max sensor values. The best way to figure out what to 
// set these values is by experimentation with the sensor using
// Serial Monitor or Serial Plotter. Also, consult a datasheet
const int MIN_SENSOR_VAL = 0; 
const int MAX_SENSOR_VAL = 500;

void setup() {
  pinMode(LED_OUTPUT_PIN, OUTPUT);
  pinMode(TMP36_INPUT_PIN, INPUT);
  Serial.begin(9600);
}

void loop() {

  // Read the variable resistor sensor value
  int tmpSensorVal = analogRead(TMP36_INPUT_PIN);

  // convert to voltage
  float voltage = tmpSensorVal / 1023.0 * 5; // Uno runs on 5V

  float temperatureC = (voltage - 0.5) * 100 ; // formula from adafruit
  float temperatureF = (temperatureC * 9.0 / 5.0) + 32.0;

  Serial.print("RawSensorVal=");
  Serial.print(tmpSensorVal);
  Serial.print(" Voltage=");
  Serial.print(voltage);
  Serial.print(" TempC=");
  Serial.print(temperatureC);
  Serial.print(" TempF=");
  Serial.println(temperatureF);

  // Remap the value for output. 
  int ledVal = map(tmpSensorVal, MIN_SENSOR_VAL, MAX_SENSOR_VAL, 0, 255);

  // The map function does not constrain output outside of the provided range
  // so, we need to make sure that things are within range for the led
  ledVal = constrain(ledVal, 0, 255);

  // Print the raw sensor value and the converted led value (e,g., for Serial Plotter)
  //Serial.println(sensorVal);
  //Serial.print(",");
  //Serial.println(ledVal);

  // Write out the LED value. 
  analogWrite(LED_OUTPUT_PIN, ledVal);

  delay(100);
}
