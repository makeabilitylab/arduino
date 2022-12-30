/*
 This example reads audio data from an Invensense's ICS43432 I2S microphone
 breakout board, and prints out the samples to the Serial console. The
 Serial Plotter built into the Arduino IDE can be used to plot the audio
 data (Tools -> Serial Plotter)

 Circuit:
 * Arduino/Genuino Zero, MKRZero or MKR1000 board
 * ICS43432:
   * GND connected GND
   * 3.3V connected 3.3V (Zero) or VCC (MKR1000, MKRZero)
   * WS connected to pin 0 (Zero) or pin 3 (MKR1000, MKRZero)
   * CLK connected to pin 1 (Zero) or pin 2 (MKR1000, MKRZero)
   * SD connected to pin 9 (Zero) or pin A6 (MKR1000, MKRZero)

 created 17 November 2016
 by Sandeep Mistry
 From: https://learn.adafruit.com/adafruit-i2s-mems-microphone-breakout/arduino-wiring-and-test
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

#define SAMPLES 128 // make it a power of two for best DMA performance

void loop() {
  // read a bunch of samples:
  int samples[SAMPLES];

  for (int i=0; i<SAMPLES; i++) {
    int sample = 0; 
    while ((sample == 0) || (sample == -1) ) {
      sample = I2S.read();
    }
    // convert to 18 bit signed
    sample >>= 14; 
    samples[i] = sample;
  }

  // ok we have the samples, get the mean (avg)
  float meanval = 0;
  for (int i=0; i<SAMPLES; i++) {
    meanval += samples[i];
  }
  meanval /= SAMPLES;
  //Serial.print("# average: " ); Serial.println(meanval);

  // subtract it from all sapmles to get a 'normalized' output
  for (int i=0; i<SAMPLES; i++) {
    samples[i] -= meanval;
    //Serial.println(samples[i]);
  }

  // find the 'peak to peak' max
  float maxsample, minsample;
  minsample = 100000;
  maxsample = -100000;
  for (int i=0; i<SAMPLES; i++) {
    minsample = min(minsample, samples[i]);
    maxsample = max(maxsample, samples[i]);
  }
  Serial.println(maxsample - minsample);
}