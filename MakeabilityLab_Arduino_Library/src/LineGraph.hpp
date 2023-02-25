/**
 * A basic line graph library for monochrome OLEDs using Adafruit_SSD1306.h, 
 * which is Adafruit's library based on the SSD1306 drivers. 
 * 
 * By Jon E. Froehlich
 * @jonfroehlich
 * https://jonfroehlich.github.io/
 * https://makeabilitylab.github.io/physcomp/advancedio/oled.html 
 * 
 */

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

class MultiValueScrollingLineGraph{
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
    MultiValueScrollingLineGraph(int numLines, PointSymbol symbols[]) :
      MultiValueScrollingLineGraph(0, 0, 128, 64, numLines, symbols){
        // purposefully empty
      }
  
    MultiValueScrollingLineGraph(int xGraph, int yGraph, int graphWidth, int graphHeight, int numLines, PointSymbol symbols[]) 
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

    ~MultiValueScrollingLineGraph(){
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
        xCurPos += textWidth + 7;
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