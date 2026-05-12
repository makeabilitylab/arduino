# serial_draw_circle.py
#
# A simple example of reading sensor data from an Arduino over a serial connection
# and visualizing it in real time using Matplotlib. The program reads a single float
# value per line (expected range 0.0 to 1.0) and draws a circle whose radius is
# proportional to that value.
#
# This is useful for visualizing analog sensor output — for example, a potentiometer
# or force-sensitive resistor connected to an Arduino's analog input.
#
# Works with https://github.com/makeabilitylab/arduino/tree/master/Serial/AnalogOut
#
# Note: Matplotlib's refresh rate is relatively slow, so this works best when the
# Arduino sends data at ~10 Hz or slower. For faster data rates, consider using
# a library like PyQtGraph or Pygame instead.
#
# ----- Setup -----
#
# This script requires two external packages: pyserial and matplotlib.
# You can install them directly:
#
#   pip install pyserial matplotlib
#
# Or, as a best practice, use a virtual environment (venv) to keep these
# packages isolated from your system Python. A venv is a self-contained
# Python installation in a folder — it avoids version conflicts and keeps
# your system clean. To set one up:
#
#   Step 1: Create the virtual environment (one-time setup):
#     python -m venv venv
#
#   Step 2: Activate it (do this each time you open a new terminal):
#     source venv/bin/activate       # macOS / Linux
#     venv\Scripts\activate          # Windows
#
#   Step 3: Install the required packages (one-time, after activating):
#     pip install pyserial matplotlib
#
#   When you're done, you can deactivate the venv by typing: deactivate
#
# ----- Usage -----
#
#   python serial_draw_circle.py --list
#   python serial_draw_circle.py COM3 9600
#   python serial_draw_circle.py "/dev/cu.usbmodem11301" 115200
#
# Written by Jon E. Froehlich and GitHub Copilot
# @jonfroehlich
# http://makeabilitylab.io

# --- Check for required packages before importing them ---
# This catches missing packages early and prints a helpful message instead
# of a confusing ModuleNotFoundError traceback.
_REQUIRED_PACKAGES = {"serial": "pyserial", "matplotlib": "matplotlib"}
_missing = []
for _mod, _pkg in _REQUIRED_PACKAGES.items():
    try:
        __import__(_mod)
    except ImportError:
        _missing.append(_pkg)
if _missing:
    print(f"Error: missing required package(s): {', '.join(_missing)}")
    print(f"Install them with:\n  pip install {' '.join(_missing)}")
    print("\nOr use a virtual environment first (recommended):")
    print("  python -m venv venv")
    print("  source venv/bin/activate   # macOS/Linux")
    print("  venv\\Scripts\\activate     # Windows")
    print(f"  pip install {' '.join(_missing)}")
    raise SystemExit(1)

import serial
import serial.tools.list_ports
import argparse
import matplotlib.pyplot as plt
from matplotlib.patches import Circle

# The maximum circle radius in plot units. This also sets the axis limits.
# Changing this single value scales the entire visualization.
MAX_RADIUS = 200

def list_serial_ports():
    """Lists all available serial ports on the system.
    
    Useful for discovering the correct port name, which varies by OS:
      - Windows: COM3, COM4, etc.
      - macOS:   /dev/cu.usbmodem11301, etc.
      - Linux:   /dev/ttyUSB0, /dev/ttyACM0, etc.
    """
    ports = serial.tools.list_ports.comports()
    if ports:
        print("Available serial ports:")
        for port in ports:
            print(f"  {port}")
    else:
        print("No serial ports found. Is your Arduino plugged in?")

def read_serial_value(serial_port):
    """
    Reads a single float value from the serial port.

    Assumes the Arduino sends one float per line (terminated by a newline character).
    For example, the Arduino might send "0.75\\n" each time through its loop.

    Returns the float value, or None if the read failed (timeout, garbled data, etc.).
    """
    try:
        # readline() blocks until it receives a newline character or the timeout expires.
        # If the timeout expires, it returns an empty bytes object (b"").
        raw_bytes = serial_port.readline()

        # decode() converts raw bytes to a Python string (assumes UTF-8 / ASCII).
        # strip() removes the trailing newline and any whitespace.
        line = raw_bytes.decode().strip()

        # If the line is empty, the read timed out — no data arrived in time.
        # This is normal and happens when the Arduino hasn't sent anything yet
        # (e.g., during startup) or if the baud rate is mismatched.
        if not line:
            return None

        value = float(line)
        print(f"Received: {value}")
        return value

    except ValueError:
        # This happens if the line isn't a valid number. Common causes:
        #   - The Arduino is printing debug text (e.g., "Starting up...")
        #   - The serial connection caught a partial line on startup
        #   - Baud rate mismatch (produces garbled characters)
        print(f"Invalid value received: '{line}' (expected a number)")
        return None

