/**
 * Records accelerometer-based gestures. Works with the Arduino program
 * ADXL335GestureRecorder.ino (or something similar that provides a CSV input 
 * stream on the serial port of "timestamp, x, y, z")
 *   
 * By Jon Froehlich
 * http://makeabilitylab.io
 * 
 * TODO:
 * - [done] Hit space to start and stop. When starting, counts down from 3, 2, 1
 * - [done] Status is shown in big letters
 * - [done] Saves both full sensor stream plus presegmented?
 * - [done] Maybe save Arduino time, Processing timestamp too?
 * - [done] Write out save file name after gesture finished recording? Maybe put in GestureAnnotation overlay
 * Future ideas:
 * - Shows capture snapshot?
 * - GUI to select which gesture to record
 * - Draw axes? If so, need rectangular boundary to draw data < full width/height of window
 * - When the window is closed (or closing), flush the printwriter and close it (to ensure all of the fulldata.csv is saved)?
 */
 
import processing.serial.*;
import java.awt.Rectangle;
import java.io.BufferedWriter;
import java.io.FileWriter;

final String GESTURE_DIR_NAME = "Gestures";
final String FULL_DATASTREAM_RECORDING_FILENAME = "fulldatastream.csv";

final int YAXIS_MIN = 0; // minimum y value to graph
final int YAXIS_MAX = 1023; // maximum y value to graph
final color XCOLOR = color(255, 61, 0, 200);
final color YCOLOR = color(73, 164, 239, 200);
final color ZCOLOR = color(255, 183, 0, 200);
final color [] SENSOR_VALUE_COLORS = { XCOLOR, YCOLOR, ZCOLOR };
final color DEFAULT_BACKGROUND_COLOR = color(44, 42, 41);
final color RECORDING_BACKGROUND_COLOR = color(80, 0, 0);
final int DISPLAY_TIMEWINDOW_MS = 1000 * 30; // 30 secs

final int MIN_SENSOR_VAL = 0;
final int MAX_SENSOR_VAL = 1023;
final int ARDUINO_SERIAL_PORT_INDEX = 3; // our serial port index

final int NUM_SAMPLES_TO_RECORD_PER_GESTURE = 5;

final String [] GESTURES = { "Backhand Tennis", "Forehand Tennis", "Underhand Bowling", 
                             "Baseball Throw", "Midair Clockwise 'O'", "At Rest", "Midair Counter-clockwise 'O'",
                             "Midair Zorro 'Z'", "Midair 'S'", "Shake", "Custom" };
int _curGestureIndex = 0;
HashMap<String, Integer> _mapGestureNameToRecordedCount = new HashMap<String, Integer>(); // tracks recorded gesture counts
ArrayList<AccelSensorData> _displaySensorData =  new ArrayList<AccelSensorData>(); // sensor data displayed to screen
ArrayList<GestureRecording> _gestureRecordings = new ArrayList<GestureRecording>(); // sensor data to dump to file
PrintWriter _printWriterAllData;

// The serial port is necessary to read data in from the Arduino
Serial _serialPort;

long _currentXMin; // the far left x-axis value on the graph

final int COUNTDOWN_TIME_MS = 4 * 1000; // how long to show a countdown timer before recording a gesture
long _timestampStartCountdownMs = -1; // timestamp of when the countdown timer was started
boolean _recordingGesture = false; // true if we are currently recording a gesture, false otherwise

Rectangle _legendRect; // location and drawing area of the legend

