/* 
 * This example parses multiple floating point values off of the serial port (formatted as a comma separated list or 'csv')
 * and plots them in a line graph. Each float value is assumed to be [0, 1]. This is a more complicated example than 
 * ArduinoGraph and ArduinoGraphScrolling
 * 
 * By Jon Froehlich
 * http://makeabilitylab.io
 * 
 */

import processing.serial.*;

// The serial port is necessary to read data in from the Arduino
// Think of this Processing program just like Arduino's Serial Monitor and Serial Plotter tools
Serial _serialPort;

final int ARDUINO_SERIAL_PORT_INDEX = 7; // our serial port index
final int NUM_INPUTS = 6;

int[][] _circularBuffer; //fast way to store values (rather than an ArrayList with remove calls)
int _curWriteIndex = 0; // tracks where we are in the circular buffer

// change this depending on which values to graph. Right now, it's graphing A0 and A1. To graph, A0, A3, A4, and A5,
// you would set { true, false, false, true, true, true };
boolean[] _activeInputs = { true, true, false, false, false, false }; 

// Palette from ColorBrewer: http://colorbrewer2.org/#type=diverging&scheme=RdYlBu&n=6
color _colorPalette[] = new color[] { //currently has colors for six inputs
  color(215,48,39,200), 
  color(252,141,89, 200), 
  color(254,224,144, 200), 
  color(224,243,248, 200), 
  color(145,191,219, 200), 
  color(69,117,180, 200), 
};

void setup() {
  size(640, 480);

  // Open the serial port
  _serialPort = new Serial(this, Serial.list()[ARDUINO_SERIAL_PORT_INDEX], 9600);

  // Don't generate a serialEvent() unless you get a newline character:
  _serialPort.bufferUntil('\n');
  
  _circularBuffer = new int[NUM_INPUTS][width]; // hold one value per pixel across screen width

  // set initial background to black
  background(0);
  
  noLoop();
}

void draw() {
  background(0); // clear the screen by drawing a black background
    
  for(int inputIndex = 0; inputIndex < NUM_INPUTS; inputIndex++){
    int xPos = 0;
    if(_activeInputs[inputIndex]){
      for (int dataIndex = _curWriteIndex; dataIndex < width; dataIndex++){
        // draw the line
        int xPos1 = xPos - 1;   
        int yVal1 = dataIndex > 0 ? _circularBuffer[inputIndex][dataIndex - 1] :
                        _circularBuffer[inputIndex][width - 1];
        int xPos2 = xPos;
        int yVal2 = _circularBuffer[inputIndex][dataIndex];
        
        drawLine(xPos1, yVal1, xPos2, yVal2, _colorPalette[inputIndex]);
        xPos++;
        //println("inputIndex= " + inputIndex + " dataIndex=" + dataIndex + " _curWriteIndex=" + _curWriteIndex + " yVal=" + yVal + " xPos=" + xPos);
      }
    }
  }
  
  for(int inputIndex = 0; inputIndex < NUM_INPUTS; inputIndex++){
    int xPos = width - _curWriteIndex;
    if(_activeInputs[inputIndex]){
      for (int dataIndex = 0; dataIndex < _curWriteIndex; dataIndex++){
        // draw the line
        int xPos1 = xPos - 1;   
        int yVal1 = dataIndex > 0 ? _circularBuffer[inputIndex][dataIndex - 1] :
                        _circularBuffer[inputIndex][width - 1];
        int xPos2 = xPos;
        int yVal2 = _circularBuffer[inputIndex][dataIndex];
        
        drawLine(xPos1, yVal1, xPos2, yVal2, _colorPalette[inputIndex]);
        xPos++;
        //println("inputIndex=" + inputIndex + " dataIndex=" + dataIndex + " _curWriteIndex=" + _curWriteIndex + " yVal=" + yVal + " xPos=" + xPos);
      }
    }
  }
}

/**
 * Draws a line at the specified x,y position (the y value is inverted to draw from bottom up)
 * Convenience method because we draw a line from two different loops
 */
void drawLine(int xPos1, int yPos1, int xPos2, int yPos2, color dataColor){
  stroke(dataColor); //set the color
  line(xPos1, height - yPos1, xPos2, height - yPos2);
}


/**
 * Draws a point at the specified x,y position (the y value is inverted to draw from bottom up)
 * Convenience method because we draw a line from two different loops
 */
void drawValue(int xPos, int yPos, color dataColor){
  fill(dataColor); //set the color
  noStroke();
  circle(xPos, height - yPos, 4);
}

void serialEvent (Serial myPort) {
  try {
    // Grab the data off the serial port. See: 
    // https://processing.org/reference/libraries/serial/index.html
    String inString = trim(_serialPort.readStringUntil('\n'));
    
    if(inString != null){
      float [] data;
      // Our parser can handle either csv strings or just one float per line
      if(inString.contains(",")){
        data = float(split(inString, ','));
      }else{
        data = new float[] { float(inString) };
      }
      //println("_curWriteIndex=" + _curWriteIndex + " Read in: " + inString);
      //printArray(data);
      
      for(int inputIndex = 0; inputIndex < data.length; inputIndex++){
        // Convert the arduino val [0,1] to a y-position value
        int curYVal = (int)map(data[inputIndex], 0, 1, 0, height);
        _circularBuffer[inputIndex][_curWriteIndex] = curYVal;
      }
      _curWriteIndex++;
      
      if(_curWriteIndex >= width){
        _curWriteIndex = 0;
      }
      
      redraw();   
    }
  }
  catch(Exception e) {
    println(e);
  }
}
