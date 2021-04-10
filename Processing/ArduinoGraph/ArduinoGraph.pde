/* 
 * This example parses a single floating point value off of serial [0,1] and plots it to the screen
 *
 * By Jon Froehlich
 * http://makeabilitylab.io
 * 
 * This example is based on:
 *  - https://www.arduino.cc/en/Tutorial/Graph
 *  - https://itp.nyu.edu/physcomp/labs/labs-serial-communication/serial-output-from-an-arduino/
 */


import processing.serial.*;

// The serial port is necessary to read data in from the Arduino
// Think of this Processing program just like Arduino's Serial Monitor and Serial Plotter tools
Serial _serialPort;

final int ARDUINO_SERIAL_PORT_INDEX = 7; // our serial port index

int _curXPos = 0; // tracks current horiz position of the drawing "pen" [0, width]
float _curArduinoVal = 0; // between [0, 1]

void setup() {
  size(800, 600);

  // Open the serial port
  _serialPort = new Serial(this, Serial.list()[ARDUINO_SERIAL_PORT_INDEX], 9600);

  // Don't generate a serialEvent() unless you get a newline character:
  _serialPort.bufferUntil('\n');
  
  // set initial background to black
  background(0);
}

void draw() {
 
  // Convert the arduino val [0,1] to a y-position value
  int curYVal = (int)map(_curArduinoVal, 0, 1, 0, height);
  
  // draw the line:
  int redColor = (int)map(_curArduinoVal, 0, 1, 0, 255);
  stroke(redColor, 34, 255); //set the color
  line(_curXPos, height, _curXPos, height - curYVal);
  
  // at the edge of the screen, go back to the beginning:
  if (_curXPos >= width) {
    _curXPos = 0;
    background(0); //clear the screen
  } else {
    // increment the horizontal position:
    _curXPos++;
  }
}

void serialEvent (Serial myPort) {
  try {
    // Grab the data off the serial port. See: 
    // https://processing.org/reference/libraries/serial/index.html
    String inString = trim(_serialPort.readStringUntil('\n'));
    
    // Our parser can handle either csv strings or just one float per line
    if (inString.contains(",")) {
      float [] data = float(split(inString, ','));
      _curArduinoVal = data[0];
    } else {
      _curArduinoVal = float(inString);
    }
    println("Read in: " + inString + " Converted val: " + _curArduinoVal + " (" + (int)(_curArduinoVal * 1023) + ")");
  }
  catch(Exception e) {
    println(e);
  }
}
