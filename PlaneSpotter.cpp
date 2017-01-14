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
      
      tft_->setAddrWindow(mcu_x, mcu_y, mcu_x + mcu_w - 1, mcu_y + mcu_h - 1);
      uint32_t count = mcu_pixels;
      while (count--) {tft_->pushColor(*pImg++);}
      // Push all MCU pixels to the TFT window, ~18% faster to pass an array pointer and length to the library
      //tft_->pushColor16(pImg, mcu_pixels); //  To be supported in HX8357 library at a future date

    }

    else if( ( mcu_y + mcu_h) >= tft_->height()) JpegDec.abort();
  
  }


}

void PlaneSpotter::drawAircraftHistory(Aircraft aircraft, AircraftHistory history) {
    Coordinates lastCoordinates;
    lastCoordinates.lat = aircraft.lat;
    lastCoordinates.lon = aircraft.lon;
    for (int j = 0; j < min(history.counter, MAX_HISTORY); j++) {

      AircraftPosition position = history.positions[j];
      Coordinates coordinates = position.coordinates;
      CoordinatesPixel p1 = geoMap_->convertToPixel(coordinates);

      CoordinatesPixel p2 = geoMap_->convertToPixel(lastCoordinates);
      uint16_t color = heightPalette_[min(position.altitude / 4000, 9)];
      tft_->drawLine(p1.x, p1.y, p2.x, p2.y, color);
      tft_->drawLine(p1.x+1, p1.y+1, p2.x+1, p2.y+1, color);

      lastCoordinates = coordinates;
      // Serial.println(String(j) + ". " + String(historyIndex) + ". " + String(coordinates.lat) + ", " + String(coordinates.lon));
    }
}

void PlaneSpotter::drawPlane(Aircraft aircraft, boolean isSpecial) {
  Coordinates coordinates;
  coordinates.lon = aircraft.lon;
  coordinates.lat = aircraft.lat;  
  CoordinatesPixel p = geoMap_->convertToPixel(coordinates);
  
  setTextColor(TFT_WHITE, TFT_BLACK);
  setTextAlignment(LEFT);
  drawString(p.x + 8,p.y - 5, aircraft.call);
  
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
  int line1 = geoMap_->getMapHeight() + 9;
  int line2 = geoMap_->getMapHeight() + 18;
  int line3 = geoMap_->getMapHeight() + 27;
  int line4 = geoMap_->getMapHeight() + 36;
  int line5 = geoMap_->getMapHeight() + 45;
  int rightTab0 = tft_->getWidth() - 3;
  int leftTab1 = 0;
  int leftTab2 = 40;
  int leftTab3 = tft_->getWidth() / 2;
  int leftTab4 = leftTab3 + 40;

  //tft_->setFont(&Dialog_plain_9);

  tft_->fillRect(0, geoMap_->getMapHeight(), tft_->width(), tft_->height() - geoMap_->getMapHeight(), TFT_BLACK);
  if (closestAircraft.call != "") {
    setTextAlignment(LEFT);
    setTextColor(TFT_GREEN);
    drawString(leftTab1, line1, "Call:");
    setTextColor(TFT_WHITE);
    drawString(leftTab2, line1, closestAircraft.call);

    setTextColor(TFT_GREEN);
    drawString(leftTab3, line1, "Type:");
    setTextColor(TFT_WHITE);
    drawString(leftTab4, line1, closestAircraft.aircraftType);
    
    setTextColor(TFT_GREEN);
    drawString(leftTab1, line2, "Alt:");
    setTextColor(TFT_WHITE);
    drawString(leftTab2, line2, String(closestAircraft.altitude) + "ft");

    setTextColor(TFT_GREEN);
    drawString(leftTab3, line2, "Head: ");
    setTextColor(TFT_WHITE);
    drawString(leftTab4, line2, String(closestAircraft.heading, 0));
  
    setTextColor(TFT_GREEN);
    drawString(leftTab1, line3, "Dst:");
    setTextColor(TFT_WHITE);
    drawString(leftTab2, line3, String(closestAircraft.distance, 2) + "km");
    setTextColor(TFT_GREEN);
    drawString(leftTab3, line3, "Spd:");
    setTextColor(TFT_WHITE);
    drawString(leftTab4, line3, String(closestAircraft.speed, 0) + "kn");
  
    if (closestAircraft.fromShort != "" && closestAircraft.toShort != "") {
      setTextColor(TFT_GREEN);
      drawString(leftTab1, line4, "From:");
      setTextColor(TFT_WHITE);
      drawString(leftTab2, line4, closestAircraft.from);
      setTextColor(TFT_GREEN);
      drawString(leftTab1, line5, "To:");
      setTextColor(TFT_WHITE);
      drawString(leftTab2, line5, closestAircraft.to);
    }
  }
 
}

void PlaneSpotter::drawMenu() {
  //tft_->setFont(&Dialog_plain_16);
  String commands[] = {"Pan", "Zoom+", "Zoom-", "Track"};
  int numberOfCommands = 4;
  tft_->fillScreen(TFT_BLACK);
  int fontHeight = 16;
  int buttonHeight = 40;
  for (int i = 0; i < numberOfCommands; i++) {
    tft_->drawFastHLine(0, i * buttonHeight, tft_->getWidth(), TFT_WHITE);
    drawString(10, i * buttonHeight + (buttonHeight - fontHeight) / 2, commands[i]); 
  }
}

void PlaneSpotter::drawString(int x, int y, char *text) {
  int16_t x1, y1;
  uint16_t w, h;
  tft_->setTextWrap(false);
  tft_->getTextBounds(text, 0, 0, &x1, &y1, &w, &h);
  switch (alignment_) {
    case LEFT:
      x1 = x;
      break;
    case CENTER:
      x1 = x - w / 2;
      break;
    case RIGHT:
      x1 = x - w;
      break;
  }
  if (textColor_ != backgroundColor_) {
    tft_->fillRect(x1, y - h -1, w + 2, h + 3, backgroundColor_);
  }
  tft_->setCursor(x1, y);
  tft_->print(text);
}

void PlaneSpotter::drawString(int x, int y, String text) {
  char buf[text.length()+2];
  text.toCharArray(buf, text.length() + 1);
  drawString(x, y, buf);
}

void PlaneSpotter::setTextColor(uint16_t c) {
  setTextColor(c, c);
}
void PlaneSpotter::setTextColor(uint16_t c, uint16_t bg) {
  textColor_ = c;
  backgroundColor_ = bg;
  tft_->setTextColor(textColor_, backgroundColor_);
}

void PlaneSpotter::setTextAlignment(TextAlignment alignment) {
  alignment_ = alignment;
}

void PlaneSpotter::setTouchScreen(XPT2046_Touchscreen* touchScreen) {
  touchScreen_ = touchScreen;
}

void PlaneSpotter::setTouchScreenCalibration(uint16_t minX, uint16_t minY, uint16_t maxX, uint16_t maxY) {
  minX_ = minX;
  minY_ = minY;
  maxX_ = maxX;
  maxY_ = maxY;
}

CoordinatesPixel PlaneSpotter::getTouchPoint() {
    TS_Point pt = touchScreen_->getPoint();
    CoordinatesPixel p;
    p.x = tft_->getWidth() * (pt.x - minX_) / (maxX_ - minX_);
    p.y = tft_->getHeight() * (pt.y - minY_) / (maxY_ - minY_);
    return p;
}

