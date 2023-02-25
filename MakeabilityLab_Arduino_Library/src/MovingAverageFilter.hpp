/**
 * A basic moving average filter class.
 * 
 * By Jon E. Froehlich
 * @jonfroehlich
 * https://jonfroehlich.github.io/
 * https://makeabilitylab.github.io/physcomp/advancedio/smoothing-input.html
 * 
 */
class MovingAverageFilter{
  protected:
    int* _samples = NULL; // circular buffer
    int _windowSize = -1;
    long _sampleTotal = 0; 
    int _curReadIndex = 0; 
    bool _isWindowFull = false; // useful for knowing if the buffer is full or not

  public:
  /**
   * @brief Construct a new Moving Average Filter with the given window size
   * 
   * @param windowSize the window size of the moving average filter
   */
    MovingAverageFilter(int windowSize) 
    {
      _samples = new int[windowSize];
      _windowSize = windowSize;

      // initialize all the readings to 0:
      for (int i = 0; i < windowSize; i++) {
        _samples[i] = 0;
      }
    }

    ~MovingAverageFilter(){
      if(_samples != NULL){
        delete [] _samples;
        _samples = NULL;
        _windowSize = -1;
      }
    }

    /**
     * @brief Add a new data point to the moving average filter
     * 
     * @param newData 
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
    int getAverage(){
      return _sampleTotal / _windowSize;
    }
  
    /**
     * @brief Get the average as an integer
     * 
     * @return float the average as a float
     */
    float getAverageAsFloat(){
      return _sampleTotal / (float)_windowSize;
    }    

    /**
     * @brief Returns true if the sliding window is full of data. Only relevant when
     * the algorithm first starts. This is helpful if you only want to use the getAverage() 
     * result if the full sliding window state has been reached
     * 
     * @return true If the sliding window buffer is full
     * @return false If the sliding window buffer is not full
     */
    bool isWindowFull(){
        return _isWindowFull;
    }
};