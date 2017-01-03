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

#include "PlaneSpotter.h"

PlaneSpotter::PlaneSpotter(Adafruit_ILI9341* tft, GeoMap* geoMap) {
  tft_ = tft;
  geoMap_ = geoMap;
}

void PlaneSpotter::copyProgmemToSpiffs(const uint8_t *data, unsigned int length, String filename) {
  File f = SPIFFS.open(filename, "w+");
  uint8_t c;
  for(int i = 0; i < length; i++) {
    c = pgm_read_byte(data + i);
    f.write(c);
  }
  f.close();
}


void PlaneSpotter::drawSPIFFSJpeg(String filename, int xpos, int ypos) {
  
  Serial.println(filename);
  char buffer[filename.length() + 1];
  filename.toCharArray(buffer, filename.length() + 1);
  JpegDec.decodeFile(buffer);
  renderJPEG(xpos, ypos);
  
}

void PlaneSpotter::renderJPEG(int xpos, int ypos) {

  uint16_t  *pImg;
  uint16_t mcu_w = JpegDec.MCUWidth;
  uint16_t mcu_h = JpegDec.MCUHeight;
  uint32_t mcu_pixels = mcu_w * mcu_h;
  uint32_t drawTime = millis();

  while( JpegDec.read()){
    
    pImg = JpegDec.pImage;
    int mcu_x = JpegDec.MCUx * mcu_w + xpos;
    int mcu_y = JpegDec.MCUy * mcu_h + ypos;
    if ( ( mcu_x + mcu_w) <= tft_->width() && ( mcu_y + mcu_h) <= tft_->height()){
      
      tft_->setWindow(mcu_x, mcu_y, mcu_x + mcu_w - 1, mcu_y + mcu_h - 1);
      uint32_t count = mcu_pixels;
      while (count--) {tft_->pushColor(*pImg++);}
      // Push all MCU pixels to the TFT window, ~18% faster to pass an array pointer and length to the library
      //tft_->pushColor16(pImg, mcu_pixels); //  To be supported in HX8357 library at a future date

    }

    else if( ( mcu_y + mcu_h) >= tft_->height()) JpegDec.abort();
  
  }


}

void PlaneSpotter::drawAircraftHistory(AircraftHistory history) {
    Coordinates lastCoordinates;
    
    for (int j = 0; j < min(history.counter, MAX_HISTORY); j++) {

      int historyIndex = (history.counter - j - 1) % MAX_HISTORY;
      Coordinates coordinates = history.coordinates[historyIndex];
      CoordinatesPixel p1 = geoMap_->convertToPixel(coordinates);

      if (j > 0) {
        CoordinatesPixel p2 = geoMap_->convertToPixel(lastCoordinates);

        tft_->drawLine(p1.x, p1.y, p2.x, p2.y, TFT_BLUE);
      }
      lastCoordinates = coordinates;
      // Serial.println(String(j) + ". " + String(historyIndex) + ". " + String(coordinates.lat) + ", " + String(coordinates.lon));
    }
}

void PlaneSpotter::drawPlane(Aircraft aircraft, boolean isSpecial) {
  Coordinates coordinates;
  coordinates.lon = aircraft.lon;
  coordinates.lat = aircraft.lat;  
  CoordinatesPixel p = geoMap_->convertToPixel(coordinates);
  tft_->setTextWrap(false);
  tft_->setTextColor(TFT_WHITE, TFT_BLACK);
  tft_->setCursor(p.x + 8,p.y - 5);
  tft_->print(aircraft.call);
  int planeDotsX[planeDots_];
  int planeDotsY[planeDots_];
  //isSpecial = false;
  for (int i = 0; i < planeDots_; i++) {
    planeDotsX[i] = cos((-450 + (planeDeg_[i] + aircraft.heading)) * PI / 180) * planeRadius_[i] + p.x; 
    planeDotsY[i] = sin((-450 + (planeDeg_[i] + aircraft.heading)) * PI / 180) * planeRadius_[i] + p.y; 
  }
  if (isSpecial) {
    tft_->fillTriangle(planeDotsX[0], planeDotsY[0], planeDotsX[1], planeDotsY[1], planeDotsX[2], planeDotsY[2], TFT_RED);
    tft_->fillTriangle(planeDotsX[2], planeDotsY[2], planeDotsX[3], planeDotsY[3], planeDotsX[4], planeDotsY[4], TFT_RED);
  } else {
      for (int i = 1; i < planeDots_; i++) {
        tft_->drawLine(planeDotsX[i], planeDotsY[i], planeDotsX[i-1], planeDotsY[i-1], TFT_RED);
      }
  }
}

void PlaneSpotter::drawInfoBox(Aircraft closestAircraft) {
  tft_->fillRect(0, geoMap_->getMapHeight() + 10, tft_->width(), tft_->height() - geoMap_->getMapHeight()-10, TFT_BLACK);
  tft_->setTextWrap(true);
  tft_->setCursor(0, geoMap_->getMapHeight() + 12);
  tft_->setTextColor(TFT_YELLOW);
  tft_->print(closestAircraft.call);
  tft_->setTextColor(TFT_GREEN);
  tft_->print(" Speed: " + String(closestAircraft.speed, 0));
  tft_->setTextColor(TFT_YELLOW);
  tft_->print(" Dst: " + String(closestAircraft.distance, 2) + "km");
  tft_->setTextColor(TFT_WHITE);
  tft_->print(" Alt: " + closestAircraft.altitude + "ft\n");
  tft_->setTextColor(TFT_WHITE);
  tft_->print("Type: " + closestAircraft.aircraftType);
  tft_->setTextColor(TFT_YELLOW);
  tft_->print(" Heading: " + String(closestAircraft.heading, 0));
  tft_->setTextColor(TFT_GREEN);
  if (closestAircraft.fromShort != "" && closestAircraft.toShort != "") {
    tft_->print("\nFrom: " + closestAircraft.fromShort + "=>" + closestAircraft.toShort);
  }
 
}


