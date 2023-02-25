/**
 * Provides a Button class to handle debouncing and other basic button niceties. 
 * 
 * There are lots of wonderful button classes for Arduino. For a relatively recent 
 * breakdown, see this Stack Overflow post: https://arduino.stackexchange.com/a/80857/63793.
 * However, all of these libraries are external. Sometimes, I just want to rely on my own code
 * and import as few as libraries as possible. 
 * 
 * A few well-designed libraries that I also took inspiration from:
 *  - https://github.com/avandalen/avdweb_Switch
 *  - https://github.com/evert-arias/EasyButton
 *  - https://github.com/JChristensen/JC_Button
 * 
 * By Jon E. Froehlich
 * @jonfroehlich
 * https://jonfroehlich.github.io/
 * https://makeabilitylab.github.io/physcomp/
 * 
 */

class Button{
  protected:
    uint8_t _pin;                   // the input pin connected to the button
    bool _isInternalPullUpEnabled;  // is internal pullup enabled
    bool _stateChange;              // state change since last read()
    bool _curState;                 // current button state—true equals pressed (regardless of internal pullup) 
    bool _prevState;                // previous button state
    bool _isActiveLow;              // if true, LOW is pressed; else HIGH is pressed
    unsigned long _lastReadTimestamp;      // last time read was called; uses ms from millis()
    unsigned long _lastChangeTimestamp;    // timestamp of the last state change; uses ms from millis()
    unsigned long _debounceTime;    // the debounce time in ms


  public:
    Button(uint8_t buttonPin, unsigned long debouncedTime=25, bool internalPullUpEnabled = true, bool activeLow = true)
      : _pin(buttonPin), _debounceTime(debouncedTime), _isInternalPullUpEnabled(internalPullUpEnabled), _isActiveLow(activeLow)
      {}

    /**
     * @brief You must call begin() before read()
     */
    void begin(){
      pinMode(_pin, _isInternalPullUpEnabled ? INPUT_PULLUP : INPUT);
      unsigned long curTimestampMs = millis();
      _curState = digitalRead(_pin);

      // If active low, invert the state
      if(_isActiveLow){ 
        _curState = !_curState;
      }
      _lastReadTimestamp = curTimestampMs;
      _lastChangeTimestamp = curTimestampMs;
      _prevState = _curState;
      _stateChange = false;
    }

    /**
     * @brief Reads the pin value and sets the button state according to debouncing specifications
     * 
     * @return the current state 
     */
    bool read(){
      unsigned long curTimestampMs = millis();
      bool pinVal = digitalRead(_pin);
      if(_isActiveLow){
        pinVal = !pinVal;
      }

      if(curTimestampMs - _lastChangeTimestamp < _debounceTime){
        _stateChange = false;
      }else{
        _prevState = _curState;
        _curState = pinVal;
        _stateChange = (_curState != _prevState);
        if(_stateChange){
          _lastChangeTimestamp = curTimestampMs;
        }
      }
      _lastReadTimestamp = curTimestampMs;
      return _curState;
    }

    /**
     * @brief Returns true if button state is currently pressed. Does not call read().
     * 
     * @return true if button state is currently pressed; false otherwise
     */
    bool isPressed(){
      return _curState;
    }

    /**
     * @brief Returns true if the button was just pressed but is now released. Does not call read();
     * 
     * @return true if button was pressed but is now released; false otherwise.
     */
    bool wasPressed(){
      return _prevState && !_curState && _stateChange;
    }

    /**
     * @brief Returns true if the button state changed between reads
     * 
     * @return true if the button state changed between read() calls; false otherwise
     */
    bool didStateChange(){
      return _stateChange;
    }

    /**
     * @brief Returns the time in milliseconds since the last button state changed
     * 
     * @return unsigned long the time in milliseconds since the last button state changed
     */
    unsigned long timeSinceLastStateChange(){
      return millis() - _lastChangeTimestamp;
    }

    /**
     * @brief The time in milliseconds (from millis()) of the last state change
     * 
     * @return unsigned long time in milliseconds (from millis()) of the last state change
     */
    unsigned long lastStateChangeTimestamp(){
      return _lastChangeTimestamp;
    }

};