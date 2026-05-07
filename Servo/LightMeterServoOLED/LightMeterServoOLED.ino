
/**
 * Light Meter Gauge with Servo, Potentiometer Override, and OLED Display
 * 
 * This sketch drives a servo-based light meter gauge using a photoresistor
 * (CdS cell / LDR) as the primary input. A potentiometer provides a manual
 * override for testing and calibration. A momentary push button toggles
 * between the two input modes.
 * 
 * === HARDWARE SETUP ===
 * 
 * Photoresistor (LDR) voltage divider:
 *   3.3V ──► [LDR] ──┬── A1
 *                     └── [10kΩ] ──► GND
 * 
 *   Why 10kΩ? A typical CdS photoresistor ranges from ~1kΩ (bright) to
 *   ~1MΩ (dark). A 10kΩ fixed resistor sits at the geometric midpoint,
 *   maximizing the ADC's dynamic range across indoor-to-outdoor conditions.
 * 
 *   With this wiring: bright light → low LDR resistance → higher voltage
 *   at A1 → higher ADC reading. So ADC value is roughly proportional to
 *   brightness, which maps naturally to our gauge (left = dark, right = bright).
 * 
 * Potentiometer:
 *   3.3V ──► [Pot outer pin] ── wiper (A0) ── [Pot outer pin] ──► GND
 * 
 * Servo:
 *   Signal pin ──► SERVO_OUTPUT_PIN (see below for board-specific defaults)
 * 
 * Mode Button:
 *   BUTTON_PIN ──► [Button] ──► GND
 *   (Uses INPUT_PULLUP, so no external resistor needed. Pressed = LOW.)
 * 
 * OLED (I2C):
 *   SDA ──► SDA pin (A4 on Uno, 23 on ESP32 Huzzah32)
 *   SCL ──► SCL pin (A5 on Uno, 22 on ESP32 Huzzah32)
 * 
 * === NON-LINEARITY NOTE ===
 * 
 * CdS photoresistors have a roughly logarithmic resistance-vs-light response.
 * However, human perception of brightness is *also* roughly logarithmic
 * (the Weber-Fechner law). These two effects partially cancel out, so the
 * raw ADC reading maps surprisingly well to "perceived brightness" without
 * any gamma correction. For a calibrated lux meter you'd need compensation,
 * but for a demonstration gauge, raw values work great.
 * 
 * If you do want to linearize for lux, apply: lux ≈ k / R^γ, where
 * γ ≈ 0.7–0.8 for most CdS cells and k is a calibration constant.
 * 
 * === SOFTWARE CONCEPTS DEMONSTRATED ===
 * - Exponential Moving Average (EMA) for ADC smoothing
 * - Debounced button input using millis() (no delay-based debounce)
 * - State machine for input mode (LDR vs. POT)
 * - Servo attach/detach to eliminate idle buzz
 * - OLED layout with multi-size text and a bar graph
 * 
 * See the Makeability Lab Physical Computing textbook:
 *   Servo:     https://makeabilitylab.github.io/physcomp/advancedio/servo.html
 *   OLED:      https://makeabilitylab.github.io/physcomp/advancedio/oled.html
 *   Smoothing: https://makeabilitylab.github.io/physcomp/advancedio/smoothing-input.html
 *
 * By Jon E. Froehlich
 * @jonfroehlich
 * https://makeabilitylab.cs.washington.edu/
 */

// ─────────────────────────────────────────────────────────────
// LIBRARIES
// ─────────────────────────────────────────────────────────────

#if defined(ESP32)
  // The standard Servo library doesn't support ESP32's LEDC PWM.
  // ESP32Servo by Kevin Harrington wraps LEDC for us.
  #include <ESP32Servo.h>
#else
  #include <Servo.h>
#endif

// OLED libraries (Adafruit SSD1306 + GFX)
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// ─────────────────────────────────────────────────────────────
// OLED CONFIGURATION
// ─────────────────────────────────────────────────────────────

