/**
 * This code is by Rob Green posted to the Adafruit forums here:
 *  https://forums.adafruit.com/viewtopic.php?p=624436#p624436
 *
 * It is intended to address the I2S.read() hanging problem also described 
 * in that forum post—and which we also experienced.
 *
 * This code appears to work more reliably than MicInputPlotter, which
 * is from https://learn.adafruit.com/adafruit-i2s-mems-microphone-breakout/arduino-wiring-and-test
 * and also posted in our GitHub here:
 * 
 */ 
#include <I2S.h>

void setup() {

  Serial.begin(115200);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  I2S.onReceive(onI2SReceive);
  
  int sampleRate = 31250; // 48,000,000 / sampleRate must be a multiple of 64
  if (!I2S.begin(I2S_PHILIPS_MODE, sampleRate, 32)) {
    Serial.println("Failed to initialize I2S!");
    while (1); // do nothing
  }

  I2S.read(); // Needed to start the interrupt handler
}

uint8_t buffer[512];
volatile int available = 0;
volatile int read = 0;

void onI2SReceive() {
  // This function will run at a frequency of (sampleRate / 64)
  // At 31.25khz, this is every 1962.5 microseconds so make sure any processing here takes less
  // time than that

  I2S.read(buffer, 512); // Will actually read 256 bytes

  // Do analysis here, but make sure it completes before the next batch samples comes in
  available = 1;
}

void loop() {
  if (available) {
    int *values = (int *) buffer;
    for (int i = 0; i < 32; i++) {
      // If the SEL pin is low, samples will be in odd numbered positions.
      // If you connect SEL to high, data will be in even positions.
      Serial.println((values[(2*i) + 1] >> 14) + 6500);
    }
    available = 0;
  }
}