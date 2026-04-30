/**
 * @file MovingAverageFilter.hpp
 * @brief Sliding-window moving average filter for smoothing sensor input.
 *
 * A basic moving average filter class. Maintains a circular buffer of the
 * most recent N samples and computes a running average. Useful for reducing
 * noise in analog sensor readings.
 *
 * @section usage Usage Example
 * @code
 *   MovingAverageFilter filter(10);  // 10-sample window
 *
 *   void loop() {
 *     int raw = analogRead(A0);
 *     filter.add(raw);
 *
 *     if (filter.isWindowFull()) {
 *       int smoothed = filter.getAverage();
 *       Serial.println(smoothed);
 *     }
 *   }
 * @endcode
 *
 * @see https://makeabilitylab.github.io/physcomp/advancedio/smoothing-input.html
 *
 * @author Jon E. Froehlich
 * @jonfroehlich
 * https://jonfroehlich.github.io/
 * https://makeabilitylab.github.io/physcomp/
 */
#pragma once

class MovingAverageFilter {
  protected:
    int* _samples = NULL;        ///< Circular buffer of sample values
    int _windowSize = -1;        ///< Number of samples in the window
    long _sampleTotal = 0;       ///< Running sum of all samples in the buffer
    int _curReadIndex = 0;       ///< Next write position in the circular buffer
    bool _isWindowFull = false;  ///< True once the buffer has been fully populated

  public:
    /**
     * @brief Construct a new Moving Average Filter with the given window size.
     *
     * @param windowSize The window size of the moving average filter (must be > 0)
     */
    MovingAverageFilter(int windowSize) {
      _samples = new int[windowSize];
      _windowSize = windowSize;

      // initialize all the readings to 0:
      for (int i = 0; i < windowSize; i++) {
        _samples[i] = 0;
      }
    }

    /** @brief Destructor. Frees the sample buffer. */
    ~MovingAverageFilter() {
      if (_samples != NULL) {
        delete[] _samples;
        _samples = NULL;
        _windowSize = -1;
      }
    }

    /**
     * @brief Add a new data point to the moving average filter.
     *
     * Replaces the oldest sample in the circular buffer and updates the
     * running total used to compute the average.
     *
     * @param newData The new sample value
     */
    void add(int newData) {
      // subtract the last reading:
      _sampleTotal = _sampleTotal - _samples[_curReadIndex];

      // add new data
      _samples[_curReadIndex] = newData;

      // add the reading to the total:
      _sampleTotal = _sampleTotal + _samples[_curReadIndex];

      // advance to the next position in the array:
      _curReadIndex = _curReadIndex + 1;

      // if we're at the end of the array...
      if (_curReadIndex >= _windowSize) {
        // ...wrap around to the beginning:
        _curReadIndex = 0;
        _isWindowFull = true;
      }
    }

    /**
     * @brief Gets the average as an integer. If the division results in a fraction,
     * the fractional component is discarded (truncated). No rounding occurs.
     *
     * @return int the average as an integer (fractional components are discarded)
     */
    int getAverage() const {
      return _sampleTotal / _windowSize;
    }

    /**
     * @brief Get the average as a float.
     *
     * @return float the average as a float
     */
    float getAverageAsFloat() const {
      return _sampleTotal / (float)_windowSize;
    }

    /**
     * @brief Returns true if the sliding window is full of data. Only relevant when
     * the algorithm first starts. This is helpful if you only want to use the getAverage()
     * result if the full sliding window state has been reached.
     *
     * @return true If the sliding window buffer is full
     * @return false If the sliding window buffer is not full
     */
    bool isWindowFull() const {
      return _isWindowFull;
    }
};