#define SCREEN_WIDTH 128  // OLED width in pixels
#define SCREEN_HEIGHT 64  // OLED height in pixels
#define OLED_RESET     4  // Reset pin (-1 if sharing Arduino reset)

// Create the display object (I2C)
Adafruit_SSD1306 _display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// ─────────────────────────────────────────────────────────────
// PIN DEFINITIONS (board-specific where needed)
// ─────────────────────────────────────────────────────────────

const int POT_PIN = A0;   // Potentiometer wiper
const int LDR_PIN = A1;   // Photoresistor voltage divider output

// Other constants
const int MAX_BRIGHTNESS_ANGLE = 140;
const int MIN_BRIGHTNESS_ANGLE = 20;

#if defined(ESP32)
  const int MAX_ANALOG_VAL = 4095;        // ESP32 has a 12-bit ADC
  const int MIN_ANALOG_LDR_VAL = 950;     // Empirically measured min analog LDR val
  const int SERVO_OUTPUT_PIN = 13;        // Safe PWM pin on Huzzah32
  const int BUTTON_PIN = 27;              // Mode toggle button
#else
  const int MAX_ANALOG_VAL = 1023;        // AVR Arduinos have a 10-bit ADC
  const int MIN_ANALOG_LDR_VAL = 230;     // Empirically measured min analog LDR val
  const int SERVO_OUTPUT_PIN = 9;         // Standard servo pin on Uno
  const int BUTTON_PIN = 7;               // Mode toggle button
#endif

// ─────────────────────────────────────────────────────────────
// INPUT MODE STATE MACHINE
// ─────────────────────────────────────────────────────────────

// We use an enum to clearly represent which input source controls the servo.
// This is cleaner than a bare bool and makes the code self-documenting.
enum InputMode {
  MODE_LDR,   // Photoresistor drives the servo (light meter mode)
  MODE_POT    // Potentiometer drives the servo (manual/test mode)
};

InputMode _currentMode = MODE_LDR;  // Start in light meter mode

// ─────────────────────────────────────────────────────────────
// BUTTON DEBOUNCE
// ─────────────────────────────────────────────────────────────

// Mechanical buttons "bounce" — a single press can produce multiple
// rapid HIGH/LOW transitions over ~5–20 ms. We debounce by ignoring
// state changes that occur within DEBOUNCE_MS of the last accepted
// transition. This approach uses millis() instead of delay(), so it
// doesn't block the main loop.
const unsigned long DEBOUNCE_MS = 50;
int _lastButtonState = HIGH;           // INPUT_PULLUP → idle is HIGH
int _currentButtonState = HIGH;
unsigned long _lastDebounceTime = 0;

// ─────────────────────────────────────────────────────────────
// EXPONENTIAL MOVING AVERAGE (EMA) SMOOTHING
// ─────────────────────────────────────────────────────────────

// EMA formula: smoothed = α * newReading + (1 − α) * smoothed
//
// α (alpha) controls the trade-off between responsiveness and smoothness:
//   α close to 1.0 → almost no smoothing (tracks raw signal closely)
//   α close to 0.0 → heavy smoothing (slow to respond, very stable)
//
// We keep separate EMA state for each input so that switching modes
// doesn't cause a jarring jump — each input "remembers" where it was.
const float EMA_ALPHA_POT = 0.4;  // Pot: moderate smoothing (human turning a knob)
const float EMA_ALPHA_LDR = 0.15; // LDR: heavier smoothing (ambient light changes slowly & LDRs can be noisy)

// Minimum angle change required to move the servo. Prevents the servo
// from buzzing back and forth between two adjacent angles due to ADC
// noise or EMA rounding. A value of 1 absorbs ±1° jitter.
const int SERVO_DEAD_BAND = 1;

float _smoothedPotVal = 0;   // EMA accumulator for potentiometer
float _smoothedLdrVal = 0;   // EMA accumulator for photoresistor

// ─────────────────────────────────────────────────────────────
// SERVO STATE
// ─────────────────────────────────────────────────────────────

