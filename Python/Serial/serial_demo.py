# serial_demo.py
#
# Demonstrates basic serial communication between Python and Arduino using
# pyserial. The program prompts the user to enter a number (0–255), sends it
# to the Arduino over serial, and prints the echoed response.
#
# This is the simplest possible serial example — no visualization, no plotting,
# just send and receive. It pairs with Arduino sketches that echo back serial
# input, such as:
#   https://github.com/makeabilitylab/arduino/tree/master/Serial
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
#   python serial_demo.py --list
#   python serial_demo.py COM3 9600
#   python serial_demo.py /dev/cu.usbmodem11301 115200
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
import time

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
        description="Send a number (0–255) to an Arduino over serial and print "
                    "the echoed response.",
        epilog="Example usage:\n"
               "  python serial_demo.py --list\n"
               "  python serial_demo.py COM3 9600\n"
               '  python serial_demo.py /dev/cu.usbmodem11301 115200',
        formatter_class=argparse.RawTextHelpFormatter
    )
    parser.add_argument("port", nargs="?", default="COM3",
                        help="Serial port name (e.g., COM3 or /dev/ttyUSB0)")
    parser.add_argument("baud", nargs="?", type=int, default=9600,
                        help="Baud rate (must match your Arduino sketch)")
    parser.add_argument("--list", action="store_true",
                        help="List available serial ports and exit")
    args = parser.parse_args()

    # If the user just wants to see available ports, print them and exit.
    if args.list:
        list_serial_ports()
        return

    print(f"Connecting to {args.port} at {args.baud} baud. Press Ctrl+C to exit...")

    try:
        # serial.Serial() opens the connection. Using it as a context manager
        # (with ... as ser) ensures the port is properly closed when we're done,
        # even if an error occurs.
        with serial.Serial(args.port, args.baud, timeout=1) as ser:
            print(f"Connected! Type a number (0–255) and press Enter.\n")

            while True:
                # Prompt the user for a number
                user_input = input("Enter a number (0–255): ").strip()

                # Validate that the input is an integer in the expected range
                try:
                    num = int(user_input)
                except ValueError:
                    print(f"  '{user_input}' is not a valid integer. Try again.\n")
                    continue

                if num < 0 or num > 255:
                    print(f"  {num} is out of range. Please enter 0–255.\n")
                    continue

                # Encode the number as a string and send it over serial.
                # We append '\n' so the Arduino's Serial.readStringUntil('\n')
                # returns immediately rather than waiting for its default
                # 1-second timeout.
                data = str(num).encode()
                print(f"  Sending: {data}")
                ser.write(data + b'\n')

                # Brief delay to let the Arduino process and respond
                time.sleep(0.05)

                # Read the Arduino's response. readline() blocks until it
                # receives a newline or the timeout expires.
                echo = ser.readline()

                # readline() returns raw bytes; decode to UTF-8 and strip whitespace
                print(f"  Received: {echo.decode('utf-8').strip()}\n")

    except serial.SerialException:
        print(f"\nError: could not open or read from '{args.port}'.")
        print(f"('{args.port}' is the default port. You can specify a different one")
        print(f" by passing it as a command-line argument.)\n")
        print(f"Usage: python serial_demo.py <port> <baud>")
        print(f"Example: python serial_demo.py /dev/cu.usbmodem101 {args.baud}\n")
        list_serial_ports()

    except KeyboardInterrupt:
        print("\nExiting.")

if __name__ == "__main__":
    main()
