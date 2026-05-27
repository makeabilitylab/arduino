# serial_reader.py
#
# Reads and prints lines from a serial port. This is the simplest possible
# serial reader — no sending, no visualization, just connect and print
# whatever arrives. It pairs with Arduino sketches that stream data
# unprompted, such as HelloBluetooth.ino:
#   https://makeabilitylab.github.io/physcomp/esp32/bluetooth-serial
#
# How to find your serial port:
#
# Your serial port name depends on your operating system:
#
#   Windows:
#     Open Device Manager → Ports (COM & LPT). For USB serial, look for
#     something like "Silicon Labs CP210x" or "Arduino Uno (COM3)". For
#     Bluetooth, look for "Standard Serial over Bluetooth link (COM16)".
#     You can also run: python serial_reader.py --list
#
#   macOS:
#     Open a terminal and run: ls /dev/cu.usb* /dev/tty.*Bluetooth*
#     USB serial looks like /dev/cu.usbmodem11301.
#     Bluetooth looks like /dev/tty.ESP32-Bluetooth.
#
#   Linux:
#     Run: ls /dev/ttyACM* /dev/ttyUSB*
#     Typically /dev/ttyACM0 or /dev/ttyUSB0.
#
# To run this script:
#
# This script requires one external package: pyserial.
#
#   pip install pyserial
#
# Usage:
#
#   python serial_reader.py --list
#   python serial_reader.py COM16 115200
#   python serial_reader.py /dev/tty.ESP32-Bluetooth 115200
#
# Written by Jon E. Froehlich
# @jonfroehlich
# http://makeabilitylab.io

# Check for pyserial before importing it
try:
    import serial
except ImportError:
    print("Error: missing required package 'pyserial'.")
    print("Install it with:\n  pip install pyserial")
    raise SystemExit(1)

import argparse
import serial.tools.list_ports


def list_serial_ports():
    """Lists all available serial ports on the system."""
    try:
        ports = serial.tools.list_ports.comports()
        if ports:
            print("Available serial ports:")
            for port in ports:
                print(f"  {port}")
        else:
            print("No serial ports found. Is your device plugged in or paired?")
    except Exception:
        print("(Could not enumerate serial ports.)")


def main():
    parser = argparse.ArgumentParser(
        description="Read and print lines from a serial port. Useful for "
                    "monitoring Arduino sketches that stream data continuously.",
        epilog="Example usage:\n"
               "  python serial_reader.py --list\n"
               "  python serial_reader.py COM16 115200\n"
               "  python serial_reader.py /dev/tty.ESP32-Bluetooth 115200",
        formatter_class=argparse.RawTextHelpFormatter
    )
    parser.add_argument("port", nargs="?", default=None,
                        help="Serial port name (e.g., COM16 or /dev/tty.ESP32-Bluetooth)")
    parser.add_argument("baud", nargs="?", type=int, default=115200,
                        help="Baud rate (default: 115200)")
    parser.add_argument("--list", action="store_true",
                        help="List available serial ports and exit")
    args = parser.parse_args()

    # If the user just wants to see available ports, print them and exit.
    if args.list:
        list_serial_ports()
        return

    # If no port was provided, show usage help.
    if args.port is None:
        print("No port specified. Use --list to see available ports.\n")
        print("Usage: python serial_reader.py <port> [baud]")
        print("Example: python serial_reader.py COM16 115200")
        list_serial_ports()
        return

    print(f"Connecting to {args.port} at {args.baud} baud...")
    print("(Bluetooth ports may take a few seconds to connect.)")
    print("Press Ctrl+C to exit.\n")

    try:
        # Open the serial port. For Bluetooth COM ports, this can take
        # several seconds while the SPP connection is established.
        # The timeout parameter here controls read timeouts, not the
        # connection itself.
        ser = serial.Serial(args.port, args.baud, timeout=1)

    except serial.SerialException:
        print(f"Error: could not open '{args.port}'.")
        print(f"Make sure the port exists and no other program is using it.\n")
        print(f"Usage: python serial_reader.py <port> [baud]")
        print(f"Example: python serial_reader.py COM16 {args.baud}\n")
        list_serial_ports()
        return

    except KeyboardInterrupt:
        print("\nExiting.")
        return

    print(f"Connected! Listening for data...\n")

    try:
        with ser:
            while True:
                # readline() blocks until it receives a newline or the
                # timeout expires. It returns raw bytes.
                line = ser.readline()

                if line:
                    # Decode from bytes to string and strip whitespace
                    print(line.decode('utf-8', errors='replace').strip())

    except serial.SerialException:
        print(f"\nError: lost connection to '{args.port}'.")

    except KeyboardInterrupt:
        print("\nExiting.")


if __name__ == "__main__":
    main()