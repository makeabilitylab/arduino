# Makeability Lab · Arduino Examples

[![License: MIT](https://img.shields.io/badge/License-MIT-blue.svg)](LICENSE)

A teaching collection of **~260 Arduino sketches** — plus supporting Python, PowerShell, and Processing helpers — that accompany the **Makeability Lab Physical Computing textbook**. Everything here is instructional material, written to be *read*: clarity and pedagogical value matter as much as correctness.

> ## 📖 New here? Start with the textbook.
> These sketches are meant to be used **alongside the lessons**, which explain the wiring, the concepts, and the code step by step.
>
> ### 👉 [makeabilitylab.github.io/physcomp](https://makeabilitylab.github.io/physcomp/)
>
> Textbook source & issues: [github.com/makeabilitylab/physcomp](https://github.com/makeabilitylab/physcomp)

> ## 🧩 Most sketches use our shared Arduino library
> The **MakeabilityLab Arduino Library** provides reusable classes for physical computing — joystick input, OLED graphics and shapes, signal filters, scrolling graphs, and more. Many sketches `#include` its headers, so install it before compiling them.
>
> ### 👉 [github.com/makeabilitylab/makelab-arduino-lib](https://github.com/makeabilitylab/makelab-arduino-lib)
>
> Install from the Arduino IDE: **Sketch → Include Library → Manage Libraries…**, search **"MakeabilityLab"**, and click **Install** (it pulls in Adafruit GFX + SSD1306 automatically).

## Getting started

1. **Install the [Arduino IDE](https://www.arduino.cc/en/software).** See the textbook's [Arduino IDE setup lesson](https://makeabilitylab.github.io/physcomp/arduino/arduino-ide.html) for a guided walkthrough.
2. **Install the MakeabilityLab library** (above) and any third-party libraries a sketch lists in its header comment (most commonly Adafruit GFX + SSD1306 for the OLED examples).
3. **Open a sketch's `.ino` file.** Each one lives in its own folder of the same name (an Arduino requirement).
4. **Select your board and port** via `Tools → Board` and `Tools → Port`, then click **Upload**.
5. **Read the header comment** at the top of every sketch — it documents the circuit (board + wiring), the required libraries, and a link to the matching textbook lesson.

> 💡 Different sketches target different boards (Uno, Leonardo, ESP32, Feather, …). Always check the header comment for the intended board. A few board-specific sketches also carry a small `sketch.yaml` that records their exact target.

## What's inside

Folders are grouped by topic or hardware platform. Every sketch is self-contained — there's no shared application or build graph.

### Learn-by-topic

| Folder | What it covers |
|---|---|
| [`Basics/`](Basics) | `analogRead`/`analogWrite`, `digitalRead`/`digitalWrite`, serial, and `tone()` — the core I/O primitives |
| [`OLED/`](OLED) | SSD1306 OLED graphics, animation, and small games |
| [`AddressableLEDs/`](AddressableLEDs) | NeoPixel and APA102 (DotStar) addressable RGB LEDs |
| [`Sensors/`](Sensors) | Accelerometers, microphones, color sensors, and more |
| [`Filters/`](Filters) | Smoothing and filtering noisy sensor input |
| [`Servo/`](Servo) | Driving servo motors |
| [`Serial/`](Serial) · [`Bluetooth/`](Bluetooth) | Serial and Bluetooth communication |
| [`GameController/`](GameController) · [`HumanInterfaceDevice/`](HumanInterfaceDevice) | USB HID — emulating a mouse/keyboard/game controller (Leonardo-class boards) |
| [`PlayingMusic/`](PlayingMusic) | Playing tones and MP3 audio |

### Board-specific

| Folder | Board |
|---|---|
| [`ESP32/`](ESP32) | ESP32 dev boards (WiFi/Bluetooth) |
| [`UnoR4WiFi/`](UnoR4WiFi) | Arduino Uno R4 WiFi |
| [`CPX/`](CPX) | Adafruit Circuit Playground Express |
| [`nRF52840/`](nRF52840) | Adafruit nRF52840 Feather |
| [`RedBearDuo/`](RedBearDuo) | RedBear Duo *(legacy)* |

### Helpers & supporting material

| Folder | What it is |
|---|---|
| [`Python/`](Python) | Host-side serial readers and tools (Python 3 + `pyserial`) |
| [`PowerShell/`](PowerShell) | Windows helper scripts |
| [`Processing/`](Processing) | Processing sketches *(legacy — the project has shifted to [p5.js](https://p5js.org/))* |
| [`templates/`](templates) | A copy-paste starting point for new sketches |
| [`docs/`](docs) | Repository conventions and maintenance notes |

## For contributors & maintainers

This repo is being modernized for the textbook's **v2.0** release. If you're editing or adding sketches, please match the existing conventions:

- **Sketch headers** follow a standard template — see [`docs/sketch-header-template.md`](docs/sketch-header-template.md), with a compile-checked starting point at [`templates/SketchTemplate/`](templates/SketchTemplate).
- **Textbook links** in headers point to *specific* lessons; keep them in lockstep with the live textbook.
- **Naming**: member/global variables use a leading underscore (`_display`); constants use `UPPER_SNAKE_CASE`.
- Some sketches are intentionally kept but not expected to compile on current toolchains — these are tracked in [`docs/legacy-sketches.md`](docs/legacy-sketches.md).

Bug reports and suggestions are welcome via [GitHub Issues](https://github.com/makeabilitylab/arduino/issues).

## About

Created and maintained by [Jon Froehlich](https://jonfroehlich.github.io/) and the [Makeability Lab](https://makeabilitylab.cs.washington.edu/) at the University of Washington, for use in physical computing courses and the accompanying [Physical Computing textbook](https://makeabilitylab.github.io/physcomp/).

## License

Released under the [MIT License](LICENSE). © 2020–present Jon Froehlich and the Makeability Lab.
