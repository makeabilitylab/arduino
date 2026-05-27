/**
 * HelloBluetooth: creates a bidirectional bridge between USB Serial
 * and Bluetooth Serial (SPP). Sends a greeting over Bluetooth every
 * 2 seconds. Data received over Bluetooth is echoed to USB Serial
 * and vice versa.
 *
 * Requires: Original ESP32 (e.g., Huzzah32). Will NOT compile on ESP32-S3.
 *
 * See: https://makeabilitylab.github.io/physcomp/esp32/bluetooth-serial
 *
 * By Professor Jon E. Froehlich
 * Director, Makeability Lab, https://makeabilitylab.cs.uw.edu
 * Author, Interactive Physical Computing Textbook, https://makeabilitylab.github.io/physcomp/
 */

#include "BluetoothSerial.h"

// These compile-time checks ensure we're running on a chip that supports
// Bluetooth Classic. On the ESP32-S3 (or C3, S2, etc.), these #error
// lines will trigger and the sketch won't compile.
#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth Classic is not enabled. This sketch requires the original ESP32.
#endif

#if !defined(CONFIG_BT_SPP_ENABLED)
#error Serial Bluetooth (SPP) is not available. It is only supported on the original ESP32 chip.
#endif

BluetoothSerial SerialBT;

unsigned long _lastGreetingMs = 0;
unsigned long _greetingCount = 0;
const unsigned long GREETING_INTERVAL_MS = 2000;

void setup() {
  Serial.begin(115200);

  // Initialize Bluetooth with a device name. You can choose any name you
  // like — "ESP32-Bluetooth", "Jon's ESP32", "Pikachu", etc. This is the
  // friendly name that appears when you scan for Bluetooth devices on your
  // computer (pick something recognizable in a classroom full of ESP32s!).
  SerialBT.begin("ESP32-Bluetooth");

  Serial.println("Bluetooth started! You can now pair with 'ESP32-Bluetooth'.");
  Serial.println("Open a Bluetooth serial connection to see greetings.");
  Serial.println("Anything you type here will be forwarded over Bluetooth (and vice versa).\n");
}

void loop() {
  // Periodic greeting
  unsigned long now = millis();
  if (now - _lastGreetingMs >= GREETING_INTERVAL_MS) {
    _lastGreetingMs = now;
    _greetingCount++;

    String msg = "Hello from ESP32! [Msg #" + String(_greetingCount)
               + " | Uptime: " + String(now / 1000.0, 1) + "s]";

    // Check if Bluetooth Serial is connected
    if (SerialBT.connected()) {
      SerialBT.println("[Bluetooth] " + msg);
    } else {
      Serial.println("[USB Serial] Waiting for Bluetooth connection...");
    }
    Serial.println("[USB Serial] " + msg);
  }

  // Forward everything received from Serial (e.g., typed in Serial Monitor) 
  // to the Bluetooth peer. We use read()/write() (byte-at-a-time) rather than 
  // readStringUntil() because it's non-blocking — the loop keeps running without 
  // waiting for a newline or timeout.
  while (Serial.available()) {
    SerialBT.write(Serial.read());
  }

  // Forward everything received over Bluetooth to Serial Monitor
  while (SerialBT.available()) {
    Serial.write(SerialBT.read());
  }
}