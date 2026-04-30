/**
 * @file LineGraph.hpp
 * @brief Multi-value scrolling line graph using point symbols on SSD1306 OLEDs.
 *
 * This file provides `MultiValueScrollingLineGraph`, which manages multiple
 * `GraphLine` instances (defined in GraphLine.hpp) and renders them together
 * on a monochrome OLED display.
 *
 * @note For a single-value scrolling graph with line rendering (not symbols),
 *       see ScrollingLineGraph.hpp.
 *
 * @see GraphLine.hpp
 * @see ScrollingLineGraphMultiValue.hpp (newer version with more features)
 *
 * @author Jon E. Froehlich
 * @jonfroehlich
 * https://jonfroehlich.github.io/
 * https://makeabilitylab.github.io/physcomp/advancedio/oled.html
 */
#pragma once

#include "GraphLine.hpp"

/**
 * @brief A multi-line scrolling graph rendered with point symbols.
 *
 * Each line uses a different PointSymbol (CIRCLE, SQUARE, TRIANGLE, POINT)
 * so they can be distinguished on a monochrome display.
 *
 * @section usage Usage Example
 * @code
 *   PointSymbol symbols[] = { CIRCLE, SQUARE };
 *   MultiValueScrollingLineGraph graph(2, symbols);
 *
 *   void loop() {
 *     graph.addData(0, analogRead(A0));
 *     graph.addData(1, analogRead(A1));
 *     display.clearDisplay();
 *     graph.draw(display);
 *     display.display();
 *   }
 * @endcode
 */
class MultiValueScrollingLineGraph {
  protected:
    GraphLine** _graphLines = NULL;  ///< Array of pointers to GraphLine objects
    int _numLines = -1;              ///< Number of graph lines

    int _minY = 0;           ///< Minimum data value (Y-axis floor)
    int _maxY = 1023;        ///< Maximum data value (Y-axis ceiling)

    int _xGraph = 0;         ///< X origin of graph area in pixels
    int _yGraph = 0;         ///< Y origin of graph area in pixels
    int _widthGraph = 128;   ///< Width of graph area in pixels
    int _heightGraph = 64;   ///< Height of graph area in pixels

  public:
    MultiValueScrollingLineGraph(int numLines, PointSymbol symbols[])
      : MultiValueScrollingLineGraph(0, 0, 128, 64, numLines, symbols) {
      // purposefully empty
    }

    MultiValueScrollingLineGraph(int xGraph, int yGraph, int graphWidth,
                                 int graphHeight, int numLines,
                                 PointSymbol symbols[]) {
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

    ~MultiValueScrollingLineGraph() {
      if (_graphLines != NULL) {
        // Each _graphLines[i] was allocated with `new` (not `new[]`),
        // so we use `delete` (not `delete[]`)
        // See: https://stackoverflow.com/a/4194228/388117
        for (int i = 0; i < _numLines; i++) {
          delete _graphLines[i];
        }

        delete[] _graphLines;
        _graphLines = NULL;
        _numLines = -1;
      }
    }

    int getMinY() const { return _minY; }

    int getMaxY() const { return _maxY; }

    void addData(int graphLineIndex, int newYPos) {
      // Guard: valid indices are 0 to _numLines - 1
      if (graphLineIndex < 0 || graphLineIndex >= _numLines) {
        return;
      }

      _graphLines[graphLineIndex]->addData(newYPos);
    }

    void draw(Adafruit_SSD1306& disp) const {
      for (int i = 0; i < _numLines; i++) {
        _graphLines[i]->draw(disp, _yGraph, _heightGraph, _minY, _maxY);
      }
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
