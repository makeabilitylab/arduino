# MakeabilityLab Arduino Library

A library for Arduino that provides useful classes for physical computing projects, including joystick input, OLED graphics, signal filtering, and more. Created for courses at the [Makeability Lab](https://makeabilitylab.cs.washington.edu/), University of Washington.

📖 **Course website:** [makeabilitylab.github.io/physcomp](https://makeabilitylab.github.io/physcomp/)

## What's Included

| Class | Header | Description |
|-------|--------|-------------|
| `ParallaxJoystick` | `ParallaxJoystick.hpp` | Read input from a [Parallax 2-Axis Joystick](https://www.parallax.com/product/2-axis-joystick/), with support for different orientations |
| `Button` | `Button.hpp` | Debounced digital button with press/release detection |
| `Shape`, `Rectangle`, `Circle`, `Ball` | `Shape.hpp` | Drawing primitives and collision detection for monochrome OLEDs (Adafruit SSD1306) |
| `ScrollingLineGraph` | `ScrollingLineGraph.hpp` | Single-value scrolling line graph for OLEDs |
| `ScrollingLineGraphMultiValue` | `ScrollingLineGraphMultiValue.hpp` | Multi-value scrolling line graph with per-line symbols |
| `MovingAverageFilter` | `MovingAverageFilter.hpp` | Sliding-window moving average for smoothing sensor input |
| `ColorName` | `ColorName.hpp` | Named RGB colors with closest-color matching (e.g., for color sensors) |
| `FileUtils` | `FileUtils.hpp` | SD card file utilities: list, count, and search files by extension |

## Installation

### Option 1: Clone the full repo (recommended)

```bash
git clone https://github.com/makeabilitylab/arduino.git
```

Then copy (or symlink) the `MakeabilityLab_Arduino_Library` folder into your Arduino `libraries` directory:

| OS | Libraries path |
|----|---------------|
| **Windows** | `C:\Users\<YourName>\Documents\Arduino\libraries\` |
| **macOS** | `~/Documents/Arduino/libraries/` |
| **Linux** | `~/Arduino/libraries/` |

### Option 2: Download ZIP

1. Download the library as a ZIP from the [GitHub repo](https://github.com/makeabilitylab/arduino)
2. In the Arduino IDE: **Sketch → Include Library → Add .ZIP Library…**

### Verify installation

After installing, open one of the example sketches below. If it compiles, you're all set.

## Quick Start

Include the header you need at the top of your sketch:

```cpp
#include <Shape.hpp>           // for Shape, Rectangle, Circle, Ball
#include <ParallaxJoystick.hpp> // for joystick input
#include <Button.hpp>           // for debounced buttons
```

## Examples

These example sketches use the library and are a great way to get started:

| Example | What it demonstrates |
|---------|---------------------|
| [BallBounceObjectOriented.ino](https://github.com/makeabilitylab/arduino/blob/master/OLED/BallBounceObjectOriented/BallBounceObjectOriented.ino) | Shape classes, OLED drawing, collision detection |
| [FlappyBird.ino](https://github.com/makeabilitylab/arduino/blob/master/OLED/FlappyBird/FlappyBird.ino) | Game loop, shapes, joystick input |

## Dependencies

Some classes require external libraries. Install these via the Arduino Library Manager (**Sketch → Include Library → Manage Libraries…**):

| This library class | Requires |
|--------------------|----------|
| `Shape`, `Rectangle`, `Circle`, `Ball` | [Adafruit SSD1306](https://github.com/adafruit/Adafruit_SSD1306), [Adafruit GFX](https://github.com/adafruit/Adafruit-GFX-Library) |
| `ScrollingLineGraph`, `ScrollingLineGraphMultiValue` | [Adafruit SSD1306](https://github.com/adafruit/Adafruit_SSD1306) |
| `FileUtils` | [SD](https://www.arduino.cc/reference/en/libraries/sd/) (built-in) |
| `ParallaxJoystick`, `Button`, `MovingAverageFilter`, `ColorName` | *(none — standalone)* |

## Class Reference

### ParallaxJoystick

Handles analog input from the Parallax 2-axis joystick, with support for different physical orientations (`UP`, `RIGHT`, `DOWN`, `LEFT`).

```cpp
ParallaxJoystick joystick(A0, A1);                    // up/down pin, left/right pin
ParallaxJoystick joystick(A0, A1, 1023, RIGHT);       // with max analog value and orientation

void setup() { }

void loop() {
  joystick.read();
  int upDown = joystick.getUpDownVal();       // 0–1023
  int leftRight = joystick.getLeftRightVal(); // 0–1023
}
```

### Button

Debounced button input with configurable debounce time and active-low/active-high support.

```cpp
Button myButton(2);           // pin 2, 25ms debounce, internal pull-up, active low

void setup() {
  myButton.begin();           // must call before read()
}

void loop() {
  myButton.read();
  if (myButton.isPressed()) { /* button is held down */ }
  if (myButton.wasPressed()) { /* button was just released */ }
}
```

### Shape / Rectangle / Circle / Ball

Drawing primitives for SSD1306 OLEDs with bounding-box collision detection.

```cpp
Circle ball(64, 32, 5);       // xCenter, yCenter, radius
Rectangle paddle(0, 28, 5, 8); // x, y, width, height

ball.setDrawFill(true);

if (ball.overlaps(paddle)) {
  // collision detected
}

ball.draw(display);
paddle.draw(display);
```

`Ball` extends `Circle` with velocity (`setSpeed`, `update`, `reverseXSpeed`, `reverseYSpeed`, `checkXBounce`, `checkYBounce`).

### MovingAverageFilter

Smooths noisy sensor input with a sliding-window average.

```cpp
MovingAverageFilter filter(10);  // window size of 10

void loop() {
  int raw = analogRead(A0);
  filter.add(raw);
  int smoothed = filter.getAverage();
}
```

### ScrollingLineGraph

Real-time scrolling graph for visualizing sensor data on an OLED.

```cpp
ScrollingLineGraph graph(128, 64);  // width, height in pixels
graph.setDataLabel("Sensor");

void loop() {
  graph.addData(analogRead(A0));
  display.clearDisplay();
  graph.draw(display);
  display.display();
}
```

### ScrollingLineGraphMultiValue

Like `ScrollingLineGraph` but supports multiple data series with distinct symbols.

```cpp
PointSymbol symbols[] = {CIRCLE, SQUARE};
ScrollingLineGraphMultiValue graph(2, symbols);

void loop() {
  graph.addData(0, analogRead(A0));  // line 0
  graph.addData(1, analogRead(A1));  // line 1
  display.clearDisplay();
  graph.draw(display);
  display.display();
}
```

## Author

[Jon E. Froehlich](https://jonfroehlich.github.io/)
Professor, [Allen School of Computer Science & Engineering](https://www.cs.washington.edu/), University of Washington
Director, [Makeability Lab](https://makeabilitylab.cs.washington.edu/)

## License

<!-- TODO: Add license information -->
