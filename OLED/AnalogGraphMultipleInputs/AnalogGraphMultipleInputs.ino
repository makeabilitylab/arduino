
/**
 *  Draws a scrolling graph of a single analog input (set to A0) on the OLED display
 *
 *  By Jon E. Froehlich
 *  @jonfroehlich
 *  http://makeabilitylab.io
 *
 */

#include <Wire.h>
#include <SPI.h>

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET     4 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 _display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

const int ANALOG_INPUT_PIN1 = A0;
const int ANALOG_INPUT_PIN2 = A1;
const int ANALOG_INPUT_PIN3 = A2;

const int MIN_ANALOG_INPUT = 0;
const int MAX_ANALOG_INPUT = 1023;
const int DELAY_LOOP_MS = 5; // change to slow down how often to read and graph value

// for tracking fps
float _fps = 0;
unsigned long _frameCount = 0;
unsigned long _fpsStartTimeStamp = 0;

enum PointSymbol {
  CIRCLE,
  SQUARE,
  TRIANGLE,
  POINT,
};

class GraphLine{
  protected:
    int* _circularBuffer = NULL; // circular buffer
    int _bufferSize = -1;
    long _sampleTotal = 0; 
    int _curWriteIndex = 0; 
    enum PointSymbol _symbol = CIRCLE; 
    int _ptSize = 4; 

  public:
    GraphLine(int displayWidth, PointSymbol symbol) 
    {
      _bufferSize = displayWidth / _ptSize;      
      _circularBuffer = new int[_bufferSize];

      _symbol = symbol;

      // initialize all the readings to 0:
      for (int i = 0; i < _bufferSize; i++) {
        _circularBuffer[i] = 0;
      }
    }

    ~GraphLine(){
      if(_circularBuffer != NULL){
        delete [] _circularBuffer;
        _circularBuffer = NULL;
        _bufferSize = -1;
      }
    }

    PointSymbol getSymbol(){
      return _symbol;
    }

    int getSymbolSize(){
      return _ptSize;
    }

    int getCurrentValue(){
      int curValIndex = _curWriteIndex - 1;
      if(curValIndex < 0){
        curValIndex = _bufferSize - 1;
      }

      return _circularBuffer[curValIndex];
    }

    void addData(int yVal){
      _circularBuffer[_curWriteIndex++] = yVal;
    
      // Set the circular buffer index back to zero when it reaches the 
      // the end of the buffer
      if(_curWriteIndex >= _bufferSize){
        _curWriteIndex = 0;
      }
    }

    void draw(const Adafruit_SSD1306& disp, int yGraph, int heightGraph, int minY, int maxY){
      // Draw the line graph based on data in _circularBuffer
      int xPos = 0; 
      for (int i = _curWriteIndex; i < _bufferSize; i++){
        int yVal = _circularBuffer[i];

        // convert to yPos
        int lineHeight = map(yVal, minY, maxY, yGraph, heightGraph);
        int yPos = yGraph + heightGraph - lineHeight;
        
        //drawValue(disp, xPos, yPos);
        drawSymbol(disp, xPos, yPos, _ptSize, _symbol);
        xPos += _ptSize;
      }
      
      for(int i = 0; i < _curWriteIndex; i++){
        int yVal = _circularBuffer[i];

        // convert to yPos
        int lineHeight = map(yVal, minY, maxY, yGraph, heightGraph);
        int yPos = yGraph + heightGraph - lineHeight;
        
        drawSymbol(disp, xPos, yPos, _ptSize, _symbol);
        xPos += _ptSize;
      }
    }

    /**
     * Draws the symbol centered at the given x,y point
     */ 
    static void drawSymbol(const Adafruit_SSD1306& disp, int x, int y, int size, PointSymbol symbol){
      // Ugh, ran into major problem with switch statements
      // Need to have brackets around cases
      // See: https://stackoverflow.com/questions/92396/why-cant-variables-be-declared-in-a-switch-statement
      int halfSize = size / 2;
      
      switch(symbol) {
         case CIRCLE:
         {
            disp.drawCircle(x, y, halfSize, SSD1306_WHITE);
            break; 
         }
         case SQUARE:
         {
            int topLeftX = x - halfSize;
            int topLeftY = y - halfSize;
            disp.drawRect(topLeftX, topLeftY, size, size, SSD1306_WHITE);
            break; 
         }
         case TRIANGLE:
         {
            int x0 = x - halfSize;
            int y0 = y + halfSize;
            
            int x1 = x;
            int y1 = y - halfSize;
            
            int x2 = x + halfSize;
            int y2 = y0;

            disp.drawTriangle(x0, y0, x1, y1, x2, y2, SSD1306_WHITE);
            break;
         }
         case POINT:
         {
            disp.drawPixel(x, y, SSD1306_WHITE);
            break;
         }
         default:
         {
            Serial.println("SYMBOL NOT FOUND!!!");
            break;
         }
      }
    }
};

class ScrollingLineGraph{
  protected:
    GraphLine** _graphLines = NULL;
    int _numLines = -1;

    int _minY = 0;
    int _maxY = 1023;

    int _xGraph = 0; // in pixels
    int _yGraph = 0; // in pixels
    int _widthGraph = 128; // in pixels
    int _heightGraph = 64; // in pixels

  public:
    ScrollingLineGraph(int numLines, PointSymbol symbols[]) :
      ScrollingLineGraph(0, 0, 128, 64, numLines, symbols){
        // purposefully empty
      }
  
