/**
 * SimpleBuiltInAnimationGallery.ino
 *
 * Cycles through all built-in animations on the Arduino UNO R4 WiFi's
 * 12x8 LED matrix, playing each one once before advancing to the next.
 *
 * Hardware: Arduino UNO R4 WiFi
 *
 * Note: We call matrix.loadWrapper() directly instead of the loadSequence()
 * macro. loadSequence() uses sizeof() internally, which only returns the
 * correct array size when given the original array name. When given a
 * pointer (like an array element), sizeof() returns the pointer size
 * (4 bytes), causing 0 frames to be loaded. We work around this by
 * storing each animation's byte size alongside its pointer.
 *
 * By Professor Jon E. Froehlich
 * https://makeabilitylab.github.io/physcomp/
 * https://github.com/makeabilitylab/arduino 
 */

#include "Arduino_LED_Matrix.h"

ArduinoLEDMatrix matrix;

// Each animation is paired with its byte size, since we can't rely on
// the loadSequence() macro when using pointers (see file comment above).
struct AnimationEntry {
  const uint32_t (*sequence)[4];
  uint32_t sizeBytes;
};

const AnimationEntry animations[] = {
  { LEDMATRIX_ANIMATION_STARTUP,              sizeof(LEDMATRIX_ANIMATION_STARTUP) },
  { LEDMATRIX_ANIMATION_TETRIS_INTRO,         sizeof(LEDMATRIX_ANIMATION_TETRIS_INTRO) },
  { LEDMATRIX_ANIMATION_ATMEGA,               sizeof(LEDMATRIX_ANIMATION_ATMEGA) },
  { LEDMATRIX_ANIMATION_LED_BLINK_HORIZONTAL, sizeof(LEDMATRIX_ANIMATION_LED_BLINK_HORIZONTAL) },
  { LEDMATRIX_ANIMATION_LED_BLINK_VERTICAL,   sizeof(LEDMATRIX_ANIMATION_LED_BLINK_VERTICAL) },
  { LEDMATRIX_ANIMATION_ARROWS_COMPASS,       sizeof(LEDMATRIX_ANIMATION_ARROWS_COMPASS) },
  { LEDMATRIX_ANIMATION_AUDIO_WAVEFORM,       sizeof(LEDMATRIX_ANIMATION_AUDIO_WAVEFORM) },
  { LEDMATRIX_ANIMATION_BATTERY,              sizeof(LEDMATRIX_ANIMATION_BATTERY) },
  { LEDMATRIX_ANIMATION_BOUNCING_BALL,        sizeof(LEDMATRIX_ANIMATION_BOUNCING_BALL) },
  { LEDMATRIX_ANIMATION_BUG,                  sizeof(LEDMATRIX_ANIMATION_BUG) },
  { LEDMATRIX_ANIMATION_CHECK,                sizeof(LEDMATRIX_ANIMATION_CHECK) },
  { LEDMATRIX_ANIMATION_CLOUD,                sizeof(LEDMATRIX_ANIMATION_CLOUD) },
  { LEDMATRIX_ANIMATION_DOWNLOAD,             sizeof(LEDMATRIX_ANIMATION_DOWNLOAD) },
  { LEDMATRIX_ANIMATION_DVD,                  sizeof(LEDMATRIX_ANIMATION_DVD) },
  { LEDMATRIX_ANIMATION_HEARTBEAT_LINE,       sizeof(LEDMATRIX_ANIMATION_HEARTBEAT_LINE) },
  { LEDMATRIX_ANIMATION_HEARTBEAT,            sizeof(LEDMATRIX_ANIMATION_HEARTBEAT) },
  { LEDMATRIX_ANIMATION_INFINITY_LOOP_LOADER, sizeof(LEDMATRIX_ANIMATION_INFINITY_LOOP_LOADER) },
  { LEDMATRIX_ANIMATION_LOAD_CLOCK,           sizeof(LEDMATRIX_ANIMATION_LOAD_CLOCK) },
  { LEDMATRIX_ANIMATION_LOAD,                 sizeof(LEDMATRIX_ANIMATION_LOAD) },
  { LEDMATRIX_ANIMATION_LOCK,                 sizeof(LEDMATRIX_ANIMATION_LOCK) },
  { LEDMATRIX_ANIMATION_NOTIFICATION,         sizeof(LEDMATRIX_ANIMATION_NOTIFICATION) },
  { LEDMATRIX_ANIMATION_OPENSOURCE,            sizeof(LEDMATRIX_ANIMATION_OPENSOURCE) },
  { LEDMATRIX_ANIMATION_SPINNING_COIN,        sizeof(LEDMATRIX_ANIMATION_SPINNING_COIN) },
  { LEDMATRIX_ANIMATION_TETRIS,               sizeof(LEDMATRIX_ANIMATION_TETRIS) },
  { LEDMATRIX_ANIMATION_WIFI_SEARCH,          sizeof(LEDMATRIX_ANIMATION_WIFI_SEARCH) },
  { LEDMATRIX_ANIMATION_HOURGLASS,            sizeof(LEDMATRIX_ANIMATION_HOURGLASS) },
};

const int ANIMATION_COUNT = sizeof(animations) / sizeof(animations[0]);
int currentIndex = 0;

void setup() {
  Serial.begin(115200);
  matrix.begin();

  Serial.print("Playing animation 1/");
  Serial.println(ANIMATION_COUNT);

  // Load the first animation and play it once
  matrix.loadWrapper(animations[0].sequence, animations[0].sizeBytes);
  matrix.play(false);
}

void loop() {
  // When the current animation finishes, advance to the next one
  if (matrix.sequenceDone()) {
    currentIndex = (currentIndex + 1) % ANIMATION_COUNT;

    Serial.print("Playing animation ");
    Serial.print(currentIndex + 1);
    Serial.print("/");
    Serial.println(ANIMATION_COUNT);

    matrix.loadWrapper(animations[currentIndex].sequence, animations[currentIndex].sizeBytes);
    matrix.play(false);
  }
}