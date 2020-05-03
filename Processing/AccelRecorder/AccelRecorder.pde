/**
 * Records accelerometer-based gestures. Works with the Arduino program
 * ADXL335SerialWriter.ino, LISDHSerialWriter.ino (or any program similar that 
 * provides a CSV input stream on the serial port of "timestamp, x, y, z")
 *   
 * By Jon E. Froehlich
 * @jonfroehlich
 * http://makeabilitylab.io
 * 
 */
 
import processing.serial.*;
import java.awt.Rectangle;
import java.io.BufferedWriter;
import java.io.FileWriter;

final String FULL_DATASTREAM_RECORDING_FILENAME = "arduino_accel.csv";

final color XCOLOR = color(255, 61, 0, 200);
final color YCOLOR = color(73, 164, 239, 200);
final color ZCOLOR = color(255, 183, 0, 200);
final color [] SENSOR_VALUE_COLORS = { XCOLOR, YCOLOR, ZCOLOR };
final color DEFAULT_BACKGROUND_COLOR = color(44, 42, 41);
final int DISPLAY_TIMEWINDOW_MS = 1000 * 30; // 30 secs. You can change this to view more data


// Make sure to change this! If you're not sure what port your Arduino is using
// Run this Processing sketch and look in the console, then change the number accordingly
final int ARDUINO_SERIAL_PORT_INDEX = 0; 

ArrayList<AccelSensorData> _displaySensorData =  new ArrayList<AccelSensorData>(); // sensor data displayed to screen
PrintWriter _printWriterAllData;

// The serial port is necessary to read data in from the Arduino
Serial _serialPort;

long _currentXMin; // the far left x-axis value on the graph
Rectangle _legendRect; // location and drawing area of the legend
boolean _dynamicYAxis = true;
int _minSensorVal = 0;
int _maxSensorVal = 1023;

void setup() {
  size(1024, 576);

  // Print to console all the available serial ports
  String [] serialPorts = getAndPrintSerialPortInfo();
  
  if(serialPorts.length <= 0){
    println("You appear to have *ZERO* active serial ports. Make sure your Arduino is plugged in. Exiting...");
    exit();
  }else if(ARDUINO_SERIAL_PORT_INDEX > serialPorts.length){
    println("You set ARDUINO_SERIAL_PORT_INDEX = " + ARDUINO_SERIAL_PORT_INDEX + "; however, you only have " +
            serialPorts.length + " total serial ports.");
    println("Please make sure your Arduino is plugged in. Then update ARDUINO_SERIAL_PORT_INDEX to the appropriate index.");
    println("Exiting...");
    exit();
    return;
  }
  
  // Open the serial port
  try{
    println("Attempting to initialize the serial port at index " + ARDUINO_SERIAL_PORT_INDEX);
    println("This index corresponds to serial port " + serialPorts[ARDUINO_SERIAL_PORT_INDEX]);
    _serialPort = new Serial(this, serialPorts[ARDUINO_SERIAL_PORT_INDEX], 9600);
  }catch(Exception e){
    println("Serial port exception: " + e);
    e.printStackTrace();
    exit();
    return;
  }

  if(_serialPort == null){
    println("Could not initialize the serial port at " + ARDUINO_SERIAL_PORT_INDEX + ". Exiting...");
    exit();
    return;
  }
  
  // Don't generate a serialEvent() unless you get a newline character:
  _serialPort.bufferUntil('\n');

  _currentXMin = System.currentTimeMillis() - DISPLAY_TIMEWINDOW_MS;

  int legendHeight = 60;
  int legendWidth = 200;
  int legendXBuffer = 10;
  int legendYBuffer = 5;
  
  //_legendRect = new Rectangle(width - legendWidth - legendXBuffer, legendYBuffer, legendWidth, legendHeight); // legend at top-right
  _legendRect = new Rectangle(legendXBuffer, legendYBuffer, legendWidth, legendHeight); // legend at top-left
  
  File file = new File(FULL_DATASTREAM_RECORDING_FILENAME); 
     
  try {
    // We save all incoming sensor data to a file (by appending)
    // Appending text to a file: 
    //  - https://stackoverflow.com/questions/17010222/how-do-i-append-text-to-a-csv-txt-file-in-processing
    //  - https://docs.oracle.com/javase/7/docs/api/java/io/FileWriter.html
    //  - Use sketchPath(string) to store in local sketch folder: https://stackoverflow.com/a/36531925
    _printWriterAllData = new PrintWriter(new BufferedWriter(new FileWriter(file, true)));
  }catch (IOException e){
    e.printStackTrace();
  }

  noLoop(); // doesn't automatically loop over draw()
}

