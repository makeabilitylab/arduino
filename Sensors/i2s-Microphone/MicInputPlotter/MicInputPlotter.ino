/*
 This example is from https://learn.adafruit.com/adafruit-i2s-mems-microphone-breakout/arduino-wiring-and-test
 However, after a short while, the I2S.read() would always return 0. Others have had this problem too
 See long thread here: https://forums.adafruit.com/viewtopic.php?t=115089&start=15
 
 --- Original comments below ----

 This example reads audio data from an I2S microphone
 breakout board, and prints out the samples to the Serial console. The
 Serial Plotter built into the Arduino IDE can be used to plot the audio
 data (Tools -> Serial Plotter)

 Circuit:
 * Arduino/Genuino Zero, MKRZero or MKR1000 board
   * GND connected GND
   * 3.3V connected 3.3V (Zero) or VCC (MKR1000, MKRZero)
   * WS connected to pin 0 (Zero) or pin 3 (MKR1000, MKRZero)
   * CLK connected to pin 1 (Zero) or pin 2 (MKR1000, MKRZero)
   * SD connected to pin 9 (Zero) or pin A6 (MKR1000, MKRZero)

 created 17 November 2016
 by Sandeep Mistry
 https://learn.adafruit.com/adafruit-i2s-mems-microphone-breakout/arduino-wiring-and-test
 */

#include <I2S.h>

void setup() {
  // Open serial communications and wait for port to open:
  // A baud rate of 115200 is used instead of 9600 for a faster data rate
  // on non-native USB ports
  Serial.begin(115200);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  // start I2S at 16 kHz with 32-bits per sample
  if (!I2S.begin(I2S_PHILIPS_MODE, 16000, 32)) {
    Serial.println("Failed to initialize I2S!");
    while (1); // do nothing
  }
}

void loop() {
  // read a sample
  int sample = I2S.read();

  // if ((sample == 0) || (sample == -1) ) {
  //   return;
  // }
  // convert to 18 bit signed
  sample >>= 14; 

  // if it's non-zero print value to serial
  Serial.println(sample);
}