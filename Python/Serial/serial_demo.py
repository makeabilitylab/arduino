# Demonstrates how to use Python3 and the pySerial library to
# write and read from the serial port.
#
# Uses the pySerial library: 
# https://pyserial.readthedocs.io/en/latest/shortintro.html
#
# To install:
# > pip3 install pyserial
#
# By Jon E. Froehlich
# @jonfroehlich
# http://makeabilitylab.io
#

import serial # from https://pyserial.readthedocs.io
import time

# Create serial object on COM13 with 9600 baud and a read timeout
# of one second (can be a float value)
# See: 
#  - https://pyserial.readthedocs.io/en/latest/shortintro.html
#  - https://pyserial.readthedocs.io/en/latest/pyserial_api.html
ser = serial.Serial(port='COM13', baudrate=9600, timeout=1)

try:
    while True:

        # Taking input from user
        num = input("Enter a number (0 - 255): ")

        # Encode numeric value as a string
        strNum = str.encode(num)
        
        # Send the data using pyserial write method.
        # We append '\n' so the Arduino's Serial.readStringUntil('\n')
        # returns immediately rather than waiting for its default 1-second timeout.
        print("Sending...", strNum)
        ser.write(strNum + b'\n')
        time.sleep(0.05) # sleep for 0.05s

        # Read data back from Arduino
        echoLine = ser.readline()

        # readline() returns raw bytes; decode to a UTF-8 string and strip whitespace
        print(echoLine.decode('utf-8').strip())
        print() # empty line

except KeyboardInterrupt:
    print("\nExiting...")
finally:
    # Always close the serial port so the OS releases it.
    # Without this, Ctrl+C leaves the port locked and you won't be able
    # to upload new Arduino code or open Serial Monitor until you kill Python.
    ser.close()
    print("Serial port closed.")