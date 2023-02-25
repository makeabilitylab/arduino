/**
 * A scrolling line graph multi-value library for monochrome OLEDs using 
 * Adafruit_SSD1306.h, which is Adafruit's library based on the SSD1306 drivers. 
 * 
 * Video demo:
 * https://youtu.be/EJDewh8Y1TY
 * 
 * See also:
 * LineGraph.hpp
 * ScrollingLineGraph.hpp
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
    int _ptSize = DEFAULT_SYMBOL_SIZE; 
    boolean _isBufferFull = false;

  public:
    const int DEFAULT_SYMBOL_SIZE = 4;

    GraphLine(int displayWidth, PointSymbol symbol) 
      : GraphLine(displayWidth, symbol, DEFAULT_SYMBOL_SIZE)
    {
      // empty on purpose
    }

    GraphLine(int displayWidth, PointSymbol symbol, int symbolSize) 
    {
      _ptSize = symbolSize;
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

    int getMinMaxYValue(int &min, int &max){
      int maxSearchRange = _bufferSize;
      if(!_isBufferFull){
        maxSearchRange = _curWriteIndex;
      }

      int minYInBuffer = 32767;
      int maxYInBuffer = -32768;
      for(int i = 0; i < maxSearchRange; i++){
        if(_circularBuffer[i] < minYInBuffer){
          minYInBuffer = _circularBuffer[i];
        }

        if(_circularBuffer[i] > maxYInBuffer){
          maxYInBuffer = _circularBuffer[i];
        }
      }
      min = minYInBuffer;
      max = maxYInBuffer;
    }

    void addData(int yVal){
      _circularBuffer[_curWriteIndex++] = yVal;
    
      // Set the circular buffer index back to zero when it reaches the 
      // the end of the buffer
      if(_curWriteIndex >= _bufferSize){
        _curWriteIndex = 0;
        _isBufferFull = true;
      }
    }

    void draw(const Adafruit_SSD1306& disp, int yGraph, int heightGraph, int minY, int maxY){

      // Draw the line graph based on data in _circularBuffer
      int xPos = 0; 
      for (int i = _curWriteIndex; i < _bufferSize; i++){
        int yVal = _circularBuffer[i];

        // convert to yPos
        int lineHeight = map(yVal, minY, maxY, yGraph, yGraph + heightGraph);
        int yPos = yGraph + heightGraph - lineHeight;
        
        //drawValue(disp, xPos, yPos);
        drawSymbol(disp, xPos, yPos, _ptSize, _symbol);
        xPos += _ptSize;
      }
      
      for(int i = 0; i < _curWriteIndex; i++){
        int yVal = _circularBuffer[i];

        // convert to yPos
        int lineHeight = map(yVal, minY, maxY, yGraph, yGraph + heightGraph);
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

class ScrollingLineGraphMultiValue{
  protected:
    GraphLine** _graphLines = NULL;
    int _numLines = -1;

    int _yMin = 0;
    int _yMax = 1023;

    int _xGraph = 0; // in pixels
    int _yGraph = 0; // in pixels
    int _widthGraph = 128; // in pixels
    int _heightGraph = 64; // in pixels

    boolean _drawAxis = true;
    boolean _isAutoYAxis = true;
    boolean _drawLegend = true;

  public:
    const int DEFAULT_SYMBOL_SIZE = 4;

    ScrollingLineGraphMultiValue(int numLines, PointSymbol symbols[]) :
      ScrollingLineGraphMultiValue(0, 0, 128, 64, numLines, symbols, DEFAULT_SYMBOL_SIZE){
        // purposefully empty
      }

    ScrollingLineGraphMultiValue(int numLines, PointSymbol symbols[], int symbolSize) :
      ScrollingLineGraphMultiValue(0, 0, 128, 64, numLines, symbols, symbolSize){
        // purposefully empty
      }
  
    ScrollingLineGraphMultiValue(int xGraph, int yGraph, int graphWidth, int graphHeight, int numLines, PointSymbol symbols[], int symbolSize) 
    {
      _xGraph = xGraph;
      _yGraph = yGraph;
      _widthGraph = graphWidth;
      _heightGraph = graphHeight;
      
      _graphLines = new GraphLine*[numLines];
      _numLines = numLines;

      for (int i = 0; i < numLines; i++) {
        _graphLines[i] = new GraphLine(graphWidth, symbols[i], symbolSize);
      }
    }

    ~ScrollingLineGraphMultiValue(){
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

    void setDrawLegend(boolean drawLegendOn){
      _drawLegend = drawLegendOn;
    }

    void setDrawAxis(boolean drawAxisOn){
      _drawAxis = drawAxisOn;
    }

    void setAutoYAxis(boolean autoYAxisOn){
        _isAutoYAxis = autoYAxisOn;
    }

    void setMinMaxY(int minY, int maxY){
        _yMin = min(minY, maxY);
        _yMax = max(minY, maxY);
    }

    int getMinY(){
      return _yMin;
    }

    int getMaxY(){
      return _yMax;
    }

    void addData(int graphLineIndex, int newYPos){
      if(graphLineIndex > _numLines){
        return;
      }
      
      _graphLines[graphLineIndex]->addData(newYPos);

      if(_isAutoYAxis){
        int minY = 32767;
        int maxY = -32768;
        
        int min = 0, max = 0;
        for (int i = 0; i < _numLines; i++) {
          _graphLines[i]->getMinMaxYValue(min, max);
          if(min < minY){
            minY = min;
          }

          if(max > maxY){
            maxY = max;
          }
        }

        _yMin = minY;
        _yMax = maxY;
      }
    }

    void draw(const Adafruit_SSD1306& disp){
      if(_drawAxis){
        drawAxis(disp);
      }

      if(_drawLegend){
        drawLegend(disp, 0, 0);
      }

      for (int i = 0; i < _numLines; i++) {
        _graphLines[i]->draw(disp, _yGraph, _heightGraph, _yMin, _yMax);
      }
    }

    // Draw the axis
    void drawAxis(const Adafruit_SSD1306& disp){
      int16_t xText, yText;
      uint16_t textWidth, textHeight;
      
      int yPos = 0;

      // Only draw top of graph val if we're not drawing legend
      // because they conflict with each other
      if(!_drawLegend){
        yPos = getYPixelFromSensorVal(_yMax);
        disp.setCursor(getLeft(), yPos);
        disp.print(_yMax);
      }
      
      // Draw middle value and dotted line
      int yMiddle = _yMin + (_yMax - _yMin) / 2;
      yPos = getYPixelFromSensorVal(yMiddle);
    
      // draw dotted line
      for(int x = getLeft(); x < getRight(); x += 5){
        disp.drawPixel(x, yPos, SSD1306_WHITE);
      }

      String strVal = (String)yMiddle;
      disp.getTextBounds(strVal, 0, 0, &xText, &yText, &textWidth, &textHeight);
    
      disp.setCursor(getLeft(), yPos - textHeight);
      disp.print(yMiddle);

      // Draw bottom of graph val
      strVal = (String)_yMin;
      disp.getTextBounds(strVal, 0, 0, &xText, &yText, &textWidth, &textHeight);
      yPos = getYPixelFromSensorVal(_yMin);
      disp.setCursor(getLeft(), yPos - textHeight);
      disp.print(_yMin);
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

    // Get the y-pixel position for the sensor value
    int getYPixelFromSensorVal(int sensorVal){
      int lineHeight = map(sensorVal, _yMin, _yMax, _yGraph, _yGraph + _heightGraph);
      int yPos = _yGraph + _heightGraph - lineHeight;
      return yPos;
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