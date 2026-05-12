# Serial Circle Plotter

Reads a float value (0.0–1.0) from an Arduino over serial and draws a circle whose radius is proportional to that value using Matplotlib.

Works with [AnalogOut](https://github.com/makeabilitylab/arduino/tree/master/Serial/AnalogOut) and similar Arduino sketches that print a single float per line to Serial.

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

## Usage

```bash
# List available serial ports (to find your Arduino)
python serial_draw_circle.py --list

# Run with a specific port and baud rate
python serial_draw_circle.py COM3 9600              # Windows
python serial_draw_circle.py /dev/cu.usbmodem11301 115200  # macOS
```

The baud rate must match what your Arduino sketch uses in `Serial.begin()`. Common values are 9600 and 115200.
