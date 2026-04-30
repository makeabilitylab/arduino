/**
 * @file GraphLine.hpp
 * @brief A single data series for plotting on a monochrome OLED.
 *
 * Provides the PointSymbol enum and GraphLine class used by both
 * LineGraph.hpp and ScrollingLineGraphMultiValue.hpp. Each GraphLine
 * stores a circular buffer of integer values and draws them using
 * a configurable point symbol (circle, square, triangle, or pixel).
 *
 * @note Requires Adafruit_SSD1306.h. Include Shape.hpp or the Adafruit
 *       headers before this file if not already included.
 *
 * @author Jon E. Froehlich
 * @jonfroehlich
 * https://jonfroehlich.github.io/
 * https://makeabilitylab.github.io/physcomp/advancedio/oled.html
 */
#pragma once

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

/** @brief Symbol shapes used to plot data points on a graph line. */
enum PointSymbol {
  CIRCLE,    ///< Open circle
  SQUARE,    ///< Open square
  TRIANGLE,  ///< Open triangle
  POINT      ///< Single pixel
};

/**
 * @brief A single data series rendered as symbols on a scrolling graph.
 *
 * Data is stored in a circular buffer. The buffer size is determined by
 * the display width divided by the symbol size, so each symbol occupies
 * a fixed horizontal space.
 */
class GraphLine {
  protected:
    int* _circularBuffer = NULL;  ///< Circular buffer of data values
    int _bufferSize = -1;         ///< Number of slots in the buffer
    long _sampleTotal = 0;        ///< Running sum (preserved from original, unused currently)
    int _curWriteIndex = 0;       ///< Next write position in the buffer
    bool _isBufferFull = false;   ///< True once the buffer has wrapped at least once
    enum PointSymbol _symbol = CIRCLE; ///< Symbol used to render this line
    int _ptSize = DEFAULT_SYMBOL_SIZE; ///< Size of each symbol in pixels (width and height)

  public:
    static const int DEFAULT_SYMBOL_SIZE = 4;

    GraphLine(int displayWidth, PointSymbol symbol)
      : GraphLine(displayWidth, symbol, DEFAULT_SYMBOL_SIZE)
    {
      // empty on purpose
    }

    GraphLine(int displayWidth, PointSymbol symbol, int symbolSize) {
      _ptSize = symbolSize;
      _bufferSize = displayWidth / _ptSize;
      _circularBuffer = new int[_bufferSize];

      _symbol = symbol;

      // initialize all the readings to 0:
      for (int i = 0; i < _bufferSize; i++) {
        _circularBuffer[i] = 0;
      }
    }

    ~GraphLine() {
      if (_circularBuffer != NULL) {
        delete[] _circularBuffer;
        _circularBuffer = NULL;
        _bufferSize = -1;
      }
    }

    PointSymbol getSymbol() const { return _symbol; }

    int getSymbolSize() const { return _ptSize; }

    int getCurrentValue() const {
      int curValIndex = _curWriteIndex - 1;
      if (curValIndex < 0) {
        curValIndex = _bufferSize - 1;
      }
      return _circularBuffer[curValIndex];
    }

    /**
     * @brief Find the min and max values currently in the buffer.
     *
     * Only searches the populated portion of the buffer (i.e., if the
     * buffer hasn't wrapped yet, only searches up to the write index).
     *
     * @param[out] minVal Set to the minimum value found
     * @param[out] maxVal Set to the maximum value found
     */
    void getMinMaxValue(int& minVal, int& maxVal) const {
      int maxSearchRange = _bufferSize;
      if (!_isBufferFull) {
        maxSearchRange = _curWriteIndex;
      }

      minVal = 32767;
      maxVal = -32768;
      for (int i = 0; i < maxSearchRange; i++) {
        if (_circularBuffer[i] < minVal) {
          minVal = _circularBuffer[i];
        }
        if (_circularBuffer[i] > maxVal) {
          maxVal = _circularBuffer[i];
        }
      }
    }

    void addData(int yVal) {
      _circularBuffer[_curWriteIndex++] = yVal;

      // Set the circular buffer index back to zero when it reaches the
      // the end of the buffer
      if (_curWriteIndex >= _bufferSize) {
        _curWriteIndex = 0;
        _isBufferFull = true;
      }
    }

    /**
     * @brief Draw this line on the OLED display.
     *
     * Renders each value in the circular buffer as a symbol, starting
     * from the oldest value and wrapping around.
     *
     * @param disp        The Adafruit_SSD1306 display
     * @param yGraph      Y-origin of the graph area (top edge, in pixels)
     * @param heightGraph Height of the graph area in pixels
     * @param minY        Data value that maps to the bottom of the graph
     * @param maxY        Data value that maps to the top of the graph
     */
    void draw(Adafruit_SSD1306& disp, int yGraph, int heightGraph,
              int minY, int maxY) const {
      // Draw the line graph based on data in _circularBuffer
      int xPos = 0;

      // First loop: draw from oldest data (at _curWriteIndex) to end of buffer
      for (int i = _curWriteIndex; i < _bufferSize; i++) {
        int yVal = _circularBuffer[i];

        // convert to yPos
        int lineHeight = map(yVal, minY, maxY, 0, heightGraph);
        int yPos = yGraph + heightGraph - lineHeight;

        drawSymbol(disp, xPos, yPos, _ptSize, _symbol);
        xPos += _ptSize;
      }

      // Second loop: draw from buffer start to the most recent data
      for (int i = 0; i < _curWriteIndex; i++) {
        int yVal = _circularBuffer[i];

        // convert to yPos
        int lineHeight = map(yVal, minY, maxY, 0, heightGraph);
        int yPos = yGraph + heightGraph - lineHeight;

        drawSymbol(disp, xPos, yPos, _ptSize, _symbol);
        xPos += _ptSize;
      }
    }

    /**
     * @brief Draws the symbol centered at the given x,y point.
     *
     * @param disp   The Adafruit_SSD1306 display
     * @param x      X center of the symbol
     * @param y      Y center of the symbol
     * @param size   Width/height of the symbol
     * @param symbol Which symbol to draw
     */
    static void drawSymbol(Adafruit_SSD1306& disp, int x, int y,
                           int size, PointSymbol symbol) {
      // Ugh, ran into major problem with switch statements
      // Need to have brackets around cases
      // See: https://stackoverflow.com/questions/92396/why-cant-variables-be-declared-in-a-switch-statement
      int halfSize = size / 2;

      switch (symbol) {
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
          break;
        }
      }
    }
};
