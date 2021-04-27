class MovingAverageFilter{
  protected:
    int* _samples = NULL; // circular buffer
    int _windowSize = -1;
    long _sampleTotal = 0; 
    int _curReadIndex = 0; 

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
      }  
    }

    int getAverage(){
      return _sampleTotal / _windowSize;
    }
  
    float getAverageAsFloat(){
      return _sampleTotal / (float)_windowSize;
    }    
};