/**
 * @file ScrollingLineGraph.hpp
 * @brief A scrolling line graph library for monochrome OLEDs using Adafruit_SSD1306.h, 
 * which is Adafruit's library based on the SSD1306 drivers.
 *
 * Renders a real-time scrolling line graph with auto-scaling Y-axis,
 * optional axis labels, and a data legend. Data is stored in a circular
 * buffer and drawn as connected line segments or individual points.
 *
 * Video demo:
 * https://youtu.be/MJt9kSNlsU4
 *
 * See also:
 * LineGraph.hpp
 * ScrollingLineGraphMultiValue.hpp
 *
 * @section usage Usage Example
 * @code
 *   ScrollingLineGraph graph(128, 64);
 *   graph.setDataLabel("Light");
 *
 *   void loop() {
 *     graph.addData(analogRead(A0));
 *     display.clearDisplay();
 *     graph.draw(display);
 *     display.display();
 *   }
 * @endcode
 *
 * @author Jon E. Froehlich
 * @jonfroehlich
 * https://jonfroehlich.github.io/
 * https://makeabilitylab.github.io/physcomp/advancedio/oled.html
 */
#pragma once

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

/** @brief Drawing primitive for the graph: connected lines or individual points. */
enum GraphPrimitive {
  POINTS,  ///< Draw each sample as an individual pixel
  LINES    ///< Draw connected line segments between samples
};

class ScrollingLineGraph {
  protected:
    int _yMin = DEFAULT_MIN_Y;
    int _yMax = DEFAULT_MAX_Y;

    boolean _isAutoYAxis = true;

    int _xGraph = 0;            // in pixels
    int _yGraph = 0;            // in pixels
    int _widthGraph = 128;      // in pixels
    int _heightGraph = 64;      // in pixels

    int* _circularBuffer;
    int _curWriteIndex = 0;
    boolean _isBufferFull = false;
    int _bufferSize = -1;

    String _dataLabel = "";
    GraphPrimitive _drawGraphPrimitive = LINES;

    boolean _drawAxis = true;
    boolean _drawLegend = true;

  public:
    static const int DEFAULT_MIN_Y = 0;
    static const int DEFAULT_MAX_Y = 1023;

    ScrollingLineGraph(int graphWidth, int graphHeight)
      : ScrollingLineGraph(0, 0, graphWidth, graphHeight) {
      //empty on purpose
    }

    ScrollingLineGraph(int xGraph, int yGraph, int graphWidth, int graphHeight) {
      _xGraph = xGraph;
      _yGraph = yGraph;
      _widthGraph = graphWidth;
      _heightGraph = graphHeight;

      _bufferSize = _widthGraph - _xGraph;
      _circularBuffer = new int[_bufferSize];

      // initialize all the readings to 0:
      for (int i = 0; i < _bufferSize; i++) {
        _circularBuffer[i] = 0;
      }
    }

    ~ScrollingLineGraph() {
      if (_circularBuffer != NULL) {
        delete[] _circularBuffer;
        _circularBuffer = NULL;
        _bufferSize = -1;
      }
    }

    void setDrawLegend(boolean drawLegendOn) { _drawLegend = drawLegendOn; }

    void setDrawAxis(boolean drawAxisOn) { _drawAxis = drawAxisOn; }

    void setAutoYAxis(boolean autoYAxisOn) { _isAutoYAxis = autoYAxisOn; }

    void setMinMaxY(int minY, int maxY) {
      _yMin = min(minY, maxY);
      _yMax = max(minY, maxY);
    }

    int getMinY() const { return _yMin; }

    int getMaxY() const { return _yMax; }

    void resetGraph() {
      // initialize all the readings to 0:
      for (int i = 0; i < _bufferSize; i++) {
        _circularBuffer[i] = 0;
      }
      _yMin = DEFAULT_MIN_Y;
      _yMax = DEFAULT_MAX_Y;
      _curWriteIndex = 0;
      _isBufferFull = false;
    }

    void setDataLabel(String label) {
      _dataLabel = label;
    }

    int getMostRecentValue() const {
      int curValIndex = _curWriteIndex - 1;
      if (curValIndex < 0) {
        curValIndex = _bufferSize - 1;
      }

      return _circularBuffer[curValIndex];
    }

    void addData(int yVal) {

      _circularBuffer[_curWriteIndex++] = yVal;

      // Set the circular buffer index back to zero when it reaches the
      // the end of the buffer
      if (_curWriteIndex >= _bufferSize) {
        _curWriteIndex = 0;
        _isBufferFull = true;
      }

      // find y min and max
      if (_isAutoYAxis) {
        int maxSearchRange = _bufferSize;
        if (!_isBufferFull) {
          maxSearchRange = _curWriteIndex;
        }

        int minYInBuffer = 32767;
        int maxYInBuffer = -32768;
        for (int i = 0; i < maxSearchRange; i++) {
          if (_circularBuffer[i] < minYInBuffer) {
            minYInBuffer = _circularBuffer[i];
          }

          if (_circularBuffer[i] > maxYInBuffer) {
            maxYInBuffer = _circularBuffer[i];
          }
        }

        _yMin = minYInBuffer;
        _yMax = maxYInBuffer;
      }
    }