def main():
    # -------------------------------------------------------------------------
    # 1. Parse command-line arguments
    # -------------------------------------------------------------------------
    parser = argparse.ArgumentParser(
        description="Reads a float value from a serial port and draws a circle "
                    "with a radius proportional to that value.",
        epilog="Example usage:\n"
               "  python serial_draw_circle.py --list\n"
               "  python serial_draw_circle.py COM3 9600\n"
               '  python serial_draw_circle.py "/dev/cu.usbmodem11301" 115200',
        formatter_class=argparse.RawTextHelpFormatter
    )

    parser.add_argument("port", nargs="?", default="COM3",
                        help="Serial port name (e.g., COM3 or /dev/ttyUSB0)")
    parser.add_argument("baud", nargs="?", type=int, default=115200,
                        help="Baud rate (must match your Arduino sketch, e.g., 9600)")
    parser.add_argument("--list", action="store_true",
                        help="List available serial ports and exit")

    args = parser.parse_args()

    # If the user just wants to see available ports, print them and exit.
    if args.list:
        list_serial_ports()
        return

    # -------------------------------------------------------------------------
    # 2. Set up the Matplotlib figure
    # -------------------------------------------------------------------------

    # Create a figure and a single set of axes (ax).
    # The figure is the window; the axes is the coordinate system inside it.
    fig, ax = plt.subplots()

    # Set axis limits so the circle has room to grow. The circle is centered at
    # (0, 0), so we need limits of [-MAX_RADIUS, MAX_RADIUS] in both directions.
    ax.set_xlim([-MAX_RADIUS, MAX_RADIUS])
    ax.set_ylim([-MAX_RADIUS, MAX_RADIUS])

    # Force equal aspect ratio so the circle looks like a circle (not an ellipse).
    ax.set_aspect('equal', adjustable='box')

    # Create a circle patch centered at the origin with an initial radius of 0.
    # A "patch" is Matplotlib's term for a 2D shape drawn on the axes.
    circle = Circle((0, 0), 0, fill=True, facecolor='darkred', edgecolor='black')
    ax.add_patch(circle)

    # Add a text label in the center of the circle to display the current value.
    # ha/va = horizontal/vertical alignment. We'll update .set_text() each frame.
    value_text = ax.text(0, 0, "", ha='center', va='center',
                         fontsize=14, color='white', fontweight='bold')

    ax.set_title("Serial Circle Plotter (waiting for data...)")

    # Enable interactive mode. Without this, plt.show() would block the program
    # and we'd never get to read from the serial port. Interactive mode lets us
    # update the plot while our loop keeps running.
    plt.ion()

    # Display the window. Because interactive mode is on, this returns immediately.
    plt.show()

    # -------------------------------------------------------------------------
    # 3. Open the serial port and start the read loop
    # -------------------------------------------------------------------------
    print(f"Attempting to connect to {args.port} at {args.baud} baud...")

    try:
        # serial.Serial() opens the connection. Using it as a context manager
        # (with ... as ser) ensures the port is properly closed when we're done,
        # even if an error occurs.
        with serial.Serial(args.port, args.baud, timeout=1) as ser:
            print(f"Connected! Listening on {args.port}. Press Ctrl+C to exit.")
            ax.set_title(f"Serial Circle Plotter ({args.port})")

            while plt.fignum_exists(fig.number):
                value = read_serial_value(ser)

                if value is not None:
                    # Clamp the value to [0.0, 1.0] so the circle stays within
                    # our axis limits. Sensor noise or incorrect scaling on the
                    # Arduino side could produce out-of-range values.
                    clamped = max(0.0, min(1.0, value))

                    if clamped != value:
                        print(f"  (clamped {value} to {clamped})")

                    # Scale to plot units: 0.0 -> radius 0, 1.0 -> radius MAX_RADIUS
                    circle.set_radius(clamped * MAX_RADIUS)

                    # Update the text overlay to show the current value
                    value_text.set_text(f"{clamped:.2f}")

                # Redraw the plot and pause briefly. plt.pause() handles both
                # the screen refresh and GUI event processing (so the window
                # stays responsive to clicks, resizes, and the close button).
                plt.draw()
                plt.pause(0.01)

    except serial.SerialException:
        print(f"\nError: could not open or read from '{args.port}'.")
        print(f"('{args.port}' is the default port. You can specify a different one")
        print(f" by passing it as a command-line argument.)\n")
        print(f"Usage: python serial_draw_circle.py <port> <baud>")
        print(f"Example: python serial_draw_circle.py /dev/cu.usbmodem101 {args.baud}\n")
        list_serial_ports()

    except KeyboardInterrupt:
        # The user pressed Ctrl+C — exit gracefully without a traceback.
        print("\nExiting.")

    finally:
        # Ensure the plot window is closed when we're done.
        plt.close('all')

if __name__ == "__main__":
    main()