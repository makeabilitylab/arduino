# serial_terminal_bar.py
#
# The simplest possible visualization of incoming Arduino sensor data: a text-based
# bar graph rendered directly in the terminal. No GUI window, no matplotlib — just
# pyserial and print().
#
# The program reads a single float value per line from the serial port (expected
# range 0.0 to 1.0) and prints a bar like this:
#
#   0.73 |████████████████████████████████████               |
#   0.45 |██████████████████████                             |
#   1.00 |██████████████████████████████████████████████████ |
#
# Works with https://github.com/makeabilitylab/arduino/tree/master/Serial/AnalogOut
# and any Arduino sketch that prints a single float per line via Serial.println().
#
# How to find your serial port:
#
# Your Arduino's serial port name depends on your operating system:
#
#   Windows:
#     Open Device Manager → Ports (COM & LPT). Your Arduino will show up as
#     something like "Arduino Uno (COM3)". Use that COM number as the port.
#     You can also check in the Arduino IDE under Tools → Port.
#
#   macOS:
#     Open a terminal and run: ls /dev/cu.usb*
#     You'll see something like /dev/cu.usbmodem11301. That's your port.
#     Also visible in the Arduino IDE under Tools → Port.
#
#   Linux:
#     Run: ls /dev/ttyACM* /dev/ttyUSB*
#     Typically /dev/ttyACM0 or /dev/ttyUSB0.
#
# How to find your baud rate:
#
# The baud rate must match what your Arduino sketch passes to Serial.begin().
# Look in your Arduino code for a line like:
#
#   Serial.begin(9600);    // → use 9600
#   Serial.begin(115200);  // → use 115200
#
# If the baud rates don't match, you'll see garbled characters instead of numbers.
#
# To run this script:
#
# This script requires one external package: pyserial.
#
#   pip install pyserial
#
# Or use a virtual environment (optional but recommended):
#
#   python -m venv venv
#   source venv/bin/activate       # macOS / Linux
#   venv\Scripts\activate          # Windows
#   pip install pyserial
#
# Usage:
#
#   python serial_terminal_bar.py COM3 9600
#   python serial_terminal_bar.py /dev/cu.usbmodem11301 115200
#
# Written by Jon E. Froehlich
# @jonfroehlich
# http://makeabilitylab.io

# --- Check for pyserial before importing it ---
try:
    import serial
except ImportError:
    print("Error: missing required package 'pyserial'.")
    print("Install it with:\n  pip install pyserial")
    raise SystemExit(1)

import argparse
import serial.tools.list_ports

# Width of the bar in characters. Increase for more resolution,
# decrease if your terminal is narrow.
BAR_WIDTH = 50

def list_serial_ports():
    """Lists all available serial ports on the system."""
    try:
        ports = serial.tools.list_ports.comports()
        if ports:
            print("Available serial ports:")
            for port in ports:
                print(f"  {port}")
        else:
            print("No serial ports found. Is your Arduino plugged in?")
    except Exception:
        print("(Could not enumerate serial ports.)")

def main():
    parser = argparse.ArgumentParser(
        description="Visualize serial sensor data as a text bar graph in the terminal.",
        epilog="Example usage:\n"
               "  python serial_terminal_bar.py COM3 9600\n"
               '  python serial_terminal_bar.py /dev/cu.usbmodem11301 115200',
        formatter_class=argparse.RawTextHelpFormatter
    )
    parser.add_argument("port", nargs="?", default="COM3",
                        help="Serial port name (e.g., COM3 or /dev/ttyUSB0)")
    parser.add_argument("baud", nargs="?", type=int, default=115200,
                        help="Baud rate (must match your Arduino sketch)")
    args = parser.parse_args()

    print(f"Connecting to {args.port} at {args.baud} baud. Press Ctrl+C to exit...")

    try:
        with serial.Serial(args.port, args.baud, timeout=1) as ser:
            while True:
                # Read one line from serial, decode bytes to string, strip whitespace
                line = ser.readline().decode().strip()

                # Skip empty lines (serial timeout) and non-numeric lines
                if not line:
                    continue
                try:
                    # Clamp to [0.0, 1.0] in case the Arduino sends out-of-range values
                    val = max(0.0, min(1.0, float(line)))
                except ValueError:
                    continue

                # Build a text bar: █ characters proportional to the value
                bar = "█" * int(val * BAR_WIDTH)
                print(f"{val:.2f} |{bar:<{BAR_WIDTH}}|")

    except serial.SerialException:
        print(f"\nError: could not open or read from '{args.port}'.")
        print(f"('{args.port}' is the default port. You can specify a different one")
        print(f" by passing it as a command-line argument.)\n")
        print(f"Usage: python serial_bar_graph.py <port> <baud>")
        print(f"Example: python serial_bar_graph.py /dev/cu.usbmodem101 {args.baud}\n")
        list_serial_ports()

    except KeyboardInterrupt:
        print("\nExiting.")

if __name__ == "__main__":
    main()
