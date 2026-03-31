/**
 * Resistive Touch to Keyboard — Multi-Key (Makey Makey style)
 * 
 * Boards: Arduino Leonardo, Micro, or any ATmega32U4-based board
 *         (UNO does NOT support Keyboard — it lacks native USB HID)
 *
 * How it works:
 * Each input pin has a high-value pull-up resistor (1MΩ–22MΩ) to 5V,
 * keeping it near 1023. When the user touches the conductive object
 * while grounded, their body resistance pulls the voltage down.
 * We detect this drop and send the corresponding keystroke over USB.
 * 
 * Note: Unconnected analog pins will show erratic readings (they "float").
 * This is normal. Connect a pull-up resistor to each pin you plan to use,
 * even before attaching the conductive object.
 *
 * Hardware setup:
 * - 1MΩ–22MΩ resistor between 5V and each analog input pin (pull-up)
 * - Conductive objects (banana, foil, etc.) wired to each pin
 * - User holds a GND wire (or stands on grounded foil)
 *
 * Default pin-to-key mapping:
 *   A0 → Space       A3 → Left Arrow
 *   A1 → Up Arrow    A4 → Right Arrow
 *   A2 → Down Arrow  A5 → W
 *
 * To add more keys, add entries to the touchKeys[] array below.
 * Leonardo pins A6–A11 are analog-input only (no digital I/O)
 * but work fine for analogRead().
 *
 * Libraries: Keyboard (by Arduino) — install via Library Manager
 *
 * ------
 * By Jon E. Froehlich
 * Professor, Allen School of Computer Science & Engineering
 * Director, Makeability Lab, https://makeabilitylab.cs.washington.edu/
 * Textbook: https://makeabilitylab.github.io/physcomp/
 * Code: https://github.com/makeabilitylab/arduino
 */

#include <Keyboard.h>

/**
 * Represents a single resistive touch input mapped to a keyboard key.
 */
struct TouchKey {
  int pin;            // Analog input pin (e.g., A0)
  int key;            // Key code to send (char or KEY_* constant)
  int threshold;      // Analog value below which a touch is detected
  const char* label;  // Label for Serial Plotter output
  bool isKeyPressed;  // Tracks current press state
};

// Edit this array to change pin assignments, keys, or thresholds.
// Use char literals for printable keys (e.g., 'w') and KEY_* constants
// for special keys (e.g., KEY_UP_ARROW). See:
// https://www.arduino.cc/reference/en/language/functions/usb/keyboard/keyboardmodifiers/
//
// We support individual key thresholds but only print one threshold to the screen
// (AO's threshold) for readability
TouchKey touchKeys[] = {
  { A0, ' ',             800, "Space", false },
  { A1, KEY_UP_ARROW,    800, "Up",    false },
  { A2, KEY_DOWN_ARROW,  800, "Down",  false },
  { A3, KEY_LEFT_ARROW,  800, "Left",  false },
  { A4, KEY_RIGHT_ARROW, 800, "Right", false },
  { A5, 'w',             800, "W",     false },
};

const int NUM_KEYS = sizeof(touchKeys) / sizeof(touchKeys[0]);

void setup() {
  Serial.begin(9600);
  pinMode(LED_BUILTIN, OUTPUT);

  // Safety delay — gives you time to upload new code if the sketch
  // starts spamming keystrokes. Hold reset, upload, then release.
  delay(1000);

  Keyboard.begin();
}

void loop() {
  bool anyKeyPressed = false;

  for (int i = 0; i < NUM_KEYS; i++) {
    int sensorValue = analogRead(touchKeys[i].pin);
    bool touched = sensorValue < touchKeys[i].threshold;

    // Press on falling edge, release on rising edge
    if (touched && !touchKeys[i].isKeyPressed) {
      Keyboard.press(touchKeys[i].key);
      touchKeys[i].isKeyPressed = true;
    } else if (!touched && touchKeys[i].isKeyPressed) {
      Keyboard.release(touchKeys[i].key);
      touchKeys[i].isKeyPressed = false;
    }

    if (touchKeys[i].isKeyPressed) {
      anyKeyPressed = true;
    }

    // Print raw value for Serial Plotter (comma-separated label:value pairs)
    Serial.print(touchKeys[i].label);
    Serial.print(":");
    Serial.print(sensorValue);
    Serial.print(",");
  }

  // Single threshold line for reference (shared across all inputs)
  Serial.print("Threshold:");
  Serial.println(touchKeys[0].threshold);

  // LED indicates if any key is currently touched
  digitalWrite(LED_BUILTIN, anyKeyPressed ? HIGH : LOW);

  // Analog read rate — keep fast enough for responsive input,
  // slow enough to avoid flooding Serial Plotter
  delay(10);
}