void setup() {
  size(1024, 576);

  //
  // Print to console all the available serial ports
  print("**All Available Serial Ports**");
  printArray(Serial.list());
  print("Make sure to change ARDUINO_SERIAL_PORT_INDEX to the correct port number!");

  // Open the serial port
  _serialPort = new Serial(this, Serial.list()[ARDUINO_SERIAL_PORT_INDEX], 9600);

  // Don't generate a serialEvent() unless you get a newline character:
  _serialPort.bufferUntil('\n');

  _currentXMin = System.currentTimeMillis() - DISPLAY_TIMEWINDOW_MS;

  int legendHeight = 60;
  int legendWidth = 200;
  int legendXBuffer = 10;
  int legendYBuffer = 5;
  
  //_legendRect = new Rectangle(width - legendWidth - legendXBuffer, legendYBuffer, legendWidth, legendHeight); // legend at top-right
  _legendRect = new Rectangle(legendXBuffer, legendYBuffer, legendWidth, legendHeight); // legend at top-left
  
  String filenameNoPath = FULL_DATASTREAM_RECORDING_FILENAME;
  File fDir = new File(sketchPath(GESTURE_DIR_NAME));
  if(!fDir.exists()){
    fDir.mkdirs(); 
  }
  File file = new File(fDir, filenameNoPath); 
     
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
  if (_recordingGesture) {
    background(RECORDING_BACKGROUND_COLOR);
  } else {
    background(DEFAULT_BACKGROUND_COLOR);
  }

  //if(_displaySensorData.size() > 0){
  //  AccelSensorData accelSensorData = _displaySensorData.get(_displaySensorData.size() - 1);
  //  float circleSize = map(accelSensorData.x, MIN_SENSOR_VAL, MAX_SENSOR_VAL, 0, width / 2.0);
  //  println("accelSensorData.x=" + accelSensorData.x + " circleSize=" + circleSize);
  //  circle(50,50,circleSize);
  //}

  //println("Drawing! _displaySensorData.size()=" + _displaySensorData.size() + " _timeWindowMs=" + _timeWindowMs);
  for (int i = 1; i < _displaySensorData.size(); i++) {
    AccelSensorData lastAccelSensorData = _displaySensorData.get(i - 1);
    AccelSensorData curAccelSensorData = _displaySensorData.get(i);

    drawSensorLine(XCOLOR, lastAccelSensorData.timestamp, lastAccelSensorData.x, curAccelSensorData.timestamp, curAccelSensorData.x);
    drawSensorLine(YCOLOR, lastAccelSensorData.timestamp, lastAccelSensorData.y, curAccelSensorData.timestamp, curAccelSensorData.y);
    drawSensorLine(ZCOLOR, lastAccelSensorData.timestamp, lastAccelSensorData.z, curAccelSensorData.timestamp, curAccelSensorData.z);
  }

  if (_timestampStartCountdownMs != -1) {
    updateAndDrawCountdownTimer();
  }else{
    drawInstructions(); 
  }

  drawGestureRecordingAnnotations();
  drawLegend(_legendRect);
  //drawRecordedGesturesStatus();
}

/**
 * Converts a sensor value to a y-pixel value and returns the y-pixel value
 */
float getYPixelFromSensorVal(int sensorVal) {
  return map(sensorVal, MIN_SENSOR_VAL, MAX_SENSOR_VAL, 0, height);
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
  float maxValStrWidth = textWidth(Integer.toString(MAX_SENSOR_VAL));
  float xBar = xLegendItemPos + strWidth + xBuffer;
  float maxBarSize = legendRect.width - (xBuffer + strWidth + 3 * xBuffer + maxValStrWidth);
  
  // Draw each legend item
  for (int i = 0; i < legendStrs.length; i++) {
    String legendStr = legendStrs[i];
    fill(textColor);
    text(legendStr, xLegendItemPos, yLegendItemPos);

    // draw dynamic legend values
    float barWidth = map(accelSensorVals[i], MIN_SENSOR_VAL, MAX_SENSOR_VAL, 0, maxBarSize);
    fill(SENSOR_VALUE_COLORS[i]);
    noStroke();
    rect(xBar, yLegendItemPos - strHeight + textDescent() + yBuffer, barWidth, legendItemHeight - yBuffer);
    float xSensorTextLoc = xBar + barWidth + xBuffer;
    text(Integer.toString(accelSensorVals[i]), xSensorTextLoc, yLegendItemPos);
    yLegendItemPos += legendItemHeight + yBuffer;
  }
}

/**
 * Draws an on-screen annotation for recently recorded annotations
 */
