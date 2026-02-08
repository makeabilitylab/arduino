/**
 * 
 * Cycles through all built-in animations on the Arduino UNO R4 WiFi's
 * 12x8 LED matrix, scrolling each animation's name before playback.
 *
 * Hardware: Arduino UNO R4 WiFi
 * Requires: ArduinoGraphics library (install via Library Manager)
 *
 * How it works:
 *   1. We store each animation in a struct along with its name and byte size.
 *   2. For each entry, we scroll the name across the display using ArduinoGraphics.
 *   3. Then we load and play the animation, waiting for it to finish.
 *   4. After the last animation, we display "RESTARTING..." and loop back.
 *
 * ArduinoGraphics.h MUST be included before Arduino_LED_Matrix.h.
 * The LED matrix library checks for ArduinoGraphics with __has_include()
 * and only enables text/drawing support if it finds it.
 */

#include "ArduinoGraphics.h"
#include "Arduino_LED_Matrix.h"

ArduinoLEDMatrix matrix;

/** The width of the LED matrix display in columns. */
const int MATRIX_WIDTH = 12;

/**
 * Pairs a built-in animation with its display name and byte size.
 *
 * Why do we need sizeBytes?
 *   The library's loadSequence() is actually a MACRO that uses sizeof() to
 *   determine how many frames are in the animation. sizeof() only works
 *   correctly when applied to the original array name — if you pass a
 *   pointer (like a struct member), sizeof() returns the pointer size
 *   (4 bytes on ARM), not the array size. This causes the library to
 *   think the animation has 0 frames, leading to undefined behavior.
 *
 *   By storing sizeof() at initialization time (when the compiler still
 *   knows the full array size), we can later call matrix.loadWrapper()
 *   directly with the correct size.
 */
struct AnimationEntry {
  const uint32_t (*sequence)[4]; ///< Pointer to the animation frame data
  const char* name;              ///< Human-readable name to scroll on display
  uint32_t sizeBytes;            ///< Total byte size of the frame array
};

/**
 * All built-in animations from the Arduino_LED_Matrix library's gallery.h.
 *
 * Names are kept short to reduce scroll time on the 12-column display.
 * sizeof() is used on each array name directly so the compiler calculates
 * the correct total size of each animation's frame data.
 */
const AnimationEntry animations[] = {
  { LEDMATRIX_ANIMATION_STARTUP,              "STARTUP",      sizeof(LEDMATRIX_ANIMATION_STARTUP) },
  { LEDMATRIX_ANIMATION_TETRIS_INTRO,         "TETRIS INTRO", sizeof(LEDMATRIX_ANIMATION_TETRIS_INTRO) },
  { LEDMATRIX_ANIMATION_ATMEGA,               "ATMEGA",       sizeof(LEDMATRIX_ANIMATION_ATMEGA) },
  { LEDMATRIX_ANIMATION_LED_BLINK_HORIZONTAL, "BLINK H",      sizeof(LEDMATRIX_ANIMATION_LED_BLINK_HORIZONTAL) },
  { LEDMATRIX_ANIMATION_LED_BLINK_VERTICAL,   "BLINK V",      sizeof(LEDMATRIX_ANIMATION_LED_BLINK_VERTICAL) },
  { LEDMATRIX_ANIMATION_ARROWS_COMPASS,       "COMPASS",      sizeof(LEDMATRIX_ANIMATION_ARROWS_COMPASS) },
  { LEDMATRIX_ANIMATION_AUDIO_WAVEFORM,       "WAVE",         sizeof(LEDMATRIX_ANIMATION_AUDIO_WAVEFORM) },
  { LEDMATRIX_ANIMATION_BATTERY,              "BATTERY",      sizeof(LEDMATRIX_ANIMATION_BATTERY) },
  { LEDMATRIX_ANIMATION_BOUNCING_BALL,        "BALL",         sizeof(LEDMATRIX_ANIMATION_BOUNCING_BALL) },
  { LEDMATRIX_ANIMATION_BUG,                  "BUG",          sizeof(LEDMATRIX_ANIMATION_BUG) },
  { LEDMATRIX_ANIMATION_CHECK,                "CHECK",        sizeof(LEDMATRIX_ANIMATION_CHECK) },
  { LEDMATRIX_ANIMATION_CLOUD,                "CLOUD",        sizeof(LEDMATRIX_ANIMATION_CLOUD) },
  { LEDMATRIX_ANIMATION_DOWNLOAD,             "DOWNLOAD",     sizeof(LEDMATRIX_ANIMATION_DOWNLOAD) },
  { LEDMATRIX_ANIMATION_DVD,                  "DVD",          sizeof(LEDMATRIX_ANIMATION_DVD) },
  { LEDMATRIX_ANIMATION_HEARTBEAT_LINE,       "HEART LN",     sizeof(LEDMATRIX_ANIMATION_HEARTBEAT_LINE) },
  { LEDMATRIX_ANIMATION_HEARTBEAT,            "HEART",        sizeof(LEDMATRIX_ANIMATION_HEARTBEAT) },
  { LEDMATRIX_ANIMATION_INFINITY_LOOP_LOADER, "LOOP",         sizeof(LEDMATRIX_ANIMATION_INFINITY_LOOP_LOADER) },
  { LEDMATRIX_ANIMATION_LOAD_CLOCK,           "CLOCK",        sizeof(LEDMATRIX_ANIMATION_LOAD_CLOCK) },
  { LEDMATRIX_ANIMATION_LOAD,                 "LOAD",         sizeof(LEDMATRIX_ANIMATION_LOAD) },
  { LEDMATRIX_ANIMATION_LOCK,                 "LOCK",         sizeof(LEDMATRIX_ANIMATION_LOCK) },
  { LEDMATRIX_ANIMATION_NOTIFICATION,         "NOTIFY",       sizeof(LEDMATRIX_ANIMATION_NOTIFICATION) },
  { LEDMATRIX_ANIMATION_OPENSOURCE,            "OPEN SRC",     sizeof(LEDMATRIX_ANIMATION_OPENSOURCE) },
  { LEDMATRIX_ANIMATION_SPINNING_COIN,        "COIN",         sizeof(LEDMATRIX_ANIMATION_SPINNING_COIN) },
  { LEDMATRIX_ANIMATION_TETRIS,               "TETRIS",       sizeof(LEDMATRIX_ANIMATION_TETRIS) },
  { LEDMATRIX_ANIMATION_WIFI_SEARCH,          "WIFI",         sizeof(LEDMATRIX_ANIMATION_WIFI_SEARCH) },
  { LEDMATRIX_ANIMATION_HOURGLASS,            "HOURGLASS",    sizeof(LEDMATRIX_ANIMATION_HOURGLASS) },
};

