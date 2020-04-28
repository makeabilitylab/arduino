
/*
 * Prints out the current battery level once per second. On the Huzzah32, the
 * A13 pin is internally tied to measuring the battery level of the LiPoly battery
 * https://learn.adafruit.com/adafruit-huzzah32-esp32-feather/esp32-faq
 * 
 * Based on example from "Program the Internet of Things with Swift for iOS" 
 * by Ahmed Bakir. See: https://bit.ly/2zFzfMT
 * 
 * By Jon E. Froehlich
 * @jonfroehlich
 * http://makeabilitylab.io
 * 
 * TODO
 */
const int MAX_ANALOG_VAL = 4095;
const float MAX_BATTERY_VOLTAGE = 4.2; // Max LiPoly voltage of a 3.7 battery is 4.2

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
}

void loop() {
  // A13 pin is not exposed on Huzzah32 board because it's tied to
  // measuring voltage level of battery. Note: you must
  // multiply the analogRead value by 2x to get the true battery
  // level. See: 
  // https://learn.adafruit.com/adafruit-huzzah32-esp32-feather/esp32-faq
  int rawValue = analogRead(A13);

  // Reference voltage on ESP32 is 1.1V
  // https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/peripherals/adc.html#adc-calibration
  // See also: https://bit.ly/2zFzfMT
  float voltageLevel = (rawValue / 4095.0) * 2 * 1.1 * 3.3; // calculate voltage level
  float batteryFraction = voltageLevel / MAX_BATTERY_VOLTAGE;
  
  Serial.println((String)"Raw:" + rawValue + " Voltage:" + voltageLevel + "V Percent: " + (batteryFraction * 100) + "%");
  delay(1000);
}
