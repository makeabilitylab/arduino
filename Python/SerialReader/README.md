# serial_reader.py

A simple Python script that connects to a serial port and prints whatever data arrives — one line at a time. That's it. No sending, no visualization, just listening.

Use this script any time you have an Arduino (or other microcontroller) that streams data over serial and you want to see the output in a terminal. It works with both USB serial and Bluetooth serial ports.

## Prerequisites

You need **Python 3** installed on your computer. To check, open a terminal (or PowerShell on Windows) and run:

```
python --version
```

If you see something like `Python 3.x.x`, you're good. If not, download Python from [python.org](https://www.python.org/downloads/). During installation on Windows, **check the box that says "Add Python to PATH"** — this lets you run `python` from any terminal.

You also need one external package called **pyserial**, which lets Python talk to serial ports. Install it with:

```
pip install pyserial
```

> **Common mistake:** There's a different package called `serial` (without the `py`). If you accidentally run `pip install serial`, it installs the wrong thing and you'll get confusing errors. Make sure you install **`pyserial`**.

## Usage

### List available serial ports

Not sure which port your device is on? Run:

```
python serial_reader.py --list
```

You'll see something like:

```
Available serial ports:
  COM1 - Communications Port (COM1)
  COM4 - Silicon Labs CP210x USB to UART Bridge (COM4)
```

On macOS, ports look like `/dev/cu.usbmodem11301` or `/dev/tty.ESP32-Bluetooth` instead of COM numbers.

### Connect and listen

Pass the port name and baud rate as arguments:

```
# Windows
python serial_reader.py COM4 115200

# macOS
python3 serial_reader.py /dev/cu.usbmodem11301 115200

# Linux
python3 serial_reader.py /dev/ttyUSB0 115200
```

The baud rate must match what your Arduino sketch uses in `Serial.begin()`. Common values are `9600` and `115200`. If the baud rates don't match, you'll see garbled characters instead of readable text.

If everything is working, you'll see data streaming in:

```
Connected! Listening for data...

Hello from Arduino!
Hello from Arduino!
Hello from Arduino!
```

Press **Ctrl+C** to stop the script.

### Example: Bluetooth serial

This script works great with Bluetooth serial ports — your operating system makes them look just like USB serial ports. For example, the [Bluetooth Serial lesson](https://makeabilitylab.github.io/physcomp/esp32/bluetooth-serial) uses this script to receive wireless greetings from an ESP32:

```
# Windows (after pairing the ESP32 over Bluetooth)
python serial_reader.py COM16 115200

# macOS
python3 serial_reader.py /dev/tty.ESP32-Bluetooth 115200
```

See the [lesson walkthrough](https://makeabilitylab.github.io/physcomp/esp32/bluetooth-serial) for full pairing and setup instructions.

## Troubleshooting

**"No serial ports found"** — Make sure your device is plugged in (for USB) or paired (for Bluetooth). On Windows, check Device Manager → Ports (COM & LPT).

**"Could not open or read from 'COMx'"** — Another program might be using that port. Only one program can use a serial port at a time. Close Arduino's Serial Monitor or any other serial terminal before running this script.

**Garbled characters** — The baud rate probably doesn't match. Check your Arduino sketch for the `Serial.begin()` line and use the same number here.

**No output, but no error either** — The script is connected and waiting for data. Make sure your Arduino sketch is actually sending something with `Serial.println()`. If you see a blinking cursor but no text, check that your sketch is running and that you're connected to the right port.

**Windows: "python was not found"** — Try `python` instead of `python3`. If neither works, reinstall Python and make sure "Add Python to PATH" is checked.

**Two Bluetooth COM ports on Windows** — Windows creates two COM ports for each Bluetooth pairing (outgoing and incoming). If the first one doesn't work, try the other.

## Related scripts

| Script | What it does |
|---|---|
| `serial_reader.py` | Listens and prints incoming serial data (this script) |
| `serial_demo.py` | Interactive send/receive — type a number, see the echo |

## Learn more

This script is part of the [Makeability Lab Physical Computing](https://makeabilitylab.github.io/physcomp/) textbook. See the [Serial Introduction lesson](https://makeabilitylab.github.io/physcomp/communication/serial-intro) for the basics of serial communication, or the [Bluetooth Serial lesson](https://makeabilitylab.github.io/physcomp/esp32/bluetooth-serial) for using this script wirelessly.