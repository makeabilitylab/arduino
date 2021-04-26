/**
 * Demonstrates how to use the Sharp infrared distance sensors.
 * 
 * This code works with the Sharp GP2Y0A21YK and Sharp GP2Y0A02YK0F infrared distance sensors 
 * 
 * - GP2Y0A21YK: ~$14: https://www.sparkfun.com/products/242 or https://www.pololu.com/product/136
 * - GP2Y0A02YK0F ~$15: https://www.sparkfun.com/products/8958
 * 
 * The GP2Y0A21YK operates from 3.1V at 10cm to 0.4V at 80cm while the GP2Y0A02YK0F is longer range: 
 * 2.8V at 15cm to 0.4V at 150cm. Both require supply voltages of ~5V.
 * 
 * See:
 * https://www.makerguides.com/sharp-gp2y0a21yk0f-ir-distance-sensor-arduino-tutorial/
 *  
 * You can also use the SharpIR Arduino library by Guillaume Rico and Thibaut Mauon:
 * https://github.com/guillaume-rico/SharpIR
 * 
 * Or another one by jeoendoggen
 * https://github.com/jeroendoggen/Arduino-GP2Y0A21YK-library
 * 
 * By Jon E. Froehlich
 * @jonfroehlich
 * http://makeabilitylab.io
 * 
 */

// The Arduino Uno ADC is 10 bits (thus, 0 - 1023 values)
#define MAX_ANALOG_INPUT_VAL 1023

enum SharpIRModel{
  GP2Y0A21YK,
  GP2Y0A02YK0F
};

SharpIRModel _sharpIrModel = GP2Y0A21YK;

void setup() {
  
}

void loop() {

  int sharpInfraredVal = analogRead(A0);
  float analogInVoltage = sharpInfraredVal * (5.0 / MAX_ANALOG_INPUT_VAL);
  
  float distanceCM = 0;
  if(_sharpIrModel == GP2Y0A21YK){
    // Voltage-to-distance formula from https://www.makerguides.com/sharp-gp2y0a21yk0f-ir-distance-sensor-arduino-tutorial/
    distanceCM = 29.988 * pow(analogInVoltage, -1.173);
  }else if(_sharpIrModel == GP2Y0A02YK0F){
    // Formula from https://diyprojects.io/proximity-sensor-a02yk0-test-calibration-sharp-gp2y0a02yk0f-asian-clone
    distanceCM = 138773.464825 / 10 * pow(sharpInfraredVal,-1.0233470);
  }
  
  Serial.print("Distance (cm): ");
  Serial.println(distanceCM);
  delay(50);
}