Servo _servo;
int _lastServoAngle = -1;  // Track last written angle to avoid redundant writes

// (OLED layout constants are computed inside drawDisplay)

// ─────────────────────────────────────────────────────────────
// SETUP
// ─────────────────────────────────────────────────────────────

void setup() {
  Serial.begin(9600);

  // Configure the mode button with the internal pull-up resistor.
  // This means the pin reads HIGH when the button is NOT pressed
  // and LOW when pressed (button connects pin to GND).
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  // Attach the servo to its output pin
  _servo.attach(SERVO_OUTPUT_PIN);

  // Initialize the OLED display
  // SSD1306_SWITCHCAPVCC tells the driver to generate the display
  // voltage internally from 3.3V
  if (!_display.begin(SSD1306_SWITCHCAPVCC, 0x3D)) {  // 0x3D for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    for (;;);  // Halt — can't continue without the display
  }

  // Seed both EMA accumulators with initial readings.
  // Without this, the smoothed values would start at 0 and slowly
  // ramp up to the actual reading, causing a visible sweep on startup.
  _smoothedPotVal = analogRead(POT_PIN);
  _smoothedLdrVal = analogRead(LDR_PIN);

  // Clear any Adafruit splash screen and prepare for drawing
  _display.clearDisplay();
  _display.setTextColor(SSD1306_WHITE);
  _display.setCursor(0, 0);
}

// ─────────────────────────────────────────────────────────────
// MAIN LOOP
// ─────────────────────────────────────────────────────────────

void loop() {

  // ── 1. CHECK BUTTON FOR MODE TOGGLE ──────────────────────

  handleButtonPress();

  // ── 2. READ AND SMOOTH BOTH INPUTS ───────────────────────

  // We always read both inputs every loop iteration, even though
  // only one controls the servo at a time. This keeps both EMA
  // accumulators "warm" so there's no jump when switching modes.

  int rawPot = analogRead(POT_PIN);
  _smoothedPotVal = EMA_ALPHA_POT * rawPot + (1.0 - EMA_ALPHA_POT) * _smoothedPotVal;
  int smoothedPot = (int)(_smoothedPotVal + 0.5);  // Round to nearest int

  int rawLdr = analogRead(LDR_PIN);
  _smoothedLdrVal = EMA_ALPHA_LDR * rawLdr + (1.0 - EMA_ALPHA_LDR) * _smoothedLdrVal;
  int smoothedLdr = (int)(_smoothedLdrVal + 0.5);

  // ── 3. COMPUTE SERVO ANGLE FROM ACTIVE INPUT ────────────

  // Pick the smoothed value from whichever input is currently active
  int servoAngle = -1;
  if(_currentMode == MODE_LDR){
    // Map the ADC range to the servo's 0–180° range.
    // For the LDR with our wiring (3.3V → LDR → A1 → 10kΩ → GND)
    // We reverse the map because low input values correspond to darker,
    // which is a higher angle (based on how we have the servo setup)
    //
    // From empirical testing, the sun angle is about 20° and the
    // dark moon is about 140°
    servoAngle = map(smoothedLdr, MIN_ANALOG_LDR_VAL, MAX_ANALOG_VAL, 
                        MAX_BRIGHTNESS_ANGLE, MIN_BRIGHTNESS_ANGLE);
  }else{
    servoAngle = map(smoothedPot, MIN_ANALOG_LDR_VAL, MAX_ANALOG_VAL, 
                        MAX_BRIGHTNESS_ANGLE, MIN_BRIGHTNESS_ANGLE);
  }

  // Constrain to valid servo range (map doesn't clamp for us if
  // the input ever falls outside the expected range due to noise)
  servoAngle = constrain(servoAngle, MIN_BRIGHTNESS_ANGLE, MAX_BRIGHTNESS_ANGLE);

  // ── 4. UPDATE SERVO (only when angle changes) ───────────

  // Writing the same angle repeatedly causes micro-corrections
  // that produce an audible buzz. We only write when the angle
  // actually changes by a threshold (set by SERVO_DEAD_BAND), 
  // and detach when idle to save power and eliminate holding-torque 
  // buzz.
  //
  // Trade-off: detaching removes holding torque, so an external
  // force could push the servo horn. For a light meter gauge
  // with no mechanical load, this is fine.
   if (abs(servoAngle - _lastServoAngle) > SERVO_DEAD_BAND) {
    if (!_servo.attached()) {
      _servo.attach(SERVO_OUTPUT_PIN);
    }
    _servo.write(servoAngle);
    _lastServoAngle = servoAngle;
  } else {
    _servo.detach();
  }

  // ── 5. UPDATE OLED DISPLAY ──────────────────────────────

  drawDisplay(rawPot, smoothedPot, rawLdr, smoothedLdr, servoAngle);

  // ── 6. SERIAL OUTPUT (for Serial Plotter / debugging) ───

  // Format: rawLdr, smoothedLdr, rawPot, smoothedPot, servoAngle
  // Open Serial Plotter (Tools → Serial Plotter) to visualize these
  Serial.print(rawLdr);
  Serial.print(",");
  Serial.print(smoothedLdr);
  Serial.print(",");
  Serial.print(rawPot);
  Serial.print(",");
  Serial.print(smoothedPot);
  Serial.print(",");
  Serial.println(servoAngle);

  // Sample at ~20 Hz. Ambient light and pot knob changes are slow (well
  // under 10 Hz), so faster sampling just adds noise without improving
  // perceived responsiveness. 20 Hz also keeps the OLED refresh smooth.
  delay(50); // 20Hz refresh
}

