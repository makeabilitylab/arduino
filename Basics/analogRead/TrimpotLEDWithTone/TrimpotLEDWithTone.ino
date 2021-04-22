/*
 * This example reads in a potentiometer value (from A0) and sets the brightness 
 * of an LED (hooked up to Pin 3).
 * 
 * The analogWrite() function uses PWM, so if you want to change the pin you're
 * using, be sure to use another PWM capable pin. On the Arduino Uno, the PWM pins
 * are identified with a "~" sign, like ~3, ~5, ~6, ~9, ~10 and ~11. For other Arduino
 * boards, please consult the pin layout data sheet.
 *
 * By Jon E. Froehlich
 * @jonfroehlich
 * http://makeabilitylab.io
 * 
 * For a walkthrough and circuit diagram, see:
 * https://makeabilitylab.github.io/physcomp/arduino/potentiometers.html
 * 
 */

// The Arduino Uno ADC is 10 bits (thus, 0 - 1023 values)
#define MAX_ANALOG_INPUT_VAL 1023

const int LED_OUTPUT_PIN = 13;
const int POT_INPUT_PIN = A0;
const int OUTPUT_PIEZO_PIN = 4;

void setup() {
  pinMode(LED_OUTPUT_PIN, OUTPUT);
  Serial.begin(9600);
}

void loop() {

  // read the potentiometer value
  int potVal = analogRead(POT_INPUT_PIN);

  // Convert potentiometer value to a frequency to play
  // https://www.arduino.cc/reference/en/language/functions/advanced-io/tone/
  // The lowest possible frequency is 31
  int freq = map(potVal, 0, 1023, 31, 1500); // change min/max freq here

  // only play sound if above 0
  if(potVal > 0){
    tone(OUTPUT_PIEZO_PIN, freq);
  }else{
    noTone(OUTPUT_PIEZO_PIN);
  }

  // the analogRead on the Arduino Uno goes from 0 to 1023. We need to remap
  // this value to the smaller range (0-255) since the analogWrite function can 
  // only write out 0-255 (a byte--2^8). The map function provides a linear
  // mapping to do this (however, a better way would likely be some sort of
  // non-linear mapping given that perceived LED brightness is not linear with current,
  // perhaps logarithmic)
  int ledVal = map(potVal, 0, MAX_ANALOG_INPUT_VAL, 0, 255);

  float analogInVoltage = potVal * (5.0 / 1023.0);
  
  // print the raw pot value and the converted led value
  Serial.print("Analog in: ");
  Serial.print(potVal);
  Serial.print(" Input voltage: ");
  Serial.print(analogInVoltage);
  Serial.println("V");
  Serial.print(" LED val: ");
  Serial.print(ledVal);
  Serial.print(" PWM Duty cycle: ");
  Serial.print(ledVal/255.0 * 100, 1);
  Serial.println("%");

  // write out the LED value. This value is translated to voltage by: 
  // voltageVal = max_voltage * val/255 or voltageVal = 3.3V * val/255 in 
  // the case of a 3.3V board
  analogWrite(LED_OUTPUT_PIN, ledVal);

  delay(10);
}
