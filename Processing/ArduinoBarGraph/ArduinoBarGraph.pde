/* 
 * This example parses multiple floating point values off of the serial port (formatted as a comma separated list or 'csv')
 * and plots them in a bar graph. Each float value is assumed to be [0, 1].
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

Bar[] _bars = new Bar[NUM_INPUTS];

// change this depending on which values to graph. Right now, it's graphing A0 and A1. To graph, A0, A3, A4, and A5,
// you would set { true, false, false, true, true, true };
boolean[] _activeInputs = { true, true, false, false, false, false }; 
String[] _labels = { "A0", "A1", "A2", "A3", "A4", "A5" }; //you can change these to be something more semantic

void setup() {
  size(640, 480);

  // Open the serial port
  _serialPort = new Serial(this, Serial.list()[ARDUINO_SERIAL_PORT_INDEX], 9600);

  // Don't generate a serialEvent() unless you get a newline character:
  _serialPort.bufferUntil('\n');
  
  float barWidth = width / (float)_bars.length;
  float curBarX = 0;
  for(int i = 0; i < _bars.length; i++){
    _bars[i] = new Bar(curBarX, barWidth, 0, _labels[i]);
    curBarX += barWidth;
  }
  
  // set initial background to black
  background(0);
  
  noLoop();
}

void draw() {
  background(20); // clear the screen by drawing a black background
    
  // draw all the bars
  for(int i = 0; i < _bars.length; i++){
    if(_activeInputs[i]){
      _bars[i].draw(); 
    }
  }
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
      
      //println(" Read in: " + inString);
      //printArray(data);
      
      for(int inputIndex = 0; inputIndex < data.length; inputIndex++){
        // Convert the arduino val [0,1] to a y-position value
        float curYPos = map(data[inputIndex], 0, 1, 0, height);
        _bars[inputIndex].barHeight = curYPos;
      }
      redraw(); // we have new data, so redraw  
    }
  }
  catch(Exception e) {
    println(e);
  }
}