// ─────────────────────────────────────────────────────────────
// BUTTON HANDLING (debounced, non-blocking)
// ─────────────────────────────────────────────────────────────

/**
 * Reads the mode button and toggles _currentMode on each press.
 * Uses millis()-based debouncing so it never blocks the main loop.
 * 
 * How debouncing works here:
 * 1. Read the button pin.
 * 2. If it differs from the last raw reading, reset the debounce timer.
 * 3. Only accept the new state once it's been stable for DEBOUNCE_MS.
 * 4. Detect a falling edge (HIGH → LOW) as the "press" event.
 */
void handleButtonPress() {
  int reading = digitalRead(BUTTON_PIN);

  // If the raw reading changed, restart the debounce timer
  if (reading != _lastButtonState) {
    _lastDebounceTime = millis();
  }

  // Only accept the reading if it's been stable long enough
  if ((millis() - _lastDebounceTime) > DEBOUNCE_MS) {
    // Has the debounced state actually changed?
    if (reading != _currentButtonState) {
      _currentButtonState = reading;

      // Falling edge = button was just pressed (INPUT_PULLUP: pressed = LOW)
      if (_currentButtonState == LOW) {
        // Toggle between LDR and POT modes
        _currentMode = (_currentMode == MODE_LDR) ? MODE_POT : MODE_LDR;

        Serial.print("Mode switched to: ");
        Serial.println(_currentMode == MODE_LDR ? "LDR" : "POT");
      }
    }
  }

  // Save the raw reading for next iteration's edge detection
  _lastButtonState = reading;
}

// ─────────────────────────────────────────────────────────────
// OLED DISPLAY DRAWING
// ─────────────────────────────────────────────────────────────

/**
 * Draws the full OLED interface. Layout (128×64 pixels):
 * 
 * ┌────────────────────────────────────┐
 * │ ● LDR:512            POT:1023 ○   │  Row 0: raw values + active dots
 * │                                    │
 * │            102°                    │  Center: large servo angle
 * │                                    │
 * │ ● LDR [████████░░░░] 2048/4095    │  Row 1: LDR bar graph + smoothed/max
 * │ ○ POT [██░░░░░░░░░░] 1023/4095    │  Row 2: POT bar graph + smoothed/max
 * └────────────────────────────────────┘
 * 
 * ● = filled circle (active input)  ○ = hollow circle (inactive)
 * 
 * Both bar graphs are always visible so you can monitor both
 * inputs simultaneously, regardless of which one drives the servo.
 */
