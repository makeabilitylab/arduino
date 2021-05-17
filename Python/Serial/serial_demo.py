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

while True:

    # Taking input from user
    num = input("Enter a number (0 - 255): ")

    # Encode numeric value as a string
    strNum = str.encode(num)
    
    # Send the data using pyserial write method
    print("Sending...", strNum)
    ser.write(strNum)
    time.sleep(0.05) # sleep for 0.05s

    # Read data back from Arduino
    echoLine = ser.readline()

    print(echoLine);
    print(); # empty line