void draw() {
  
  background(DEFAULT_BACKGROUND_COLOR);
 
  //println("Drawing! _displaySensorData.size()=" + _displaySensorData.size() + " _timeWindowMs=" + _timeWindowMs);
  for (int i = 1; i < _displaySensorData.size(); i++) {
    AccelSensorData lastAccelSensorData = _displaySensorData.get(i - 1);
    AccelSensorData curAccelSensorData = _displaySensorData.get(i);

    drawSensorLine(XCOLOR, lastAccelSensorData.timestamp, lastAccelSensorData.x, curAccelSensorData.timestamp, curAccelSensorData.x);
    drawSensorLine(YCOLOR, lastAccelSensorData.timestamp, lastAccelSensorData.y, curAccelSensorData.timestamp, curAccelSensorData.y);
    drawSensorLine(ZCOLOR, lastAccelSensorData.timestamp, lastAccelSensorData.z, curAccelSensorData.timestamp, curAccelSensorData.z);
  }

  drawLegend(_legendRect);
}

/**
 * Prints information about the serial port and returns a list of all available serial ports
 */
String[] getAndPrintSerialPortInfo(){
  println("** All Available Serial Ports **");
  String[] listOfSerialPorts = Serial.list();
  printArray(listOfSerialPorts);
  println("** END SERIAL PORT LIST**");
  println("Make sure to change ARDUINO_SERIAL_PORT_INDEX to the correct port number!");
  
  if(listOfSerialPorts.length > 0){
    String firstPortName = listOfSerialPorts[0];
    println("For example, if your Arduino is on port " + firstPortName + 
            " then you would set ARDUINO_SERIAL_PORT_INDEX = " + 0);
  }
  return listOfSerialPorts;
}

/**
 * Converts a sensor value to a y-pixel value and returns the y-pixel value
 */
float getYPixelFromSensorVal(int sensorVal) {
  return map(sensorVal, _minSensorVal, _maxSensorVal, 0, height);
}

/**
 * Converts a timestamp value to an x-pixel value and returns the x-pixel value
 */
float getXPixelFromTimestamp(long timestamp) {
  return (timestamp - _currentXMin) / (float)DISPLAY_TIMEWINDOW_MS * width;
}

/**
 * Draws a sensor line with the given color
 */
void drawSensorLine(color col, long timestamp1, int sensorVal1, long timestamp2, int sensorVal2) {
  stroke(col);
  strokeWeight(2);
  float xLastPixelVal = getXPixelFromTimestamp(timestamp1);
  float yLastPixelVal = getYPixelFromSensorVal(sensorVal1);
  float xCurPixelVal = getXPixelFromTimestamp(timestamp2);
  float yCurPixelVal = getYPixelFromSensorVal(sensorVal2); 
  line(xLastPixelVal, yLastPixelVal, xCurPixelVal, yCurPixelVal);
}

/**
 * Draws the graph legend, which is dynamic based on the current sensor values
 */
