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
#include <Arduino.h>
#include <FS.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266HTTPClient.h>

#define MAPQUEST_TILE_LENGTH 256.0

typedef void (*ProgressCallback)(String fileName, uint32_t bytesDownloaded, uint32_t bytesTotal, boolean isFirstCall);

enum MapProvider {
  MapQuest,
  Google  
};

struct Coordinates {
  double lat;
  double lon;
};

struct CoordinatesPixel {
  int x;
  int y;
};

struct CoordinatesTiles {
  double x;
  double y;
};

class GeoMap {
  private:
    MapProvider mapProvider_;
    String apiKey_;
    int mapWidth_, mapHeight_;
    long zoom_;
    Coordinates mapCenter_;

  public:
    GeoMap(MapProvider mapProvider, String apiKey, int mapWidth, int mapHeight);
    void downloadMap(Coordinates mapCenter, int zoom, ProgressCallback progressCallback);
    void downloadMap(Coordinates mapCenter, int zoom);
    String getMapName();
    CoordinatesPixel convertToPixel(Coordinates coordinates);
    Coordinates convertToCoordinates(CoordinatesPixel coordinatesPixel);
    CoordinatesTiles convertToTiles(Coordinates coordinates);
    Coordinates convertToCoordinatesFromTiles(CoordinatesTiles tiles);
    void downloadFile(String url, String filename, ProgressCallback progressCallback);
    void downloadFile(String url, String filename);
    int getMapWidth();
    int getMapHeight();
  
};

