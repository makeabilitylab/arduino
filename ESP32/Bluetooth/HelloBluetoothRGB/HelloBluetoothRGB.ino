/**
 * HelloBluetoothRGB: creates a bidirectional bridge between USB Serial
 * and Bluetooth Serial (SPP). Sends a greeting over Bluetooth every
 * 2 seconds. Data received over Bluetooth is echoed to USB Serial
 * and vice versa.
 *
 * Uses an onboard NeoPixel to show Bluetooth activity with color:
 *   - BLUE flash:   data sent over Bluetooth (periodic greeting)
 *   - GREEN flash:  data received from Bluetooth peer
 *   - PURPLE flash: USB Serial data forwarded to Bluetooth
 *
 * The flash animation is fully non-blocking (millis()-based), so serial
 * data is never delayed. For a simpler version that uses the built-in
 * red LED with blocking delays, see HelloBluetooth.ino.
 *
 * Board: Requires an ESP32 with a built-in NeoPixel, such as the
 *        Adafruit ESP32 Feather V2 (NeoPixel on pin 0, power on pin 2).
 *        Will NOT compile on ESP32-S3 (no Bluetooth Classic).
 *
 *        If using an external NeoPixel, change NEOPIXEL_PIN to your
 *        data pin and set NEOPIXEL_POWER_PIN to -1.
 *
 * Requires: Adafruit NeoPixel library (install via Library Manager)
 *
 * See: https://makeabilitylab.github.io/physcomp/esp32/bluetooth-serial
 *
 * By Professor Jon E. Froehlich
 * Director, Makeability Lab, https://makeabilitylab.cs.uw.edu
 * Author, Interactive Physical Computing Textbook, https://makeabilitylab.github.io/physcomp/
 */

#include "BluetoothSerial.h"
#include <Adafruit_NeoPixel.h>

// Compile-time checks: Bluetooth Classic requires the original ESP32 chip ---
#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth Classic is not enabled. This sketch requires the original ESP32.
#endif

#if !defined(CONFIG_BT_SPP_ENABLED)
#error Serial Bluetooth (SPP) is not available. It is only supported on the original ESP32 chip.
#endif

// NeoPixel configuration
// ESP32 Feather V2: NeoPixel on pin 0, power on pin 2.
// External NeoPixel: change NEOPIXEL_PIN, set NEOPIXEL_POWER_PIN to -1.
const int NEOPIXEL_PIN = 0;
const int NEOPIXEL_POWER_PIN = 2;   // Set to -1 if no power pin needed
const int NUM_PIXELS = 1;

// Flash timing
const unsigned long FLASH_MS = 60;   // Duration of a single on/off cycle
const unsigned int NUM_FLASHES = 3;

// Colors for Bluetooth activity
const uint32_t COLOR_BT_SEND = Adafruit_NeoPixel::Color(0, 0, 255);  // Blue
const uint32_t COLOR_BT_RECV = Adafruit_NeoPixel::Color(0, 255, 0);  // Green
const uint32_t COLOR_USB_TO_BT = Adafruit_NeoPixel::Color(180, 0, 255); // Purple
const uint32_t COLOR_OFF     = Adafruit_NeoPixel::Color(0, 0, 0);

Adafruit_NeoPixel pixel(NUM_PIXELS, NEOPIXEL_PIN, NEO_GRB + NEO_KHZ800);

// Non-blocking flash state machine
// When startFlash() is called, it records the color and timestamp. Each time
// through loop(), updateFlash() checks whether it's time to toggle the LED
// on or off, or whether the animation is finished. The main loop never pauses.
bool _flashing = false;
uint32_t _flashColor = COLOR_OFF;
unsigned long _flashStartMs = 0;
unsigned int _flashStep = 0;       // Counts half-cycles: 0=on, 1=off, 2=on, ...
unsigned int _totalFlashSteps = 0; // NUM_FLASHES * 2 (on + off per flash)

// Bluetooth stuff
BluetoothSerial SerialBT;