    ScrollingLineGraph(int xGraph, int yGraph, int graphWidth, int graphHeight, int numLines, PointSymbol symbols[]) 
    {
      _xGraph = xGraph;
      _yGraph = yGraph;
      _widthGraph = graphWidth;
      _heightGraph = graphHeight;
      
      _graphLines = new GraphLine*[numLines];
      _numLines = numLines;

      for (int i = 0; i < numLines; i++) {
        _graphLines[i] = new GraphLine(graphWidth, symbols[i]);
      }
    }

    ~ScrollingLineGraph(){
      if(_graphLines != NULL){
        // From https://stackoverflow.com/a/4194228/388117
        for (int i=0; i<_numLines; i++){
          delete[] _graphLines[i];
        }
        
        delete[] _graphLines;
        _graphLines = NULL;
        _numLines = -1;
      }      
    }

    int getMinY(){
      return _minY;
    }

    int getMaxY(){
      return _maxY;
    }

    void addData(int graphLineIndex, int newYPos){
      if(graphLineIndex > _numLines){
        return;
      }
      
      _graphLines[graphLineIndex]->addData(newYPos);
    }

    void draw(const Adafruit_SSD1306& disp){
      for (int i = 0; i < _numLines; i++) {
        _graphLines[i]->draw(disp, _yGraph, _heightGraph, _minY, _maxY);
      }
    }

    void drawLegend(const Adafruit_SSD1306& disp, int x, int y){
      int xCurPos = -1;
      int ySymbolPos = -1;
      int16_t xText, yText;
      uint16_t textWidth, textHeight;
      int symbolSize = 6;
      
      for (int i = 0; i < _numLines; i++) {
        PointSymbol ptSymbol = _graphLines[i]->getSymbol();
        //int symbolSize = _graphLines[i]->getSymbolSize();

        if(xCurPos == -1){
          xCurPos = x + symbolSize / 2;
        }

        if(ySymbolPos == -1){
          ySymbolPos = y + symbolSize / 2; 
        }
        
        GraphLine::drawSymbol(disp, xCurPos, ySymbolPos, symbolSize, ptSymbol);
        xCurPos += symbolSize;

        String strVal = (String)_graphLines[i]->getCurrentValue();
        disp.getTextBounds(strVal, 0, 0, &xText, &yText, &textWidth, &textHeight);
        disp.setCursor(xCurPos, 0);
        disp.print(strVal);
        xCurPos += textWidth + 5;
      }
    }

    /**
     * @brief Get the width of the graph
     * 
     * @return int Width of the shape in pixels
     */
    int getWidth(){
      return _widthGraph;
    }

    /**
     * @brief Get the height of the graph
     * 
     * @return int Height of the graph in pixels
     */
    int getHeight(){
      return _heightGraph;
    }

    /**
     * @brief Get the left (x) location of the graph
     * 
     * @return int The left location of the shape
     */
    int getLeft() {
      return _xGraph;
    }

    /**
     * @brief Get the right (x) location of the graph
     * 
     * @return int The right location (x) of the graph
     */
    int getRight() {
      return _xGraph + _widthGraph;
    }

    /**
     * @brief Get the bottom of the graph (y)
     * 
     * @return int 
     */
    int getBottom() {
      return _yGraph + _heightGraph;
    }

    /**
     * @brief Get the top of the graph
     * 
     * @return int 
     */
    int getTop() {
      return _yGraph;
    }
};


const int NUM_GRAPH_LINES = 3;
const PointSymbol GRAPH_SYMBOLS[NUM_GRAPH_LINES] = {CIRCLE, SQUARE, TRIANGLE};
ScrollingLineGraph _scrollingLineGraph(NUM_GRAPH_LINES, GRAPH_SYMBOLS);

void setup() {
  Serial.begin(9600);

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if (!_display.begin(SSD1306_SWITCHCAPVCC, 0x3D)) { // Address 0x3D for 128x64
    Serial.println(F("SSD1306 allocation failed"));
    for (;;); // Don't proceed, loop forever
  }

  // Clear the buffer
  _display.clearDisplay();

  _display.setTextSize(1);
  _display.setTextColor(WHITE, BLACK);
  _display.setCursor(0, 0);
  _display.println("Screen initialized!");
  _display.display();
  delay(500);
  _display.clearDisplay();

  _fpsStartTimeStamp = millis();
}

void loop() {
  // Clear the display on each frame. 
  _display.clearDisplay();

  // Read and store the analog data into a circular buffer
  int analogVal = analogRead(ANALOG_INPUT_PIN1);
  _scrollingLineGraph.addData(0, analogVal);

  delay(1);
  analogVal = analogRead(ANALOG_INPUT_PIN2);
  _scrollingLineGraph.addData(1, analogVal);

  delay(1);
  analogVal = analogRead(ANALOG_INPUT_PIN3);
  _scrollingLineGraph.addData(2, analogVal);

  if(_drawFps){
    drawFps();
  }

  _scrollingLineGraph.drawLegend(_display, 0, 0);
  _scrollingLineGraph.draw(_display);
  
  _display.display();
  
  calcFrameRate();
  
  delay(DELAY_LOOP_MS);
}

/**
 * Call this every frame to calculate frame rate
 */
void calcFrameRate() {
    
  unsigned long elapsedTime = millis() - _fpsStartTimeStamp;
  _frameCount++;
  if (elapsedTime > 1000) {
    _fps = _frameCount / (elapsedTime / 1000.0);
    _fpsStartTimeStamp = millis();
    _frameCount = 0;
  }
}

/**
 * Draws the status bar at top of screen with fps
 */
void drawFps() {

  // Draw frame count
  int16_t x1, y1;
  uint16_t w, h;
  _display.getTextBounds("XX.X fps", 0, 0, &x1, &y1, &w, &h);
  _display.setCursor(_display.width() - w, 0);
  _display.print(_fps, 1);
  _display.print(" fps");
}
