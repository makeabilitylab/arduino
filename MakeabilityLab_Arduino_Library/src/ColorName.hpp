/**
 * @file ColorName.hpp
 * @brief Named RGB color with closest-color matching via mean squared error.
 *
 * Creates a named color and provides color-related methods (e.g., find closest color).
 * Useful with color sensors (e.g., TCS34725) to map a raw RGB reading to
 * the nearest color from a predefined palette.
 *
 * @section usage Usage Example
 * @code
 *   ColorName colors[] = {
 *     ColorName("Red",   255, 0,   0),
 *     ColorName("Green", 0,   255, 0),
 *     ColorName("Blue",  0,   0,   255),
 *   };
 *   int numColors = sizeof(colors) / sizeof(colors[0]);
 *
 *   // Find the closest named color to a sensor reading
 *   ColorName* match = ColorName::getClosestColorName(colors, numColors, 240, 10, 30);
 *   Serial.println(match->getName()); // "Red"
 * @endcode
 *
 * @author Jon E. Froehlich
 * @jonfroehlich
 * https://jonfroehlich.github.io/
 * https://makeabilitylab.github.io/physcomp/
 */
#pragma once

class ColorName {
  protected:
    String _name;
    byte _red;
    byte _green;
    byte _blue;

  public:
    /**
     * @brief Construct a named color.
     *
     * @param name  Human-readable color name (e.g., "Red")
     * @param red   Red component (0–255)
     * @param green Green component (0–255)
     * @param blue  Blue component (0–255)
     */
    ColorName(String name, byte red, byte green, byte blue) {
      _name = name;
      _red = red;
      _green = green;
      _blue = blue;
    }

    /**
     * @brief Get the red component of the color (0-255)
     * @return byte
     */
    byte getRed() const { return _red; }

    /**
     * @brief Get the green component of the color (0-255)
     * @return byte
     */
    byte getGreen() const { return _green; }

    /**
     * @brief Get the blue component of the color (0-255)
     * @return byte Blue component
     */
    byte getBlue() const { return _blue; }

    /**
     * @brief Get the name of the color
     * @return String The color name
     */
    String getName() const { return _name; }

    /**
     * @brief Compute the mean square error between this color and the passed in color.
     *
     * Lower values indicate a closer match. Uses long arithmetic to avoid
     * overflow from squaring byte differences.
     *
     * See: https://en.wikipedia.org/wiki/Mean_squared_error
     *
     * @param red   Red component to compare (0–255)
     * @param green Green component to compare (0–255)
     * @param blue  Blue component to compare (0–255)
     * @return long MSE across the three channels
     */
    long computeMse(byte red, byte green, byte blue) const {
      return (long) ((
        (long)(red - _red) * (red - _red) +
        (long)(green - _green) * (green - _green) +
        (long)(blue - _blue) * (blue - _blue)) / 3);
    }

    /**
     * @brief Find the closest color in a palette to the given RGB value.
     *
     * Compares each palette entry using mean squared error and returns
     * a pointer to the best match.
     *
     * @param colorNames  Array of ColorName entries (the palette)
     * @param numColors   Number of entries in the palette
     * @param red         Red component to match (0–255)
     * @param green       Green component to match (0–255)
     * @param blue        Blue component to match (0–255)
     * @param debug       If true, print match scores to Serial (default false)
     * @return Pointer to the closest ColorName in the palette
     */
    static ColorName* getClosestColorName(ColorName colorNames[], unsigned int numColors,
                                          byte red, byte green, byte blue,
                                          bool debug = false) {
      if (numColors == 0) {
        return NULL;
      }

      if (debug) {
        Serial.println((String)"Checking best match for (R, G, B): (" +
                        red + ", " + green + ", " + blue + ")");
      }

      ColorName* _bestMatchColorName = NULL;
      long bestMatchDistance = 65535;
      for (unsigned int i = 0; i < numColors; i++) {
        ColorName* curColorName = &colorNames[i];
        if (_bestMatchColorName == NULL) {
          _bestMatchColorName = &colorNames[0];
        }

        long meanSquareError = curColorName->computeMse(red, green, blue);

        if (debug) {
          Serial.println("Match score: " + curColorName->getName() + " with MSE: " + meanSquareError);
        }

        if (meanSquareError < bestMatchDistance) {
          _bestMatchColorName = &colorNames[i];
          bestMatchDistance = meanSquareError;

          if (debug) {
            Serial.println("New best match: " + _bestMatchColorName->getName() +
                           " with MSE: " + meanSquareError);
          }
        }
      }
      return _bestMatchColorName;
    }
};