void drawDisplay(int rawPot, int smoothedPot, int rawLdr, int smoothedLdr, int servoAngle) {
  _display.clearDisplay();
  _display.setTextSize(1);  // 6×8 pixel characters for all small text

  // ── TOP ROW: Raw values with mode indicator dots ────────
  //
  // Left side:  ● LDR:512   (filled dot if LDR is active)
  // Right side: POT:1023 ○  (filled dot if POT is active)
  //
  // This gives a quick at-a-glance view of raw ADC readings from
  // both sensors, with the dot indicating which one controls the servo.

  const int DOT_RADIUS = 2;
  const int TOP_Y = 0;           // Top text row baseline
  const int DOT_CENTER_Y = 3;    // Vertically center dot within 8px text row

  // ── Left side: [dot] LDR:value
  int dotX = DOT_RADIUS;  // Inset from left edge
  if (_currentMode == MODE_LDR) {
    _display.fillCircle(dotX, DOT_CENTER_Y, DOT_RADIUS, SSD1306_WHITE);
  } else {
    _display.drawCircle(dotX, DOT_CENTER_Y, DOT_RADIUS, SSD1306_WHITE);
  }

  // Position label after the dot with a small gap
  int labelX = dotX + DOT_RADIUS + 3;
  _display.setCursor(labelX, TOP_Y);
  _display.print("LDR:");
  _display.print(rawLdr);

  // ── Right side: [dot] POT:value (right-aligned as a group)
  String potStr = "POT:" + String(rawPot);
  int potStrWidth = potStr.length() * 6;           // 6px per char at size 1
  int potTextX = SCREEN_WIDTH - potStrWidth;       // Text flush right
  int rightDotX = potTextX - DOT_RADIUS - 3;      // Dot just left of text

  if (_currentMode == MODE_POT) {
    _display.fillCircle(rightDotX, DOT_CENTER_Y, DOT_RADIUS, SSD1306_WHITE);
  } else {
    _display.drawCircle(rightDotX, DOT_CENTER_Y, DOT_RADIUS, SSD1306_WHITE);
  }

  _display.setCursor(potTextX, TOP_Y);
  _display.print(potStr);

  // ── CENTER: Large servo angle with degree symbol ────────

  _display.setTextSize(4);  // 24×32 pixel characters — big and readable
  String strAngle = String(servoAngle);

  // Use getTextBounds to measure the rendered string for centering.
  // This handles variable-width caused by different digit counts (0–180).
  int16_t x1, y1;
  uint16_t textWidth, textHeight;
  _display.getTextBounds(strAngle, 0, 0, &x1, &y1, &textWidth, &textHeight);

  // Center horizontally; vertically shift up slightly from true center
  // to leave room for the two bar graph rows at the bottom
  uint16_t xText = (SCREEN_WIDTH - textWidth) / 2;
  uint16_t yText = 12;  // Fixed top position: sits below top row, above bars
  _display.setCursor(xText, yText);
  _display.print(strAngle);

  // Draw a degree symbol as a small circle.
  // The actual '°' character renders poorly at large text sizes on the
  // SSD1306, so a small circle looks much cleaner.
  int degreeRadius = 3;
  int degreeX = xText + textWidth + degreeRadius + 2;
  int degreeY = yText + degreeRadius;
  _display.drawCircle(degreeX, degreeY, degreeRadius, SSD1306_WHITE);

  // ── BOTTOM: Dual bar graphs (LDR on top, POT below) ────
  //
  // Each row has the layout:
  //   [dot] LBL [bar graph outline with fill] smoothed/max
  //
  // Both rows are always visible so you can monitor both inputs
  // simultaneously. The filled dot marks the active input.

  _display.setTextSize(1);

  // Layout constants for the bar rows
  const int BAR_ROW_1_Y = 47;    // LDR row: y position
  const int BAR_ROW_2_Y = 57;    // POT row: y position
  const int BAR_HEIGHT = 6;      // Height of each bar
  const int BAR_DOT_RADIUS = 2;

  // Draw LDR bar row
  drawBarRow(BAR_ROW_1_Y, BAR_HEIGHT, BAR_DOT_RADIUS,
             "L", smoothedLdr, MAX_ANALOG_VAL,
             _currentMode == MODE_LDR);

  // Draw POT bar row
  drawBarRow(BAR_ROW_2_Y, BAR_HEIGHT, BAR_DOT_RADIUS,
             "P", smoothedPot, MAX_ANALOG_VAL,
             _currentMode == MODE_POT);

  // Push the framebuffer to the physical display
  _display.display();
}