unsigned long _lastMsgMs = 0;
unsigned long _msgCount = 0;
const unsigned long GREETING_INTERVAL_MS = 2000;

void setup() {
  Serial.begin(115200);

  // Enable power to the onboard NeoPixel (ESP32 Feather V2 only)
  if (NEOPIXEL_POWER_PIN >= 0) {
    pinMode(NEOPIXEL_POWER_PIN, OUTPUT);
    digitalWrite(NEOPIXEL_POWER_PIN, HIGH);
  }

  pixel.begin();
  pixel.setBrightness(30);  // Keep it dim — NeoPixels are bright!
  pixel.show();             // Initialize to off

  // Initialize Bluetooth with a device name. Pick something recognizable
  // in a classroom full of ESP32s!
  SerialBT.begin("ESP32-Bluetooth");

  Serial.println("Bluetooth started! You can now pair with 'ESP32-Bluetooth'.");
  Serial.println("Open a Bluetooth serial connection to see messages.");
  Serial.println("Anything you type here will be forwarded over Bluetooth (and vice versa).\n");
}

void loop() {
  // Advance the flash animation (returns immediately if idle)
  updateFlash();

  // Periodic greeting
  unsigned long now = millis();
  if (now - _lastMsgMs >= GREETING_INTERVAL_MS) {
    _lastMsgMs = now;
    _msgCount++;

    String msg = "Hello from ESP32! [Msg #" + String(_msgCount)
               + " | Uptime: " + String(now / 1000.0, 1) + "s]";

    if (SerialBT.connected()) {
      SerialBT.println("[Bluetooth] " + msg);
      startFlash(COLOR_BT_SEND);  // Blue flash: data sent over Bluetooth
    } else {
      Serial.println("[USB Serial] Waiting for Bluetooth connection...");
    }
    Serial.println("[USB Serial] " + msg);
  }

  // USB Serial → Bluetooth
  if (Serial.available()) {
    while (Serial.available()) {
      SerialBT.write(Serial.read());
    }
    startFlash(COLOR_USB_TO_BT);  // Purple flash: USB data forwarded to Bluetooth
  }

  // Bluetooth → USB Serial
  if (SerialBT.available()) {
    while (SerialBT.available()) {
      Serial.write(SerialBT.read());
    }
    startFlash(COLOR_BT_RECV);  // Green flash: data received over Bluetooth
  }
}

/**
 * Begins a non-blocking flash animation. If a flash is already in progress,
 * the new color takes over immediately (restarts the animation).
 *
 * @param color  The color to flash (use Adafruit_NeoPixel::Color(r, g, b))
 */
void startFlash(uint32_t color) {
  _flashing = true;
  _flashColor = color;
  _flashStep = 0;
  _totalFlashSteps = NUM_FLASHES * 2;  // Each flash = on + off
  _flashStartMs = millis();

  // Immediately turn on for the first half-cycle
  pixel.setPixelColor(0, _flashColor);
  pixel.show();
}

/**
 * Advances the flash animation by one step if enough time has elapsed.
 * Call this once per loop() iteration. Does nothing if no flash is active.
 *
 * The animation alternates between on and off states, each lasting
 * FLASH_MS / 2 milliseconds, for a total of NUM_FLASHES blinks.
 */
void updateFlash() {
  if (!_flashing) {
    return;
  }

  unsigned long elapsed = millis() - _flashStartMs;
  unsigned int expectedStep = elapsed / (FLASH_MS / 2);

  // Have we advanced to a new half-cycle?
  if (expectedStep > _flashStep) {
    _flashStep = expectedStep;

    if (_flashStep >= _totalFlashSteps) {
      // Animation complete — turn off and stop
      pixel.setPixelColor(0, COLOR_OFF);
      pixel.show();
      _flashing = false;
    } else if (_flashStep % 2 == 0) {
      // Even step = LED on
      pixel.setPixelColor(0, _flashColor);
      pixel.show();
    } else {
      // Odd step = LED off
      pixel.setPixelColor(0, COLOR_OFF);
      pixel.show();
    }
  }
}
