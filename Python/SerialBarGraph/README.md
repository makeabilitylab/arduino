# Serial Terminal Bar

The simplest possible visualization of Arduino sensor data: a text-based bar graph printed directly in the terminal. No GUI, no matplotlib — just pyserial and `print()`.

```
0.73 |████████████████████████████████████               |
0.45 |██████████████████████                             |
1.00 |██████████████████████████████████████████████████ |
```

Works with [AnalogOut](https://github.com/makeabilitylab/arduino/tree/master/Serial/AnalogOut) and any Arduino sketch that prints a single float (0.0–1.0) per line via serial.

## Setup

```bash
pip install pyserial
```

Or, optionally, use a virtual environment:

```bash
python -m venv venv
source venv/bin/activate       # macOS / Linux
venv\Scripts\activate          # Windows
pip install pyserial
```

## Finding your serial port

**Windows:** Open Device Manager → Ports (COM & LPT), or check Arduino IDE → Tools → Port. It will be something like `COM3`.

**macOS:** Run `ls /dev/cu.usb*` in a terminal, or check Arduino IDE → Tools → Port. It will be something like `/dev/cu.usbmodem11301`.

**Linux:** Run `ls /dev/ttyACM* /dev/ttyUSB*`. Typically `/dev/ttyACM0`.

## Finding your baud rate

The baud rate must match what your Arduino sketch passes to `Serial.begin()`. Look for a line like `Serial.begin(9600);` in your Arduino code and use that same number here. Common values are **9600** and **115200**.

## Usage

```bash
python serial_bar_graph.py COM3 9600                        # Windows
python serial_bar_graph.py /dev/cu.usbmodem11301 115200     # macOS
```