    void draw(Adafruit_SSD1306& disp) const {
      if (_drawAxis) {
        drawAxis(disp);
      }

      if (_drawLegend) {
        drawLegend(disp);
      }
      drawLineGraph(disp);
    }

    void drawLegend(Adafruit_SSD1306& disp) const {
      int16_t xText, yText;
      uint16_t textWidth, textHeight;
      String strLegend = _dataLabel + ": " + getMostRecentValue();
      disp.getTextBounds(strLegend, 0, 0, &xText, &yText, &textWidth, &textHeight);

      disp.setCursor(getRight() - textWidth, getTop());
      disp.print(strLegend);
    }

    // Draw the line graph.
    // We use a prevSensorVal variable to safely track the previous value
    // across the circular buffer boundary, avoiding out-of-bounds access
    // that would occur with _circularBuffer[i - 1] when i == _curWriteIndex.
    void drawLineGraph(Adafruit_SSD1306& disp) const {
      int xPos = _xGraph;
      int prevSensorVal = _circularBuffer[_curWriteIndex]; // oldest value in buffer

      for (int i = _curWriteIndex; i < _bufferSize; i++) {
        int sensorVal = _circularBuffer[i];

        // draw the line (or point)
        if (_drawGraphPrimitive == POINTS) {
          drawPoint(disp, xPos, sensorVal);
        } else {
          if (xPos > _xGraph) {
            // Only draw a line segment starting from the second pixel onward
            drawLine(disp, xPos - 1, prevSensorVal, xPos, sensorVal);
          }
        }

        prevSensorVal = sensorVal;
        xPos++;
      }

      for (int i = 0; i < _curWriteIndex; i++) {
        int sensorVal = _circularBuffer[i];

        //draw the line (or point)
        if (_drawGraphPrimitive == POINTS) {
          drawPoint(disp, xPos, sensorVal);
        } else {
          // prevSensorVal carries across the buffer boundary correctly
          drawLine(disp, xPos - 1, prevSensorVal, xPos, sensorVal);
        }

        prevSensorVal = sensorVal;
        xPos++;
      }
    }

    // Draws a line between two sensor values at the given x positions
    void drawLine(Adafruit_SSD1306& disp, int x1, int sensorVal1,
                  int x2, int sensorVal2) const {
      int y1 = getYPixelFromSensorVal(sensorVal1);
      int y2 = getYPixelFromSensorVal(sensorVal2);

      disp.drawLine(x1, y1, x2, y2, SSD1306_WHITE);
    }

    // Get the y-pixel position for the sensor value
    int getYPixelFromSensorVal(int sensorVal) const {
      int lineHeight = map(sensorVal, _yMin, _yMax, 0, _heightGraph);
      int yPos = _yGraph + _heightGraph - lineHeight;
      return yPos;
    }

    // Draws the value at the x pixel position
    void drawPoint(Adafruit_SSD1306& disp, int xPos, int sensorVal) const {
      int yPos = getYPixelFromSensorVal(sensorVal);
      disp.drawPixel(xPos, yPos, SSD1306_WHITE);
    }

    // Draw the axis in middle of screen
    void drawAxis(Adafruit_SSD1306& disp) const {
      int16_t xText, yText;
      uint16_t textWidth, textHeight;

      // Draw top of graph val
      int yPos = getYPixelFromSensorVal(_yMax);
      disp.setCursor(getLeft(), yPos);
      disp.print(_yMax);

      // Draw middle value and dotted line
      int yMiddle = _yMin + (_yMax - _yMin) / 2;
      yPos = getYPixelFromSensorVal(yMiddle);

      // draw dotted line
      for (int x = getLeft(); x < getRight(); x += 5) {
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

    /**
     * @brief Get the width of the graph
     * @return int Width of the shape in pixels
     */
    int getWidth() const { return _widthGraph; }

    /**
     * @brief Get the height of the graph
     * @return int Height of the graph in pixels
     */
    int getHeight() const { return _heightGraph; }

    /**
     * @brief Get the left (x) location of the graph
     * @return int The left location of the shape
     */
    int getLeft() const { return _xGraph; }

    /**
     * @brief Get the right (x) location of the graph
     * @return int The right location (x) of the graph
     */
    int getRight() const { return _xGraph + _widthGraph; }

    /**
     * @brief Get the bottom of the graph (y)
     * @return int
     */
    int getBottom() const { return _yGraph + _heightGraph; }

    /**
     * @brief Get the top of the graph
     * @return int
     */
    int getTop() const { return _yGraph; }
};