const int ANIMATION_COUNT = sizeof(animations) / sizeof(animations[0]);
int currentIndex = 0;

// Forward declarations
void scrollText(const char* text);
void playAnimation(const AnimationEntry& entry);

/**
 * Initializes Serial, the LED matrix, and clears the default startup animation.
 */
void setup() {
  Serial.begin(115200);
  matrix.begin();

  // The matrix auto-plays the startup animation on begin().
  // Load a blank frame to stop it so we start with a clean slate.
  const uint32_t blank[3] = { 0, 0, 0 };
  matrix.loadFrame(blank);
  delay(100);
}

/**
 * Main loop: scrolls the animation name, plays it, then advances.
 */
void loop() {
  const AnimationEntry& current = animations[currentIndex];

  // Print progress to Serial Monitor for debugging
  Serial.print("Playing ");
  Serial.print(currentIndex + 1);
  Serial.print("/");
  Serial.print(ANIMATION_COUNT);
  Serial.print(": ");
  Serial.println(current.name);

  // Step 1: Scroll the animation name across the display
  scrollText(current.name);

  // Step 2: Play the animation and wait for it to finish
  playAnimation(current);

  // Step 3: Advance to next animation (wraps around to 0 at the end)
  currentIndex = (currentIndex + 1) % ANIMATION_COUNT;

  // After the last animation, show a restart message before looping
  if (currentIndex == 0) {
    Serial.println("--- Restarting from beginning ---");
    scrollText("RESTARTING...");
  }

  delay(500); // Brief pause between animations
}

/**
 * Scrolls a string across the LED matrix from right to left.
 *
 * Uses the ArduinoGraphics library to render and scroll text one pixel at
 * a time. This is a BLOCKING call — it does not return until the text has
 * fully scrolled off the left edge of the display.
 *
 * Text begins off-screen to the right (at column MATRIX_WIDTH) so the
 * first character scrolls in fully rather than appearing already partially
 * visible on the display.
 *
 * We do NOT wrap this in beginDraw()/endDraw() because endText()
 * internally calls beginDraw()/endDraw() for each scroll step.
 * Wrapping it in an outer pair causes nested calls and display glitches.
 *
 * @param text  The null-terminated string to scroll across the display.
 */
void scrollText(const char* text) {
  matrix.stroke(0xFFFFFFFF);                    // Set text color (white / all LEDs on)
  matrix.textScrollSpeed(70);                   // Milliseconds per scroll step (lower = faster)
  matrix.textFont(Font_5x7);                   // Use the built-in 5x7 pixel font
  matrix.beginText(MATRIX_WIDTH, 1, 0xFFFFFF); // Start off-screen right, row 1 (vertically centered)
  matrix.println(text);
  matrix.endText(SCROLL_LEFT);                 // Blocking: scrolls all text off screen, then returns
}

/**
 * Loads and plays a built-in animation sequence, waiting for completion.
 *
 * Calls matrix.loadWrapper() directly instead of the loadSequence() macro.
 * This is necessary because loadSequence() expands to:
 *
 *   loadWrapper(frames, sizeof(frames))
 *
 * sizeof() only returns the correct array size when given the original array
 * name — not a pointer (like our struct member). With a pointer, sizeof()
 * returns 4 bytes (on ARM), causing the library to compute 0 frames and
 * resulting in undefined behavior (modulo by zero in the frame counter).
 *
 * Includes a 30-second timeout as a safety net in case an animation never
 * reports completion.
 *
 * @param entry  Reference to the AnimationEntry containing the sequence
 *               data, display name, and byte size.
 */
void playAnimation(const AnimationEntry& entry) {
  // Load the animation frames with the correct byte size
  matrix.loadWrapper(entry.sequence, entry.sizeBytes);

  // Play once (false = don't loop)
  matrix.play(false);

  // Wait for the animation to finish.
  // sequenceDone() returns true once all frames have been displayed.
  // We also use a timeout as a safety net in case an animation never
  // reports completion (some built-in animations may loop internally).
  unsigned long start = millis();
  const unsigned long TIMEOUT_MS = 30000; // 30 second max per animation

  while (!matrix.sequenceDone() && (millis() - start < TIMEOUT_MS)) {
    delay(10);
  }

  if (millis() - start >= TIMEOUT_MS) {
    Serial.print("  (timed out after 30s: ");
    Serial.print(entry.name);
    Serial.println(")");
  }

  // Load a blank frame to cleanly stop the animation before moving on.
  // Without this, the last frame (or the interrupt-driven player) could
  // interfere with the next scrollText() call.
  const uint32_t blank[3] = { 0, 0, 0 };
  matrix.loadFrame(blank);
  delay(50);
}
