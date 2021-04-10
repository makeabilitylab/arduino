/* 
 * This is skeleton code for continuously reading multiple float values [0,1], which
 * are in a comma separated list, off serial
 *
 * By Jon Froehlich
 * http://makeabilitylab.io
 * 
 */

import processing.serial.*;

// We communicate with the Arduino via the serial port
Serial _serialPort;

// Our serial port index (this will change depending on your computer)
final int ARDUINO_SERIAL_PORT_INDEX = 7; 

float [] _currentData = null;

void setup(){
  size(640, 480);
  //fullScreen();
  
  // Open the serial port
  _serialPort = new Serial(this, Serial.list()[ARDUINO_SERIAL_PORT_INDEX], 9600);
}

void draw(){
  background(10); // set background color
  
  if(_currentData != null){
    // TODO: do stuff with _currentData
    
  }
}

/**
* Called automatically when new data is received over the serial port. 
*/
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
      
      printArray(data);
      
      _currentData = data;  
    }
  }
  catch(Exception e) {
    println(e);
  }
}