void drawLegend(Rectangle legendRect) {
  if(_displaySensorData.size() <= 0){
    println("Can't draw legend. No data");
    return;
  }
  
  color textColor = color(255, 255, 255, 128);
  stroke(textColor);
  strokeWeight(1);
  noFill();
  rect(legendRect.x, legendRect.y, legendRect.width, legendRect.height);

  // Setup dimension calculations for legend times
  int yBuffer = 4;
  int xBuffer = 4;
  int numLegendItems = 3;
  float legendItemHeight = (legendRect.height - (numLegendItems * yBuffer)) / (float)numLegendItems;  
  String [] legendStrs = { "X", "Y", "Z" };
  textSize(legendItemHeight);
  float strHeight = textAscent() + textDescent();
  float xLegendItemPos = legendRect.x + xBuffer;
  float yLegendItemPos = legendRect.y + strHeight - textDescent();
  AccelSensorData accelSensorData = _displaySensorData.get(_displaySensorData.size() - 1);
  int [] accelSensorVals = accelSensorData.getSensorValues();
  float strWidth = textWidth("X");
  float maxValStrWidth = textWidth(Integer.toString(_maxSensorVal));
  float xBar = xLegendItemPos + strWidth + xBuffer;
  float maxBarSize = legendRect.width - (xBuffer + strWidth + 3 * xBuffer + maxValStrWidth);
  
  // Draw each legend item
  for (int i = 0; i < legendStrs.length; i++) {
    String legendStr = legendStrs[i];
    fill(textColor);
    text(legendStr, xLegendItemPos, yLegendItemPos);

    // draw dynamic legend values
    float barWidth = map(accelSensorVals[i], _minSensorVal, _maxSensorVal, 0, maxBarSize);
    fill(SENSOR_VALUE_COLORS[i]);
    noStroke();
    rect(xBar, yLegendItemPos - strHeight + textDescent() + yBuffer, barWidth, legendItemHeight - yBuffer);
    float xSensorTextLoc = xBar + barWidth + xBuffer;
    text(Integer.toString(accelSensorVals[i]), xSensorTextLoc, yLegendItemPos);
    yLegendItemPos += legendItemHeight + yBuffer;
  }
}
 //<>//
/**
 * Called automatically when there is data on the serial port
 * See: https://processing.org/reference/libraries/serial/serialEvent_.html
 */
void serialEvent (Serial myPort) {
  long currentTimestampMs = System.currentTimeMillis();
  _currentXMin = currentTimestampMs - DISPLAY_TIMEWINDOW_MS;

  try {
    // Grab the data off the serial port. See: 
    // https://processing.org/reference/libraries/serial/index.html
    String inString = trim(_serialPort.readStringUntil('\n'));
    println(inString);

    if (inString != null) {
      int [] data;

      // Our parser can handle either csv strings or just one float per line
      if (inString.contains(",")) {
        String [] strData = split(inString, ',');
        data = new data[strData.length];
        for(int i=0; i<strData.length; i++){
          data[i] = int(strData[i]); 
        }
      } else {
        data = new int[] { int(inString) };
      }

      printArray(data);
      AccelSensorData accelSensorData = new AccelSensorData(currentTimestampMs, data[0], data[1], data[2], data[3]);
      checkAndSetNewMinMaxSensorValues(accelSensorData);
      
      _displaySensorData.add(accelSensorData);

      // Remove data that is no longer relevant to be displayed
      while(_displaySensorData.get(0).timestamp < _currentXMin){
        _displaySensorData.remove(0);
      }
      println("_displaySensorData.length: " + _displaySensorData.size());
      _printWriterAllData.println(accelSensorData.toCsvString());
      
      redraw();
    }
  }
  catch(Exception e) {
    println(e);
  }
}

void checkAndSetNewMinMaxSensorValues(AccelSensorData accelSensorData){
  int min = min(accelSensorData.x, accelSensorData.y, accelSensorData.z);
  int max = max(accelSensorData.x, accelSensorData.y, accelSensorData.z);
  if(min < _minSensorVal){
    _minSensorVal = min; 
  }else{
    println("Min: " + min + " _minSensorVal: " + _minSensorVal);
  }
  
  if(max > _maxSensorVal){
    _maxSensorVal = max; 
  }
  
  println("Min: " + _minSensorVal + " max: " + _maxSensorVal);
}

// Class for the accelerometer data
class AccelSensorData {
  public final static String CSV_HEADER = "Processing Timestamp (ms), Arduino Timestamp (ms), X, Y, Z";
  
  public int x;
  public int y;
  public int z;
  public long timestamp;
  public long arduinoTimestamp;

  public AccelSensorData(long timestamp, long arduinoTimestamp, int x, int y, int z) {
    this.timestamp = timestamp;
    this.arduinoTimestamp = arduinoTimestamp;
    this.x = x;
    this.y = y;
    this.z = z;
  }
  
  // Creates a dynamic array on every call
  public int[] getSensorValues(){
    return new int[] { this.x, this.y, this.z };
  }
  
  public String toCsvHeaderString(){
    return CSV_HEADER;
  }
  
  public String toCsvString(){
    return String.format("%d, %d, %d, %d, %d", this.timestamp, this.arduinoTimestamp, this.x, this.y, this.z);
  }

  public String toString() { 
    return String.format("timestamp=%d x=%d y=%d z=%d", this.timestamp, this.x, this.y, this.z);
  }
}