void drawGestureRecordingAnnotations() {
  
  while(_gestureRecordings.size() > 0 && 
        _gestureRecordings.get(0).hasGestureCompleted() &&
        _gestureRecordings.get(0).endTimestamp < _currentXMin){
    _gestureRecordings.remove(0);
  }
  
  if(_gestureRecordings.size() <= 0) { return; }
  
  for(GestureRecording gestureRecording : _gestureRecordings){
    textSize(10);
    fill(255);
    stroke(255);
    strokeWeight(1);
    
    String strGesture = "Gesture " + (gestureRecording.hasGestureCompleted() ? "Completed:" : "Active:") 
                        + "\n" + gestureRecording.name;
    float xPixelStartGesture = getXPixelFromTimestamp(gestureRecording.startTimestamp);
    line(xPixelStartGesture, 0, xPixelStartGesture, height);
    text(strGesture, xPixelStartGesture + 2, 20);  
    
    if(gestureRecording.hasGestureCompleted()){
      String strEndGesture = "Gesture End: \n" + gestureRecording.name;
      float xPixelEndGesture = getXPixelFromTimestamp(gestureRecording.endTimestamp);
      
      noStroke();
      fill(255, 255, 255, 30);
      rect(xPixelStartGesture, 0, xPixelEndGesture - xPixelStartGesture, height);
      
      stroke(255);
      line(xPixelEndGesture, 0, xPixelEndGesture, height);
      
      textSize(10);
      fill(255);
      
      // null check on savedfilename
      // this is because a gesture might be completed but the save to file might not be done
      // see: https://github.com/jonfroehlich/CSE599Sp2019/issues/1
      if(gestureRecording.savedFilename != null){
        text(gestureRecording.savedFilename, xPixelStartGesture + 2, 50); 
      }
    }
  }
  
}

/**
 * Writes out basic instructions for the user
 */
void drawInstructions(){
  textSize(30);
  
  String strInstructions = "";
  
  if(_curGestureIndex < GESTURES.length){
    int sampleNum = getNumGesturesRecordedWithName(GESTURES[_curGestureIndex]) + 1;
    strInstructions = "Hit spacebar to record Sample " + sampleNum + "/" + NUM_SAMPLES_TO_RECORD_PER_GESTURE 
                              + " of gesture '" + GESTURES[_curGestureIndex] + "'";
  }else{
    strInstructions = "You did it! Gesture recording completed!";
  }
  
  float strWidth = textWidth(strInstructions);
  float strHeight = textAscent() + textDescent();

  fill(255);
  text(strInstructions, width / 2.0 - strWidth / 2.0, height / 4.0 + strHeight / 2.0 - textDescent());
}

/**
 * Controls the gesture recording timer and recording logic (i.e., sets recording flag)
 */
void updateAndDrawCountdownTimer() {
  
  fill(255);
  
  long curTimestampMs = System.currentTimeMillis();
  long elapsedTimeMs = curTimestampMs - _timestampStartCountdownMs;
  int countdownTimeSecs = (int)((COUNTDOWN_TIME_MS - elapsedTimeMs) / 1000.0);

  // draw center of screen
  String str = ""; 
  if (countdownTimeSecs <= 0) {
    textSize(40);
    float strHeight = textAscent() + textDescent();
    int sampleNum = getNumGesturesRecordedWithName(GESTURES[_curGestureIndex]) + 1;
    str = "Recording Sample " + sampleNum + "/" + NUM_SAMPLES_TO_RECORD_PER_GESTURE + " of '" + GESTURES[_curGestureIndex]  + "'!";

    if (!_recordingGesture) {
      _recordingGesture = true;
      GestureRecording gestureRecording = new GestureRecording(GESTURES[_curGestureIndex], curTimestampMs);
      _gestureRecordings.add(gestureRecording);
    }
    float strWidth = textWidth(str);
    text(str, width / 2.0 - strWidth / 2.0, height * 0.75 + strHeight / 2.0 - textDescent());
  } else {
    textSize(80);
    float strHeight = textAscent() + textDescent();
    str = String.format("%d", countdownTimeSecs);
    float strWidth = textWidth(str);
    text(str, width / 2.0 - strWidth / 2.0, height / 2.0 + strHeight / 2.0 - textDescent());
  }
}

/**
 * Called automatically when a key is pressed. We use this to capture the spacebar keypress (used to start/stop)
 */
