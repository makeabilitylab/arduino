/* 
 * This example sends a hue value as a float [0,1] in HSB colorspace over serial whenver the mouse
 * is clicked. To use this, run the ArduinoControlRGB.ino program on your Arduino.
 *   
 * FUTURE IDEAS:
 *  - Add cursor that highlights current color under mouse (done)
 *  - Add inset that shows currently selected color
 *
 * By Jon Froehlich
 * http://makeabilitylab.io
 * 
 */
import processing.serial.*;

// We communicate with the Arduino via the serial port
Serial _serialPort;

final int ARDUINO_SERIAL_PORT_INDEX = 7;

void setup(){
  size(640, 480);
  //fullScreen();
  
  // Set the color mode to HSB because it's so much easier to deal with
  // when trying to select colors (aka hues). See https://processing.org/reference/colorMode_.html
  colorMode(HSB, 1.0);  
  
  // Open the serial port
  _serialPort = new Serial(this, Serial.list()[ARDUINO_SERIAL_PORT_INDEX], 9600);
}

void draw(){
  
  for(int x = 0; x < width; x++){
    float hue = map(x, 0, width, 0, 1.0);
    stroke(hue, 1.0, 1.0);
    line(x, 0, x, height);
  }
  
  stroke(1,0,1);
  line(mouseX, 0, mouseX, height);
  
  //if you want to include saturation, you'd run this loop instead
  //for(int x = 0; x < width; x++){
  //  for(int y = 0; y < height; y++){
  //    float hue = map(x, 0, width, 0, 1.0);
  //    float saturation = map(y, 0, height, 0, 1.0);
  //    stroke(hue, saturation, 1.0);
  //    point(x, y);
  //  }
  //}
}

/**
* Called automatically when the mouse is pressed. Grabs the current hue under
* the mouse and sends it over serial as a float
*/
void mousePressed() {
  float hue = map(mouseX, 0, width, 0, 1.0); // use x-position of mouse to get hue color
  // float saturation = map(y, 0, height, 0, 1.0); //only if saturation enabled
  String dataToSend = String.format("%.2f\n",hue); // convert to string to send over serial
  println(dataToSend); // print to console
  _serialPort.write(dataToSend); // send the data over serial
}

/**
* Called automatically when new data is received over the serial port. In this case,
* we use it for debugging as our debug Arduino Serial.print calls will send data over serial
*/
void serialEvent (Serial myPort) {
  try {
    // Grab the data off the serial port. See: 
    // https://processing.org/reference/libraries/serial/index.html
    String inString = trim(_serialPort.readStringUntil('\n'));
    if(inString != null){
      println("Read in: " + inString);
    }
  }
  catch(Exception e) {
    println(e);
  }
}