/**
 * Draws a single bar graph row with mode indicator dot, label,
 * filled bar, and numeric value.
 * 
 * Layout:  ● L [████████░░░░░░] 2048/4095
 * 
 * @param y           Top y-coordinate of this row
 * @param barHeight   Height of the bar in pixels
 * @param dotRadius   Radius of the mode indicator dot
 * @param label       Short label ("L" for LDR, "P" for POT)
 * @param value       Current smoothed value (fills the bar)
 * @param maxValue    Maximum possible value (e.g., 4095 for 12-bit ADC)
 * @param isActive    true → filled dot (this input controls the servo)
 */
void drawBarRow(int y, int barHeight, int dotRadius,
                const char* label, int value, int maxValue,
                bool isActive) {

  // ── 1. Mode indicator dot ──
  int dotX = dotRadius;
  int dotY = y + barHeight / 2;

  if (isActive) {
    _display.fillCircle(dotX, dotY, dotRadius, SSD1306_WHITE);
  } else {
    _display.drawCircle(dotX, dotY, dotRadius, SSD1306_WHITE);
  }

  // ── 2. Short label (1 char to save horizontal space) ──
  int labelX = dotX + dotRadius + 3;
  _display.setCursor(labelX, y - 1);
  _display.print(label);

  // ── 3. Compute fixed bar width from worst-case value string ──
  // The longest possible string is "maxValue/maxValue" (e.g., "4095/4095").
  // By always reserving space for this, the bar width stays constant
  // regardless of the current value, making the two bars easy to compare.
  String worstCase = String(maxValue) + "/" + String(maxValue);
  int reservedWidth = worstCase.length() * 6;  // 6px per char at size 1
  int valueX = SCREEN_WIDTH - reservedWidth;

  // ── 4. Bar graph (fixed width between label and reserved text area) ──
  int barX = labelX + 8;
  int barWidth = valueX - barX - 2;

  _display.drawRect(barX, y, barWidth, barHeight, SSD1306_WHITE);

  int fillWidth = map(value, 0, maxValue, 0, barWidth - 2);
  fillWidth = constrain(fillWidth, 0, barWidth - 2);

  if (fillWidth > 0) {
    if (isActive) {
      // Solid fill for the active input
      _display.fillRect(barX + 1, y + 1, fillWidth, barHeight - 2, SSD1306_WHITE);
    } else {
      // Dithered (checkerboard) fill for the inactive input.
      // Every other pixel is drawn, creating a 50% density pattern
      // that visually distinguishes it from the solid active bar.
      for (int py = y + 1; py < y + barHeight - 1; py++) {
        for (int px = barX + 1; px < barX + 1 + fillWidth; px++) {
          if ((px + py) % 2 == 0) {
            _display.drawPixel(px, py, SSD1306_WHITE);
          }
        }
      }
    }
  }

  // ── 5. Numeric value right-aligned within the reserved area ──
  String strValue = String(value) + "/" + String(maxValue);
  int actualWidth = strValue.length() * 6;
  int actualX = SCREEN_WIDTH - actualWidth;

  _display.setCursor(actualX, y - 1);
  _display.print(strValue);
}
