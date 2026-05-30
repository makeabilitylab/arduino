/**
 * BluetoothPotentiometer: reads a potentiometer and streams a normalized
 * value (0.0–1.0) over Bluetooth Serial. The pot also controls the
 * brightness of the built-in LED, so you get immediate visual feedback
 * on what the sensor is doing.
 *
 * Channel split: numerical data goes out over Bluetooth (for the Python
 * visualizer to consume), while USB Serial carries human-readable status
 * messages — the setup banner and pairing events — for the Serial Monitor.
 *
 * Pairs with the Makeability Lab serial visualizers (serial_bar_graph.py,
 * serial_draw_circle.py), which read one float per line in the range [0.0, 1.0].
 *
 * Circuit:
 * - 10kΩ potentiometer on A7 (GPIO 32) — must be an ADC1 pin
 *
 * Requires: Original ESP32 (e.g., Huzzah32). Will NOT compile on ESP32-S3.
 *
 * See: https://makeabilitylab.github.io/physcomp/esp32/bluetooth-web-serial
 *
 * By Professor Jon E. Froehlich
 * Director, Makeability Lab, https://makeabilitylab.cs.uw.edu
 * Author, Interactive Physical Computing Textbook, https://makeabilitylab.github.io/physcomp/
 */

#include "BluetoothSerial.h"

// Compile-time checks: Bluetooth Classic (SPP) is only available on the
// original ESP32. On ESP32-S3/C3/S2 these will trip and stop the build.
#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth Classic is not enabled. This sketch requires the original ESP32.
#endif

#if !defined(CONFIG_BT_SPP_ENABLED)
#error Serial Bluetooth (SPP) is not available. It is only supported on the original ESP32 chip.
#endif

BluetoothSerial SerialBT;

const int POT_INPUT_PIN = A7;           // GPIO 32, an ADC1 pin on the Huzzah32
const int LED_PIN = LED_BUILTIN;        // Pin 13 on the Huzzah32; aliased for clarity
const int ADC_MAX = 4095;               // ESP32 ADC is 12-bit (0..4095)
const int PWM_MAX = 255;                // analogWrite() expects 0..255 by default
const bool MIRROR_DATA_TO_USB = false;  // if on, transmits data over Serial.println as well.

// Sample rate is defined once here; the actual frequency is computed from it
// and printed in setup(), so changing this value automatically updates the banner.
const unsigned long SAMPLE_INTERVAL_MS = 50;  // 50 ms → 20 Hz

void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);

  SerialBT.begin("ESP32-PotSensor");

  Serial.println("Bluetooth started! Pair with 'ESP32-PotSensor' to see live data.");
  Serial.printf("Streaming normalized pot values (0.0–1.0) at %.1f Hz (every %lu ms).\n",
                1000.0 / SAMPLE_INTERVAL_MS, SAMPLE_INTERVAL_MS);
  Serial.println("The built-in LED brightness tracks the pot position.\n");
}

void loop() {
  int potVal = analogRead(POT_INPUT_PIN);   // 0..4095 (12-bit ADC on ESP32)

  // Normalize to 0.0–1.0 before sending. Two reasons:
  //   1. Decouples sender from receiver. The receiving program doesn't need
  //      to know the ADC bit depth (10-bit on Uno, 12-bit here, etc.) 
  //
  //   2. Matches the protocol our Python visualizers expect
  //      (serial_bar_graph.py, serial_draw_circle.py): one float per line
  //      in [0.0, 1.0].
  //
  // Note the cast to float: without it, integer division would round
  // every result to 0.
  float normalized = potVal / (float)ADC_MAX;

  // Drive the built-in LED. analogWrite on the ESP32 wraps LEDC and expects 0..255.
  int brightness = (int)(normalized * PWM_MAX);
  analogWrite(LED_PIN, brightness);

  // Track Bluetooth connection state changes and report them over USB Serial.
  static bool wasConnected = false;
  bool isConnected = SerialBT.connected();
  if (isConnected != wasConnected) {
    Serial.println(isConnected ? "[BT] Client connected." : "[BT] Client disconnected.");
    wasConnected = isConnected;
  }

  // Send the normalized value over Bluetooth — but only if a client is paired
  // Use 4 decimal places: the default of 2 throws away most of the ADC's
  // resolution (~100 distinct values instead of ~4000).
  if (isConnected) {
    SerialBT.println(normalized, 4);
  }

  // Also send data via USB Serial for debugging
  if(MIRROR_DATA_TO_USB){
    Serial.println(normalized, 4);
  }

  delay(SAMPLE_INTERVAL_MS);
}
