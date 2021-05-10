
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

// status bar
boolean _drawStatusBar = true; // change to show/hide status bar
int _graphHeight = SCREEN_HEIGHT;

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
    int _ptSize = 5; 

  public:
    GraphLine(int bufferSize, PointSymbol symbol) 
    {
      _circularBuffer = new int[bufferSize];
      _bufferSize = bufferSize;
      _symbol = symbol;

      // initialize all the readings to 0:
      for (int i = 0; i < bufferSize; i++) {
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

    void addData(int yVal){
      _circularBuffer[_curWriteIndex++] = yVal;
    
      // Set the circular buffer index back to zero when it reaches the 
      // the end of the buffer
      if(_curWriteIndex >= _bufferSize){
        _curWriteIndex = 0;
      }
    }

    void draw(const Adafruit_SSD1306& disp){
      // Draw the line graph based on data in _circularBuffer
      int xPos = 0; 
      int step = _ptSize;
      for (int i = _curWriteIndex; i < _bufferSize; i += step){
        int yPos = _circularBuffer[i];
        drawValue(disp, xPos, yPos);
        xPos += step;
      }
      
      for(int i = 0; i < _curWriteIndex; i += step){
        int yPos = _circularBuffer[i];
        drawValue(disp, xPos, yPos);
        xPos += step;
      }
    }

    void drawValue(const Adafruit_SSD1306& disp, int x, int y){
      // Ugh, ran into major problem with switch statements
      // Need to have brackets around cases
      // See: https://stackoverflow.com/questions/92396/why-cant-variables-be-declared-in-a-switch-statement
      int halfSize = _ptSize / 2;
      Serial.println((String)"drawValue " + _symbol + " triangle: " + TRIANGLE);

      if(_symbol == TRIANGLE){
        Serial.println("THIS IS A TRIANGLE");
      }
      
      switch(_symbol) {
         case CIRCLE:
         {
            Serial.println("IN CIRCLE!!!");
            disp.drawCircle(x, y, halfSize, SSD1306_WHITE);
            break; 
         }
         case SQUARE:
         {
            Serial.println("IN SQUARE!!!");
            int topLeftX = x - halfSize;
            int topLeftY = y - halfSize;
            disp.drawRect(topLeftX, topLeftY, _ptSize, _ptSize, SSD1306_WHITE);
            break; 
         }
         case TRIANGLE:
         {
            Serial.println("SWITCH STATEMENT TRIANGLE");
            int x0 = x - halfSize;
            int y0 = y + halfSize;
            
            int x1 = x;
            int y1 = y - halfSize;
            
            int x2 = x + halfSize;
            int y2 = y0;

            Serial.println((String)"x0=" + x0 + " y0=" + y0 + "x1=" + x1 + " y1=" + y1 + "x2=" + x2 + " y2=" + y2);
            
            disp.drawTriangle(x0, y0, x1, y1, x2, y2, SSD1306_WHITE);
            break;
         }
         case POINT:
         {
            Serial.println("IN POINT!!!");
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

  public:
    ScrollingLineGraph(int displayWidth, int numLines, PointSymbol symbols[]) 
    {
      _graphLines = new GraphLine*[numLines];
      _numLines = numLines;

      for (int i = 0; i < numLines; i++) {
        _graphLines[i] = new GraphLine(displayWidth, symbols[i]);
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

    void addData(int graphLineIndex, int newYPos){
      if(graphLineIndex > _numLines){
        return;
      }
      
      _graphLines[graphLineIndex]->addData(newYPos);
    }

    void draw(const Adafruit_SSD1306& disp){
      for (int i = 0; i < _numLines; i++) {
        _graphLines[i]->draw(disp);
      }
    }
};
//
const int NUM_GRAPH_LINES = 3;
const PointSymbol GRAPH_SYMBOLS[NUM_GRAPH_LINES] = {CIRCLE, SQUARE, TRIANGLE};
ScrollingLineGraph _scrollingLineGraph(32, NUM_GRAPH_LINES, GRAPH_SYMBOLS);

//
//const int NUM_GRAPH_LINES = 2;
//const PointSymbol GRAPH_SYMBOLS[NUM_GRAPH_LINES] = {CIRCLE, TRIANGLE};
//ScrollingLineGraph _scrollingLineGraph(SCREEN_WIDTH, NUM_GRAPH_LINES, GRAPH_SYMBOLS);

//const int NUM_GRAPH_LINES = 1;
//const PointSymbol GRAPH_SYMBOLS[NUM_GRAPH_LINES] = {TRIANGLE};
//ScrollingLineGraph _scrollingLineGraph(SCREEN_WIDTH, NUM_GRAPH_LINES, GRAPH_SYMBOLS);

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

  if(_drawStatusBar){
    _graphHeight = SCREEN_HEIGHT - 10;
  }

  _fpsStartTimeStamp = millis();
}

void loop() {
  // Clear the display on each frame. We draw from the _circularBuffer
  _display.clearDisplay();

  // Read and store the analog data into a circular buffer
  int analogVal = analogRead(ANALOG_INPUT_PIN1);
  int lineHeight = map(analogVal, MIN_ANALOG_INPUT, MAX_ANALOG_INPUT, 0, _graphHeight);
  int yPos = _display.height() - lineHeight;
  _scrollingLineGraph.addData(0, yPos);

  analogVal = analogRead(ANALOG_INPUT_PIN2);
  lineHeight = map(analogVal, MIN_ANALOG_INPUT, MAX_ANALOG_INPUT, 0, _graphHeight);
  yPos = _display.height() - lineHeight;
  _scrollingLineGraph.addData(1, yPos);
//
//
//  analogVal = analogRead(ANALOG_INPUT_PIN3);
//  lineHeight = map(analogVal, MIN_ANALOG_INPUT, MAX_ANALOG_INPUT, 0, _graphHeight);
//  yPos = _display.height() - lineHeight;
//  _scrollingLineGraph.addData(2, yPos);

  if(_drawStatusBar){
    drawStatusBar(analogVal);
  }
  
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
 * Draws the status bar at top of screen with fps and analog value
 */
void drawStatusBar(int analogVal) {

   // erase status bar by drawing all black
  _display.fillRect(0, 0, _display.width(), 8, SSD1306_BLACK); 
  
  // Draw current val
  _display.setCursor(0, 0);
  _display.print(analogVal);

  // Draw frame count
  int16_t x1, y1;
  uint16_t w, h;
  _display.getTextBounds("XX.XX fps", 0, 0, &x1, &y1, &w, &h);
  _display.setCursor(_display.width() - w, 0);
  _display.print(_fps);
  _display.print(" fps");
}
