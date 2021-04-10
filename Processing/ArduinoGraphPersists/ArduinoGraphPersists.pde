/* 
 * This example parses a single floating point value off of serial [0,1] and plots it to the screen.
 * Unlike ArduinoGraph, this example redraws the entire graph (not just a single value) each draw call
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

float [] _sensorVals; // stores our sensor values
int _curIndex = 0; 

void setup() {
  size(800, 600);

  // Open the serial port
  _serialPort = new Serial(this, Serial.list()[ARDUINO_SERIAL_PORT_INDEX], 9600);

  // Don't generate a serialEvent() unless you get a newline character:
  _serialPort.bufferUntil('\n');
  
  // will store our sensor values
  _sensorVals = new float[width];
  
  // set initial background to black
  background(0);
}

void draw() {
  
  // change the background based on y-position of mouse
  // helps demonstrate the difference between ArduinoGraph and this approach
  background(map(mouseY,0,height,0,255)); 

  // loop through the entire array of sensor values and graph them
  for(int i = 0; i < _sensorVals.length; i++){
    float sensorVal = _sensorVals[i];
    
    // Convert the arduino val [0,1] to a y-position value
    int curYVal = (int)map(sensorVal, 0, 1, 0, height);
    
    // draw the line:
    int redColor = (int)map(sensorVal, 0, 1, 0, 255);
    stroke(redColor, 34, 255); //set the color
    
    line(i, height, i, height - curYVal);
  }
  
  // at the edge of the sensor array, go back to the beginning:
  if (_curIndex >= _sensorVals.length) {
    _curIndex = 0;
    
    //clear the screen
    for(int i = 0; i < _sensorVals.length; i++){
      _sensorVals[i] = 0; 
    }
  } else {
    // increment the horizontal position:
    _curIndex++;
  }
}

void serialEvent (Serial myPort) {
  try {
    // Grab the data off the serial port. See: 
    // https://processing.org/reference/libraries/serial/index.html
    String inString = trim(_serialPort.readStringUntil('\n'));
    
    // Our parser can handle either csv strings or just one float per line
    float currentSensorVal = -1;
    if (inString.contains(",")) {
      float [] data = float(split(inString, ','));
      currentSensorVal= data[0];
    } else {
      currentSensorVal = float(inString);
    }
    _sensorVals[_curIndex++] = currentSensorVal;
    println("Read in: " + inString + " Converted val: " + currentSensorVal + " (" + (int)(currentSensorVal * 1023) + ")");
  }
  catch(Exception e) {
    println(e);
  }
}