void keyPressed() {
  if (key == ' ') { 
    if (_recordingGesture) {
      // save gesture!
      _recordingGesture = false;
      _timestampStartCountdownMs = -1;
      long currentTimestampMs = System.currentTimeMillis();
      GestureRecording curGestureRecording = _gestureRecordings.get(_gestureRecordings.size() - 1);
      curGestureRecording.endTimestamp = currentTimestampMs;
      curGestureRecording.save();
      
      if(!_mapGestureNameToRecordedCount.containsKey(curGestureRecording.name)){ //<>//
        _mapGestureNameToRecordedCount.put(curGestureRecording.name, 1);
      }else{
        int curRecordingCntForGesture = (int)_mapGestureNameToRecordedCount.get(curGestureRecording.name);
        int newRecordingCntForGesture = curRecordingCntForGesture + 1;
        _mapGestureNameToRecordedCount.put(curGestureRecording.name, newRecordingCntForGesture);
        
        if(newRecordingCntForGesture >= NUM_SAMPLES_TO_RECORD_PER_GESTURE){
           _curGestureIndex++; 
        }
      }
       
    } else {
      if(_curGestureIndex < GESTURES.length){
        // if there are still gestures left to record, start countdown timer
        _timestampStartCountdownMs = System.currentTimeMillis();
      }
    }
  }
}

/**
 * Convenience method that returns the number of gestures recoreded with the given name
 */
int getNumGesturesRecordedWithName(String name){
  return _mapGestureNameToRecordedCount.containsKey(name) ? (int)_mapGestureNameToRecordedCount.get(name) : 0;
}

// Called automatically when there is data on the serial port
// See: https://processing.org/reference/libraries/serial/serialEvent_.html
void serialEvent (Serial myPort) {
  long currentTimestampMs = System.currentTimeMillis();
  _currentXMin = currentTimestampMs - DISPLAY_TIMEWINDOW_MS;

  try {
    // Grab the data off the serial port. See: 
    // https://processing.org/reference/libraries/serial/index.html
    String inString = trim(_serialPort.readStringUntil('\n'));
    //println(inString);

    if (inString != null) {
      int [] data;

      // Our parser can handle either csv strings or just one float per line
      if (inString.contains(",")) {
        data = int(split(inString, ','));
      } else {
        data = new int[] { int(inString) };
      }

      AccelSensorData accelSensorData = new AccelSensorData(currentTimestampMs, data[0], data[1], data[2], data[3]);
      
      if(_recordingGesture){
        GestureRecording curGestureRecording = _gestureRecordings.get(_gestureRecordings.size() - 1);
        curGestureRecording.listSensorData.add(accelSensorData);
      }
      
      _displaySensorData.add(accelSensorData);

      // Remove data that is no longer relevant to be displayed
      while(_displaySensorData.get(0).timestamp < _currentXMin){
        _displaySensorData.remove(0);
      }
     
      _printWriterAllData.println(accelSensorData.toCsvString());
      
      redraw();
    }
  }
  catch(Exception e) {
    println(e);
  }
}

class GestureRecording{
  public long startTimestamp;
  public long endTimestamp = -1;
  public String name;
  public String savedAbsolutePath;
  public String savedFilename;
  
  ArrayList<AccelSensorData> listSensorData =  new ArrayList<AccelSensorData>();
  
  public GestureRecording(String gestureName, long startTimestamp){
    this.name = gestureName;
    this.startTimestamp = startTimestamp;
  }
  
  public boolean hasGestureCompleted(){
    return this.endTimestamp != -1;
  }
  
  public void save(){
    long currentTimestampMs = System.currentTimeMillis();
    String filenameNoPath = this.name + "_" + currentTimestampMs + "_" +  this.listSensorData.size() +  ".csv";
    File fDir = new File(sketchPath(GESTURE_DIR_NAME));
    if(!fDir.exists()){
      fDir.mkdirs(); 
    }
    File file = new File(fDir, filenameNoPath); 
    
    try {
      PrintWriter printWriter = new PrintWriter(new BufferedWriter(new FileWriter(file, false)));
      printWriter.println(AccelSensorData.CSV_HEADER);
      for(AccelSensorData accelSensorData : listSensorData){
        printWriter.println(accelSensorData.toCsvString());
      }
      printWriter.flush();
      printWriter.close();
      this.savedAbsolutePath = file.getAbsolutePath();
      this.savedFilename = file.getName();
    }catch (IOException e){
      e.printStackTrace();
    }
  }
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
