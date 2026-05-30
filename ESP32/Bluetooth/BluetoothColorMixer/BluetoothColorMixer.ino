/**
 * BluetoothColorMixer
 *
 * Bidirectional Bluetooth SPP demo for an ESP32 + a NeoPixel.
 *
 * Each side of the wireless link owns one axis of an HSV color:
 *   - The BROWSER  owns the HUE        (browser → ESP32: "h\n",  int 0..360)
 *   - The ESP32    owns the BRIGHTNESS (ESP32  → browser: "v\n",  float 0..1)
 *
 * The NeoPixel renders the combined state. The browser's brightness
 * readout mirrors the pot in real time so the UI stays in sync with
 * the physical knob without the user ever touching it. Brightness is
 * only retransmitted when it actually changes, so the wireless link
 * stays idle when the pot is sitting still.
 *
 * Hardware:
 *   - 10kΩ potentiometer on A7 (GPIO 32, an ADC1 pin — safe with WiFi/BT)
 *   - NeoPixel:
 *       * Adafruit ESP32 Feather V2 (and similar): uses the onboard NeoPixel,
 *         no extra wiring needed. PIN_NEOPIXEL is defined automatically.
 *       * Original Huzzah32 (no onboard NeoPixel): wire an external WS2812B
 *         (e.g. the 8-LED stick from the kit). Data → GPIO 27, 5V → USB pin,
 *         GND → GND. See the Addressable LEDs lesson for wiring details.
 *
 * Requires: An ESP32 with Bluetooth Classic — the original ESP32 chip
 * family (Huzzah32, ESP32 Feather V2). The S3, S2, C3, and C6 do NOT
 * support Bluetooth Classic.
 *
 * Pair the browser with the device name "ESP32-ColorMixer", then open:
 *   https://makeabilitylab.github.io/js/src/apps/serial/SerialHueBrightnessTest/
 *
 * See: https://makeabilitylab.github.io/physcomp/esp32/bluetooth-web-serial
 *
 * By Professor Jon E. Froehlich
 * Director, Makeability Lab, https://makeabilitylab.cs.uw.edu
 * Author, Interactive Physical Computing Textbook, https://makeabilitylab.github.io/physcomp/
 */

#include "BluetoothSerial.h"
#include <Adafruit_NeoPixel.h>

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth Classic is not enabled. This sketch requires the original ESP32 chip family.
#endif

#if !defined(CONFIG_BT_SPP_ENABLED)
#error Serial Bluetooth (SPP) is unavailable on this chip variant.
#endif

// NeoPixel configuration
// This looks complicated but we setup the code so that it will work both
// with ESP32 boards with a built-in NeoPixel as well as boards that do not
// (and thus, you must wire up an external NeoPixel pixel or strip)
// 
// PIN_NEOPIXEL is defined automatically for boards with an onboard NeoPixel
// (e.g. the Adafruit ESP32 Feather V2). For other boards, wire an external
// NeoPixel to the EXTERNAL_NEOPIXEL_PIN below (default: GPIO 27).
#ifdef PIN_NEOPIXEL
  const int NEOPIXEL_PIN = PIN_NEOPIXEL;
  const int NUM_NEOPIXELS = 1;
  #ifdef NEOPIXEL_I2C_POWER
    // Feather V2: this pin powers both the I2C bus and the onboard NeoPixel.
    const int NEOPIXEL_POWER_PIN = NEOPIXEL_I2C_POWER;
    #define HAS_NEOPIXEL_POWER_PIN
  #endif
#else
  const int NEOPIXEL_PIN = 27;    // external NeoPixel data line
  const int NUM_NEOPIXELS = 8;    // 8-LED stick from the kit; change if different
#endif

// Pin / protocol constants
const int POT_INPUT_PIN = A7;          // GPIO 32; ADC1 (the only safe ADC bank with BT)
const int ADC_MAX = 4095;              // 12-bit ADC on the ESP32

const unsigned long BRIGHTNESS_SEND_INTERVAL_MS = 50;  // ~20 Hz updates to the browser
const float SMOOTHING_ALPHA = 0.15f;   // exponential moving average; lower = smoother
const float BRIGHTNESS_CHANGE_THRESHOLD = 0.005f;  // ~20 LSB on a 12-bit ADC; suppresses jitter

// Globals
BluetoothSerial SerialBT;
Adafruit_NeoPixel _neoPixels(NUM_NEOPIXELS, NEOPIXEL_PIN, NEO_GRB + NEO_KHZ800);

int   _hueDegrees      = 0;     // browser-owned, 0..360
float _valueNormalized = 0.0f;  // pot-owned, 0..1 (smoothed)
float _lastSentBrightness = -1.0f;  // sentinel: forces first send after (re)connect
unsigned long _lastBrightnessSendMs = 0; // timestamp to track data transmission

void setup() {
  Serial.begin(115200);

  #ifdef HAS_NEOPIXEL_POWER_PIN
    // V2 Feather: enable power to the onboard NeoPixel.
    pinMode(NEOPIXEL_POWER_PIN, OUTPUT);
    digitalWrite(NEOPIXEL_POWER_PIN, HIGH);
  #endif

  _neoPixels.begin();
  _neoPixels.clear();
  _neoPixels.show();

  SerialBT.begin("ESP32-ColorMixer");
  Serial.println("Bluetooth started! Pair with 'ESP32-ColorMixer'.");
  Serial.println("Then open the SerialHueBrightnessTest browser app and connect.");
}

void loop() {
  // 1. Read the pot and smooth it into our brightness state
  int potRaw = analogRead(POT_INPUT_PIN);
  float potNorm = potRaw / (float)ADC_MAX;
  _valueNormalized = SMOOTHING_ALPHA * potNorm
                   + (1.0f - SMOOTHING_ALPHA) * _valueNormalized;

  // 2. Receive hue updates from the browser
  if (SerialBT.available()) {
    String line = SerialBT.readStringUntil('\n');
    line.trim();
    if (line.length() > 0) {
      int parsed = line.toInt();
      _hueDegrees = constrain(parsed, 0, 360);
    }
  }

  // 3. Render the NeoPixel(s)
  // Adafruit_NeoPixel::ColorHSV takes hue 0..65535, sat 0..255, val 0..255.
  uint16_t hue16 = map(_hueDegrees, 0, 360, 0, 65535);
  uint8_t  val8  = (uint8_t)(_valueNormalized * 255.0f);
  uint32_t color = _neoPixels.gamma32(_neoPixels.ColorHSV(hue16, 255, val8));
  for (int i = 0; i < NUM_NEOPIXELS; i++) {
    _neoPixels.setPixelColor(i, color);
  }
  _neoPixels.show();

  // 4. Stream brightness back to the browser
  // We only retransmit when the smoothed pot value has actually changed
  // beyond a small deadband. When the pot is sitting still the wireless
  // link stays idle — no wasted bandwidth, no spurious browser updates.
  // The 50 ms throttle caps the maximum rate when the pot IS moving.
  unsigned long now = millis();
  bool isConnected = SerialBT.connected();

  if (!isConnected) {
    // Reset the sentinel so the next reconnect always pushes a fresh value.
    _lastSentBrightness = -1.0f;
  } else if (now - _lastBrightnessSendMs >= BRIGHTNESS_SEND_INTERVAL_MS) {
    if (fabsf(_valueNormalized - _lastSentBrightness) >= BRIGHTNESS_CHANGE_THRESHOLD) {
      _lastBrightnessSendMs = now;
      _lastSentBrightness = _valueNormalized;
      SerialBT.println(_valueNormalized, 4);  // e.g. "0.7321\n"
    }
  }
}