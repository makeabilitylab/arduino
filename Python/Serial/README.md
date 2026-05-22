# Serial Demo

The simplest possible Python-to-Arduino serial example: type a number (0–255), send it to the Arduino, and see the echoed response. No visualization, no plotting — just `pyserial` and `input()`.

```
Enter a number (0–255): 128
  Sending: b'128'
  Received: 128

Enter a number (0–255): 255
  Sending: b'255'
  Received: 255
```

Works with Arduino sketches that echo back serial input, such as those in the [Serial](https://github.com/makeabilitylab/arduino/tree/master/Serial) folder.

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
# List available serial ports (to find your Arduino)
python serial_demo.py --list

# Run with a specific port and baud rate
python serial_demo.py COM3 9600                        # Windows
python serial_demo.py /dev/cu.usbmodem11301 115200     # macOS
```
