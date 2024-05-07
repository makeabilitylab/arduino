# This code reads a float value from a serial port and draws a circle with a radius
# proportional to the value. The circle is displayed using Matplotlib.
#
# Works with https://github.com/makeabilitylab/arduino/tree/master/Serial/AnalogOut 
# Note that the matplotlib refresh rate is slow so this works best if we're receiving
# data at ~10Hz or slower.
#
# You need to install pyserial and matplotlib to run this code.
# You can install them using pip:
# > pip3 install pyserial matplotlib
#
# Written by Jon E. Froehlich and GitHub Copilot
# @jonfroehlich
# http://makeabilitylab.io

import serial
import serial.tools.list_ports
import argparse
import matplotlib.pyplot as plt
from matplotlib.patches import Circle

def list_serial_ports():
    """Lists serial ports on the system."""
    print("Available serial ports:")
    ports = serial.tools.list_ports.comports()
    for port in ports:
        print(port)

def read_serial_value(serial_port):
    """
    Reads a float value from the serial port.
    Assumes that each line contains a single float value followed by a newline character.
    """
    try:
        line = serial_port.readline().decode().strip()  # Read a line from serial
        print(f"Received: {line} from serial.")
        return float(line)
    except ValueError:
        print("Invalid value received from serial.")
        return None

def main():
    print("Plotting data off serial port. Hit ctrl-C to exit...")
    parser = argparse.ArgumentParser(
        description="Serial Circle Plotter",
        epilog="Example usage:\n"
           "  1. python serial_draw_circle.py COM3 9600\n"
           '  2. python serial_draw_circle.py "/dev/cu.usbmodem11301" 115200',
        formatter_class=argparse.RawTextHelpFormatter
    )

    parser.add_argument("port", nargs="?", default="COM3", help="Serial port name (e.g., COM1 or /dev/ttyUSB0)")
    parser.add_argument("baud", nargs="?", type=int, default=115200, help="Baud rate (e.g., 9600)")

    args = parser.parse_args()

    # Setup plot
    fig, ax = plt.subplots()

    # Set the limits of the plot (optional)
    ax.set_xlim([-200, 200])
    ax.set_ylim([-200, 200])

    # Create a circle with radius 0
    circle = Circle((0, 0), 0, fill=True, facecolor='darkred', edgecolor='black')

    # Set aspect of the plot to be equal
    ax.set_aspect('equal', adjustable='box')

    # Add the circle to the axes
    ax.add_patch(circle)

    # Turn on interactive mode
    plt.ion()

    # Show the plot
    plt.show()

    try:
        with serial.Serial(args.port, args.baud, timeout=1) as ser:
            print(f"Listening on {args.port} at {args.baud} baud rate...")
            while True:
                value = read_serial_value(ser)
                if value is not None:
                    circle.radius = value * 200  # Scale the value to the desired range
                    plt.draw()
                    plt.pause(0.01)
    except serial.SerialException:
        print(f"Error opening or reading from {args.port}. Make sure the device is connected.")
        list_serial_ports()

if __name__ == "__main__":
    main()

