/**
 * Creates a named color and provides color-related methods (e.g., find closest color)
 * 
 * By Jon E. Froehlich
 * @jonfroehlich
 * https://jonfroehlich.github.io/
 * https://makeabilitylab.github.io/physcomp/
 * 
 */
class ColorName {
  protected:
    String _name;
    byte _red;
    byte _green; 
    byte _blue;

  public:
    ColorName(String name, byte red, byte green, byte blue){
      _name = name;
      _red = red;
      _green = green;
      _blue = blue;
    }

    /**
     * @brief Get the red component of the color (0-255)
     * 
     * @return byte 
     */
    byte getRed(){
      return _red;
    }

    /**
     * @brief Get the green component of the color (0-255)
     * 
     * @return byte 
     */
    byte getGreen(){
      return _green;
    }

    /**
     * @brief Get the blue component of the color (0-255)
     * 
     * @return byte Blue component
     */
    byte getBlue(){
      return _blue;
    }

    /**
     * @brief Get the name of the color
     * 
     * @return String The color name
     */
    String getName(){
      return _name;
    }

    /**
     * @brief Compute the mean square error between this color and the passed in color
     * See:  https://en.wikipedia.org/wiki/Mean_squared_error
     * 
     * @param red 
     * @param green 
     * @param blue 
     * @return long 
     */
    long computeMse(byte red, byte green, byte blue) {
      return (long) ((
        (long)(red - _red) * (red - _red) + 
        (long)(green - _green) * (green - _green) + 
        (long)(blue - _blue) * (blue - _blue)) / 3);
    }

    // gets the closest color from the given red, green, blue values
    static ColorName* getClosestColorName(ColorName colorNames[], unsigned int numColors, byte red, byte green, byte blue){
      Serial.println((String)"Checking best match for (R, G, B): (" + red + ", " + green + ", " + blue + ")");
      
      ColorName* _bestMatchColorName = NULL;
      long bestMatchDistance = 65535;
      for(int i = 0; i < numColors; i++){
        ColorName* curColorName = &colorNames[i];
        if(_bestMatchColorName == NULL){
          _bestMatchColorName = &colorNames[0];
        }

        long meanSquareError = curColorName->computeMse(red, green, blue);

        Serial.println("Match score: " + curColorName->getName() + " with MSE: " + meanSquareError);
        
        if(meanSquareError < bestMatchDistance){
        
          _bestMatchColorName = &colorNames[i];
          bestMatchDistance = meanSquareError;
          Serial.println("New best match: " + _bestMatchColorName->getName() + " with MSE: " + meanSquareError);
        }
      }
      return _bestMatchColorName;
    }
};