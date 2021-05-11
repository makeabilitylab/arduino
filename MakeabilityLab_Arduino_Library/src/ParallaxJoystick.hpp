
// The joysticks orientation with respect to the user
// We need this because sometimes we have to place a joystick
// upside down, etc. in our designs
enum JoystickYDirection {
  UP,
  RIGHT,
  DOWN,
  LEFT
};



class ParallaxJoystick {

  protected:
    const int DEFAULT_MAX_ANALOG_VALUE = 1023;
    const int _upDownInputPin;
    const int _leftRightInputPin;
    int _maxAnalogValue = DEFAULT_MAX_ANALOG_VALUE;
    int _joyStickCenterValue = int(DEFAULT_MAX_ANALOG_VALUE / 2);
    enum JoystickYDirection _joystickYDir = UP;

    int _upDownVal;
    int _leftRightVal;


  public:
    ParallaxJoystick(int upDownInputPin, int leftRightInputPin) 
      : _upDownInputPin(upDownInputPin), _leftRightInputPin(_leftRightInputPin) { // syntax to initialize consts
//      _upDownInputPin = upDownInputPin;
//      _leftRightInputPin = leftRightInputPin;
    }

    ParallaxJoystick(int upDownInputPin, int leftRightInputPin, int maxAnalogValue, JoystickYDirection joystickYDir)
      : ParallaxJoystick(upDownInputPin, leftRightInputPin) { // syntax to call constructor in C++ 11
      _joystickYDir = joystickYDir;
      setMaxAnalogValue(maxAnalogValue);
    }

    int getMaxAnalogValue() const{
      return _maxAnalogValue;
    }

    /**
     * Defaults to 1023
     */
    void setMaxAnalogValue(int maxAnalogVal) {
      _maxAnalogValue = maxAnalogVal;
      _joyStickCenterValue = int(maxAnalogVal / 2);
    }

    /**
     * Defaults to UP
     */
    void setJoystickYOrientation(JoystickYDirection joystickYDir){
      _joystickYDir = joystickYDir;
    }

    /**
     * @brief Gets a value between 0 and _maxAnalogValue (1023 on Arduino Uno)
     * that represents up-down joystick direction.
     * 
     * @return int 
     */
    int getUpDownVal() const{
      return _upDownVal;
    }

    int getLeftRightVal() const{
      return _leftRightVal;
    }

    void read() {
      int joystickUpDownVal = analogRead(_upDownInputPin);
      int joystickLeftRightVal = analogRead(_leftRightInputPin);

      // If hooked up on the breadboard, we often have to
      // install the joystick in a different orientation than
      // with the Y up direction facing up. The code below
      // handles the different orientations
      if (_joystickYDir == RIGHT) {
        int tmpX = joystickLeftRightVal;
        _leftRightVal = _maxAnalogValue - joystickUpDownVal;
        _upDownVal = tmpX;
      } else if (_joystickYDir == DOWN) {
        _upDownVal = _maxAnalogValue - joystickUpDownVal;
        _leftRightVal = _maxAnalogValue - joystickLeftRightVal;
      } else if (_joystickYDir == LEFT) {
        int tmpX = joystickLeftRightVal;
        _leftRightVal = joystickUpDownVal;
        _upDownVal = _maxAnalogValue - tmpX;
      }else{
        _leftRightVal = joystickLeftRightVal;
        _upDownVal = joystickUpDownVal;  
      }
    }
};
