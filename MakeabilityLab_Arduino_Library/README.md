This library includes a [joystick class](https://github.com/makeabilitylab/arduino/blob/master/MakeabilityLab_Arduino_Library/src/ParallaxJoystick.hpp) to handle input from the [Parallax 2-Axis joystick](https://www.adafruit.com/product/245), an example [moving average filter](https://github.com/makeabilitylab/arduino/blob/master/MakeabilityLab_Arduino_Library/src/MovingAverageFilter.hpp), and a [basic "game" library](https://github.com/makeabilitylab/arduino/blob/master/MakeabilityLab_Arduino_Library/src/Shape.hpp) with shape drawing and simple collision detection.

## Installation

The easiest way to get and install the library is to do a git clone on `https://github.com/makeabilitylab/arduino.git`. That is:

`git clone https://github.com/makeabilitylab/arduino.git`

Then move the `MakeabilityLab_Arduino_Library` into your Arduino `libraries` folder:
- On my Windows box, this is C:\Users\jonfr\Documents\Arduino\libraries
- On Mac, it's: /Users/jonf/Documents/Arduino/libraries

Finally, you can include the relevant libraries via `#include <libraryname.hpp>`. For example, to include `shape.hpp`, you would type `#include <shape.hpp>` just as we do in the [BallBounceObjectOriented.ino](https://github.com/makeabilitylab/arduino/blob/master/OLED/BallBounceObjectOriented/BallBounceObjectOriented.ino) demo.

To test and ensure that you've installed the library correctly, try running [BallBounceObjectOriented.ino](https://github.com/makeabilitylab/arduino/blob/master/OLED/BallBounceObjectOriented/BallBounceObjectOriented.ino) or [FlappyBird](https://github.com/makeabilitylab/arduino/blob/master/OLED/FlappyBird/FlappyBird.ino).
