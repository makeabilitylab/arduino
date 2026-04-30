/**
 * @file ScrollingLineGraphMultiValue.hpp
 * @brief A scrolling line graph multi-value library for monochrome OLEDs using 
 * Adafruit_SSD1306.h, which is Adafruit's library based on the SSD1306 drivers.
 *
 * Supports multiple data series, each rendered with a different PointSymbol.
 * Includes auto-scaling Y-axis, optional axis labels, and a legend.
 *
 * This is the more feature-rich successor to the MultiValueScrollingLineGraph
 * class in LineGraph.hpp. If you only need one data series rendered as lines
 * (not symbols), see ScrollingLineGraph.hpp instead.
 *
 * Video demo:
 * https://youtu.be/EJDewh8Y1TY
 *
 * See also:
 * LineGraph.hpp
 * ScrollingLineGraph.hpp
 *
 * @section usage Usage Example
 * @code
 *   PointSymbol symbols[] = { CIRCLE, SQUARE };
 *   ScrollingLineGraphMultiValue graph(2, symbols);
 *   graph.setAutoYAxis(true);
 *
 *   void loop() {
 *     graph.addData(0, analogRead(A0));
 *     graph.addData(1, analogRead(A1));
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

#include "GraphLine.hpp"

class ScrollingLineGraphMultiValue {
  protected:
    GraphLine** _graphLines = NULL;
    int _numLines = -1;

    int _yMin = 0;
    int _yMax = 1023;

    int _xGraph = 0;         // in pixels
    int _yGraph = 0;         // in pixels
    int _widthGraph = 128;   // in pixels
    int _heightGraph = 64;   // in pixels

    boolean _drawAxis = true;
    boolean _isAutoYAxis = true;
    boolean _drawLegend = true;

  public:
    static const int DEFAULT_SYMBOL_SIZE = 4;

    ScrollingLineGraphMultiValue(int numLines, PointSymbol symbols[])
      : ScrollingLineGraphMultiValue(0, 0, 128, 64, numLines, symbols, DEFAULT_SYMBOL_SIZE) {
      // purposefully empty
    }

    ScrollingLineGraphMultiValue(int numLines, PointSymbol symbols[], int symbolSize)
      : ScrollingLineGraphMultiValue(0, 0, 128, 64, numLines, symbols, symbolSize) {
      // purposefully empty
    }

    ScrollingLineGraphMultiValue(int xGraph, int yGraph, int graphWidth,
                                 int graphHeight, int numLines,
                                 PointSymbol symbols[], int symbolSize) {
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

    ~ScrollingLineGraphMultiValue() {
      if (_graphLines != NULL) {
        // Each _graphLines[i] was allocated with `new` (not `new[]`),
        // so we use `delete` (not `delete[]`)
        // From https://stackoverflow.com/a/4194228/388117
        for (int i = 0; i < _numLines; i++) {
          delete _graphLines[i];
        }

        delete[] _graphLines;
        _graphLines = NULL;
        _numLines = -1;
      }
    }

    void setDrawLegend(boolean drawLegendOn) {
      _drawLegend = drawLegendOn;
    }

    void setDrawAxis(boolean drawAxisOn) {
      _drawAxis = drawAxisOn;
    }

    void setAutoYAxis(boolean autoYAxisOn) {
      _isAutoYAxis = autoYAxisOn;
    }

    void setMinMaxY(int minY, int maxY) {
      _yMin = min(minY, maxY);
      _yMax = max(minY, maxY);
    }

    int getMinY() const { return _yMin; }

    int getMaxY() const { return _yMax; }

    void addData(int graphLineIndex, int newYPos) {
      // Guard: valid indices are 0 to _numLines - 1
      if (graphLineIndex < 0 || graphLineIndex >= _numLines) {
        return;
      }

      _graphLines[graphLineIndex]->addData(newYPos);

      // Recalculate Y-axis bounds across all lines
      if (_isAutoYAxis) {
        int minY = 32767;
        int maxY = -32768;

        int min = 0, max = 0;
        for (int i = 0; i < _numLines; i++) {
          _graphLines[i]->getMinMaxValue(min, max);
          if (min < minY) {
            minY = min;
          }

          if (max > maxY) {
            maxY = max;
          }
        }

        _yMin = minY;
        _yMax = maxY;
      }
    }

    void draw(Adafruit_SSD1306& disp) const {
      if (_drawAxis) {
        drawAxis(disp);
      }

      if (_drawLegend) {
        drawLegend(disp, 0, 0);
      }

      for (int i = 0; i < _numLines; i++) {
        _graphLines[i]->draw(disp, _yGraph, _heightGraph, _yMin, _yMax);
      }
    }

    // Draw the axis
    void drawAxis(Adafruit_SSD1306& disp) const {
      int16_t xText, yText;
      uint16_t textWidth, textHeight;

      int yPos = 0;

      // Only draw top of graph val if we're not drawing legend
      // because they conflict with each other
      if (!_drawLegend) {
        yPos = getYPixelFromSensorVal(_yMax);
        disp.setCursor(getLeft(), yPos);
        disp.print(_yMax);
      }

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

    void drawLegend(Adafruit_SSD1306& disp, int x, int y) const {
      int xCurPos = -1;
      int ySymbolPos = -1;
      int16_t xText, yText;
      uint16_t textWidth, textHeight;
      int symbolSize = 6;

      for (int i = 0; i < _numLines; i++) {
        PointSymbol ptSymbol = _graphLines[i]->getSymbol();
        //int symbolSize = _graphLines[i]->getSymbolSize();

        if (xCurPos == -1) {
          xCurPos = x + symbolSize / 2;
        }

        if (ySymbolPos == -1) {
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
    int getYPixelFromSensorVal(int sensorVal) const {
      int lineHeight = map(sensorVal, _yMin, _yMax, 0, _heightGraph);
      int yPos = _yGraph + _heightGraph - lineHeight;
      return yPos;
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
