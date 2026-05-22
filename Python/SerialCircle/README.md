# Serial Circle Plotter

Reads a float value (0.0–1.0) from an Arduino over serial and draws a circle whose radius is proportional to that value using Matplotlib. The current value is displayed as a label in the center of the circle.

This is useful for visualizing analog sensor output — for example, a potentiometer or force-sensitive resistor connected to an Arduino's analog input.

Works with [AnalogOut](https://github.com/makeabilitylab/arduino/tree/master/Serial/AnalogOut) and similar Arduino sketches that print a single float per line to Serial.

**Note:** Matplotlib's refresh rate is relatively slow, so this works best when the Arduino sends data at ~10 Hz or slower. For faster data rates, consider using a library like PyQtGraph or Pygame instead.

## Setup

Install the required packages:

```bash
pip install pyserial matplotlib
```

Or, optionally, use a virtual environment to keep things isolated:

```bash
python -m venv venv
source venv/bin/activate       # macOS / Linux
venv\Scripts\activate          # Windows
pip install pyserial matplotlib
```

## Finding your serial port

**Windows:** Open Device Manager → Ports (COM & LPT), or check Arduino IDE → Tools → Port. It will be something like `COM3`.

**macOS:** Run `ls /dev/cu.usb*` in a terminal, or check Arduino IDE → Tools → Port. It will be something like `/dev/cu.usbmodem11301`.

**Linux:** Run `ls /dev/ttyACM* /dev/ttyUSB*`. Typically `/dev/ttyACM0`.

## Finding your baud rate

The baud rate must match what your Arduino sketch passes to `Serial.begin()`. Look for a line like `Serial.begin(9600);` in your Arduino code and use that same number here. Common values are **9600** and **115200**.

## Usage

```bash
# List available serial ports (to find your Arduino)
python serial_draw_circle.py --list

# Run with a specific port and baud rate
python serial_draw_circle.py COM3 9600                        # Windows
python serial_draw_circle.py /dev/cu.usbmodem11301 115200     # macOS
```
