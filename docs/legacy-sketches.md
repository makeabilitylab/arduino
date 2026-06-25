# Legacy / CI-excluded sketches

Some sketches are intentionally kept for historical or reference value — for
example, they are the source behind demo videos linked from the textbook — but
are **not** expected to compile on current toolchains. The compile sweep and CI
**skip** the sketches listed here.

Each listed sketch should also carry a `LEGACY EXAMPLE` banner at the top of its
`.ino` explaining why (so it's obvious to anyone browsing the code on GitHub).

**Format:** one repo-relative sketch folder path per line under a section. Lines
starting with `#` or blank lines are ignored. CI/sweep tooling reads the
column-0 paths.

## ESP32/Tone — legacy v2.x (superseded by native `tone()`)

The `esp32/tone` lesson (https://makeabilitylab.github.io/physcomp/esp32/tone)
was rewritten for v2.0 to teach native `tone()`/`noTone()` (ESP32 core v3.x) with
inline code. These sketches remain as the source behind the lesson's "bonus" demo
videos; they use the removed `Tone32.hpp` and/or the v2.x LEDC API and will not
compile on core v3.x.

ESP32/Tone/AnalogInputTone
ESP32/Tone/AnalogInputTone32
ESP32/Tone/AnalogInputTone32WithOLED
ESP32/Tone/AnalogInputToneWithOLED
ESP32/Tone/BallBounceTone32WithOLED
ESP32/Tone/PlayScale
ESP32/Tone/PlayScaleWithOLED
