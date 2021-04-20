
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
    const int _upDownInputPin;
    const int _leftRightInputPin;
    int _maxAnalogValue = 1023;
    enum JoystickYDirection _joystickYDir = UP;

    int _upDownVal;
    int _leftRightVal;


  public:
    ParallaxJoystick(int upDownInputPin, int leftRightInputPin) {
      _upDownInputPin = upDownInputPin;
      _leftRightInputPin = leftRightInputPin;
    }

    /**
       Defaults to 1023
    */
    setMaxAnalogValue(int maxAnalogVal) {
      _maxAnalogValue = maxAnalogVal;
    }

    setJoystickYOrientation(JoystickYDirection joystickYDir){
      _joystickYDir = joystickYDir;
    }

    int getUpDownVal(){
      return _upDownVal;
    }

    int getLeftRightVal(){
      return _getLeftRightVal;
    }

    update() {
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
        _upDownVal = _maxAnalogValueOG_VAL - tmpX;
      }else{
        _leftRightVal = joystickLeftRightVal;
        _upDownVal = joystickUpDownVal;  
      }

    }
};
