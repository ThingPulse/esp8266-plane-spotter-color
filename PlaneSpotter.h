/**The MIT License (MIT)

Copyright (c) 2015 by Daniel Eichhorn

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

See more at https://blog.squix.org


*/

#pragma once

#include <FS.h>
// JPEG decoder library
#include <JPEGDecoder.h>
#include "AdsbExchangeClient.h"
#include "ILI9341.h"
#include "GeoMap.h"

#define TFT_BLACK   0x0000
#define TFT_BLUE    0x001F
#define TFT_RED     0xF800
#define TFT_GREEN   0x07E0
#define TFT_CYAN    0x07FF
#define TFT_MAGENTA 0xF81F
#define TFT_YELLOW  0xFFE0  
#define TFT_WHITE   0xFFFF

enum TextAlignment {
  LEFT, CENTER, RIGHT
};

class PlaneSpotter {
  public:
    PlaneSpotter(Adafruit_ILI9341* tft, GeoMap* geoMap);
    void copyProgmemToSpiffs(const uint8_t *data, unsigned int length, String filename);

    void drawSPIFFSJpeg(String filename, int xpos, int ypos);
    void renderJPEG(int xpos, int ypos);

    void drawPlane(Aircraft aircraft, boolean isSpecial);

    void drawInfoBox(Aircraft closestAircraft);

    void drawAircraftHistory(AircraftHistory history);

    void drawString(int x, int y, char *text);
    
    void drawString(int x, int y, String text);
    
    void setTextAlignment(TextAlignment alignment);
    
    void setTextColor(uint16_t c);
    
    void setTextColor(uint16_t c, uint16_t bg);

  private:
    Adafruit_ILI9341* tft_;
    GeoMap* geoMap_;
    // Shape of the plane
    // The points are defined as degree on a circle, the first array are the degrees, 
    // the second the radius of the circle
    int planeDeg_[5] = {0, 130, 180, 230, 0};
    int planeRadius_[5] = {10, 10, 3, 10, 10};
    int planeDots_ = 5;
    TextAlignment alignment_ = LEFT;
    uint16_t textColor_;
    uint16_t backgroundColor_;




  
};

