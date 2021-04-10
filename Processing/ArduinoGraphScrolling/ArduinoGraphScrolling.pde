/* 
 * This example parses a single floating point value off of serial [0,1] and plots it to the screen
 * Unlike ArduinoGraph, the graph scrolls automatically
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

int [] _circularBuffer; //fast way to store values (rather than an ArrayList with remove calls)
int _curWriteIndex = 0; // tracks where we are in the circular buffer

void setup() {
  size(640, 480);

  // Open the serial port
  _serialPort = new Serial(this, Serial.list()[ARDUINO_SERIAL_PORT_INDEX], 9600);

  // Don't generate a serialEvent() unless you get a newline character:
  _serialPort.bufferUntil('\n');
  
  _circularBuffer = new int[width]; // hold one value per pixel across screen width

  // set initial background to black
  background(0);
  
  noLoop();
}

void draw() {
  background(0); // clear the screen by drawing a black background
  
  int xPos = 0; 
  for (int i = _curWriteIndex; i < width; i++){
    // draw the line
    int yVal = _circularBuffer[i];
    drawValue(xPos, yVal);
    xPos++;
    //println("i=" + i + " _curWriteIndex=" + _curWriteIndex + " yVal=" + yVal + " xPos=" + xPos);
  }
  
  // println("xPos=" + xPos + " (width - _curWriteIndex)=" + (width - _curWriteIndex));
  for(int i = 0; i < _curWriteIndex; i++){
    //draw the line
    int yVal = _circularBuffer[i];
    drawValue(xPos, yVal);
    xPos++;
    //println("i=" + i + " _curWriteIndex=" + _curWriteIndex + " yVal=" + yVal + " xPos=" + xPos);
  }
  
}

/**
 * Draws a line at the specified x,y position (the y value is inverted to draw from bottom up)
 * Convenience method because we draw a line from two different loops
 */
void drawValue(int xPos, int yVal){
  int redColor = (int)map(yVal, 0, height, 0, 255);
  stroke(redColor, 34, 255); //set the color
  line(xPos, height, xPos, height - yVal);
}

void serialEvent (Serial myPort) {
  try {
    // Grab the data off the serial port. See: 
    // https://processing.org/reference/libraries/serial/index.html
    String inString = trim(_serialPort.readStringUntil('\n'));
    
    if(inString != null){
      float curArduinoVal = -1;
      
      // Our parser can handle either csv strings or just one float per line
      if (inString.contains(",")) {
        float [] data = float(split(inString, ','));
        curArduinoVal = data[0];
      } else {
        curArduinoVal = float(inString);
      }
      
      // Convert the arduino val [0,1] to a y-position value
      int curYVal = (int)map(curArduinoVal, 0, 1, 0, height);
 
      _circularBuffer[_curWriteIndex++] = curYVal;
      
      if(_curWriteIndex >= _circularBuffer.length){
        _curWriteIndex = 0;
      }
      
      redraw();
      //println("Read in: " + inString + " Converted val: " + curArduinoVal + "(" + (int)(curArduinoVal * 1023) + ")");
    }
  }
  catch(Exception e) {
    println(e);
  }
}
