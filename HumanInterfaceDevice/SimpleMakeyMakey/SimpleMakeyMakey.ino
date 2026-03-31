/**
 * Resistive Touch to Keyboard (Makey Makey style)
 * 
 * Boards: Arduino Leonardo, Micro, or any ATmega32U4-based board
 *         (UNO does NOT support Keyboard — it lacks native USB HID)
 *
 * How it works:
 * A high-value pull-up resistor (1MΩ–22MΩ) keeps A0 near 5V (~1023).
 * When the user touches the conductive object while grounded, their body
 * resistance (~100kΩ–1MΩ) forms a voltage divider, pulling A0 down.
 * We detect this drop and send a keystroke over USB.
 *
 * Hardware setup:
 * - 1MΩ–22MΩ resistor between 5V and A0 (pull-up)
 * - Conductive object (banana, foil, etc.) wired to A0
 * - User holds a GND wire (or stands on grounded foil)
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

const int SENSOR_PIN = A0;

// Threshold depends on the pull-up resistor value and the conductive object.
// A typical resting value is near 1023 (5V). Touching drops it significantly.
// Tune this value based on what you see in the Serial Plotter.
const int THRESHOLD = 800; 
const char KEY_TO_PRESS = ' ';

// Tracks if key is pressed
bool isKeyPressed = false;

void setup() {
  // Initialize serial communication for the Serial Plotter
  Serial.begin(9600);
  
  // Setup the built-in LED to turn on when a button is pressed
  pinMode(LED_BUILTIN, OUTPUT);

  // Safety delay — gives you time to upload new code if the sketch
  // starts spamming keystrokes. Hold reset, upload, then release.
  delay(1000);

  // Initialize USB keyboard emulation
  Keyboard.begin();
}

void loop() {
  int sensorValue = analogRead(SENSOR_PIN);

  // When the user touches the object, voltage drops below the threshold
  if (sensorValue < THRESHOLD && !isKeyPressed) {
    Keyboard.press(KEY_TO_PRESS);
    isKeyPressed = true;
    digitalWrite(LED_BUILTIN, HIGH);
  } else if (sensorValue >= THRESHOLD && isKeyPressed) {
    Keyboard.release(KEY_TO_PRESS);
    isKeyPressed = false;
    digitalWrite(LED_BUILTIN, LOW);
  }
  
  // Format output so the IDE auto-generates a legend and distinct lines
  Serial.print("Raw_A0:");
  Serial.print(sensorValue);
  Serial.print(",");
  Serial.print("Threshold:");
  Serial.println(THRESHOLD);
  
  // Analog read rate — keep fast enough for responsive input,
  // slow enough to avoid flooding Serial Plotter
  delay(10);
}