/**
 * This class toggles output between HIGH and LOW for a given pin
 * based on the provided "blink" interval
 * 
 * Usage:
 *  Blinker _ledBlinker(3, 200); // blinks Pin 3 on/off every 200 ms
 *  
 *  loop(){
 *    _ledBlinker.update();
 *  }
 */

#include <Arduino.h>

class Blinker{

  private:
    const int _pin;                 // output pin
    const unsigned long _interval;  // blink interval in ms

    int _state;                     // current state (either HIGH OR LOW)
    unsigned long _lastToggledTimestamp; // last state toggle in ms
    
  public:
    Blinker(const int pin, const unsigned long blinkInterval);
    void update();
};
