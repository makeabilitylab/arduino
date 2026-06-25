# Sketch header template

Every Arduino sketch (`.ino`) in this repo should begin with a standard header
comment. This file is the **canonical source of truth** for that header. The
footer is shared with the Signals and p5.js repos — mirror it there, don't
re-invent.

> Status: **draft for review.** The items under "Conventions & decisions" are
> recommended defaults; adjust to taste before we apply across the repo.

## Template (copy/paste)

```cpp
/**
 * <One to three sentences: what the sketch does and why it's interesting.>
 *
 * The circuit:
 *  - <board, e.g., Arduino Uno>
 *  - <component> → <pin>   (e.g., Potentiometer wiper → A0)
 *  - <component> → <pin>
 *
 * Libraries:
 *  - <name> (<where to get it>)        // omit this whole section if only core libs
 *
 * Learn more in the Physical Computing textbook:
 *  - <Most relevant lesson title>: https://makeabilitylab.github.io/physcomp/<lesson>
 *  - <Another relevant lesson>:    https://makeabilitylab.github.io/physcomp/<lesson>
 *
 * By Professor Jon E. Froehlich
 * Director, Makeability Lab: https://makeabilitylab.cs.washington.edu/
 * Physical Computing Textbook: https://makeabilitylab.github.io/physcomp/
 *
 * SPDX-License-Identifier: MIT
 */
```

List one lesson per bullet, most relevant first. Use a single bullet if only one
lesson applies, and omit the whole section if none do.

## Filled example

```cpp
/**
 * Smooths a noisy analog signal with a moving average filter and prints the raw
 * and smoothed values to the Serial Monitor so you can compare them side by side.
 *
 * The circuit:
 *  - Arduino Uno
 *  - Analog sensor (e.g., potentiometer wiper) → A0
 *  - Uses the built-in LED (pin 13 / LED_BUILTIN)
 *
 * Libraries:
 *  - MakeabilityLab Arduino library (MovingAverageFilter.hpp):
 *    https://github.com/makeabilitylab/makelab-arduino-lib
 *
 * Learn more in the Physical Computing textbook:
 *  - Smoothing signals:      https://makeabilitylab.github.io/physcomp/signals/...
 *  - Analog input (analogRead): https://makeabilitylab.github.io/physcomp/arduino/...
 *
 * By Professor Jon E. Froehlich
 * Director, Makeability Lab: https://makeabilitylab.cs.washington.edu/
 * Physical Computing Textbook: https://makeabilitylab.github.io/physcomp/
 *
 * SPDX-License-Identifier: MIT
 */
```

## Field guide

- **Description** — Lead with what it does and why it's interesting, in plain
  language a student can follow. One to three sentences.
- **The circuit** — List the board, then each component and the pin it connects
  to. Use `→` to show wiring. This mirrors the "The circuit:" heading used in
  official Arduino examples.
- **Libraries** — List non-core libraries and where to get them, so a student
  knows what to install before compiling. **Omit the entire section** if the
  sketch only uses core libraries.
- **Learn more** — Link to the *specific* relevant textbook lesson(s), not just
  the textbook root. One lesson per bullet, **most relevant first**; multiple
  bullets are fine when several lessons apply. Omit the whole section if there's
  no corresponding lesson.
- **Footer** — Use verbatim. Shared across the arduino, signals, and p5.js repos.
- **License** — `SPDX-License-Identifier: MIT` keeps the license attached when a
  student copies a single file out of the repo (repo license is MIT, © Jon
  Froehlich, 2020).

## Conventions & decisions

1. **No dates in headers.** Git history is the source of truth for created /
   last-modified. Hardcoded dates across hundreds of files inevitably go stale
   and start lying. (Optional exception: a single `Created: <year>` line if
   desired — but never a manually-maintained "last edited.")
2. **"The circuit:" heading** for wiring (matches official Arduino convention).
3. **Include the Libraries section** when non-core libraries are used; omit
   otherwise.
4. **Include the SPDX license line.**
5. **Comment style: `/** … */`** (Doxygen-friendly), not `/* … */`.

Dropped from the old headers: the `@jonfroehlich` handle and `makeabilitylab.io`
line — superseded by the footer above.
