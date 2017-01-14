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

See more at http://blog.squix.ch
*/
#include "GeoMap.h"

GeoMap::GeoMap(MapProvider mapProvider, String apiKey, int mapWidth, int mapHeight) {
  mapProvider_ = mapProvider;
  apiKey_ = apiKey;
  mapWidth_ = mapWidth;
  mapHeight_ = mapHeight;
}

int GeoMap::getMapWidth() {
  return mapWidth_;
}
int GeoMap::getMapHeight() {
  return mapHeight_;
}

void GeoMap::downloadMap(Coordinates mapCenter, int zoom) {
  downloadMap(mapCenter, zoom, nullptr);
}

void GeoMap::downloadMap(Coordinates mapCenter, int zoom, ProgressCallback progressCallback) {
  mapCenter_= mapCenter;
  zoom_ = zoom;
  switch(mapProvider_) {
    case MapProvider::MapQuest:
      downloadFile("http://open.mapquestapi.com/staticmap/v4/getmap?key=" + apiKey_ + "&type=map&scalebar=false&size=" 
        + String(mapWidth_) + "," + String(mapHeight_) + "&zoom=" + String(zoom_) + "&center="+ String(mapCenter_.lat) + "," + String(mapCenter_.lon), getMapName(), progressCallback); 
      break;
    case MapProvider::Google:
        downloadFile("http://maps.googleapis.com/maps/api/staticmap?key=" + apiKey_ + "&center="+ String(mapCenter_.lat) + "," + String(mapCenter_.lon)+"&zoom="+ String(zoom_)+"&size=" 
        + String(mapWidth_) + "x" + String(mapHeight_)+"&format=jpg-baseline&maptype=roadmap", getMapName(), progressCallback);
        break;
  }

}

String GeoMap::getMapName() {
  return "map" + String(mapCenter_.lat) + "_" + String(mapCenter_.lon) + "_" + String(zoom_) + ".jpg";
}

CoordinatesPixel GeoMap::convertToPixel(Coordinates coordinates) {
  CoordinatesTiles centerTile = convertToTiles(mapCenter_);
  CoordinatesTiles poiTile = convertToTiles(coordinates);
  CoordinatesPixel poiPixel;
  poiPixel.x = (poiTile.x - centerTile.x) * MAPQUEST_TILE_LENGTH + mapWidth_ / 2;
  poiPixel.y = (poiTile.y - centerTile.y) * MAPQUEST_TILE_LENGTH + mapHeight_ / 2;
  return poiPixel;
}

CoordinatesTiles GeoMap::convertToTiles(Coordinates coordinates) {
  
  double lon_rad = coordinates.lon * PI / 180;
  double lat_rad = coordinates.lat * PI / 180;;
  double n = pow(2.0, zoom_);

  CoordinatesTiles tile;
  tile.x = ((coordinates.lon + 180) / 360) * n;
  tile.y = (1 - (log(tan(lat_rad) + 1.0/cos(lat_rad)) / PI)) * n / 2.0;
  
  return tile;
}

Coordinates GeoMap::convertToCoordinatesFromTiles(CoordinatesTiles tiles) {
  Coordinates result;

  double n = pow(2.0, zoom_);

  
  result.lon = (360 * tiles.x / n)  - 180;
  result.lat = 180 / PI * (atan(sinh(PI * (1 - 2 * tiles.y / n))));

  return result;
}

Coordinates GeoMap::convertToCoordinates(CoordinatesPixel poiPixel) {
  CoordinatesTiles centerTile = convertToTiles(mapCenter_);
  CoordinatesTiles poiTile;
  Serial.println(String(centerTile.x, 9) + ", " + String(centerTile.y, 9));
  poiTile.x = ((poiPixel.x - (mapWidth_ / 2.0)) / MAPQUEST_TILE_LENGTH) + centerTile.x;
  poiTile.y = ((poiPixel.y - (mapHeight_ / 2.0)) / MAPQUEST_TILE_LENGTH) + centerTile.y;
  Serial.println(String(poiTile.x, 9) + ", " + String(poiTile.y, 9));
  Coordinates poiCoordinates = convertToCoordinatesFromTiles(poiTile);
  return poiCoordinates;
}


void GeoMap::downloadFile(String url, String filename) {
  downloadFile(url, filename, nullptr);
}

void GeoMap::downloadFile(String url, String filename, ProgressCallback progressCallback) {
    Serial.println("Downloading " + url + " and saving as " + filename);

    if (SPIFFS.exists(filename) == true) {
      Serial.println("File already exists. Skipping");
      //return;
    }
    // wait for WiFi connection
    boolean isFirstCall = true;
    ESP8266WiFiMulti _wifiMulti;
    if((_wifiMulti.run() == WL_CONNECTED)) {
        HTTPClient http;

        Serial.print("[HTTP] begin...\n");

        // configure server and url
        http.begin(url);

        Serial.print("[HTTP] GET...\n");
        // start connection and send HTTP header
        int httpCode = http.GET();
        if(httpCode > 0) {
            //SPIFFS.remove(filename);
            File f = SPIFFS.open(filename, "w+");
            if (!f) {
                Serial.println("file open failed");
                return;
            }
            // HTTP header has been send and Server response header has been handled
            Serial.printf("[HTTP] GET... code: %d\n", httpCode);

            // file found at server
            if(httpCode == HTTP_CODE_OK) {

                // get lenght of document (is -1 when Server sends no Content-Length header)
                int total = http.getSize();
                int len = total;
                progressCallback(filename, 0,total, true);
                // create buffer for read
                uint8_t buff[128] = { 0 };

                // get tcp stream
                WiFiClient * stream = http.getStreamPtr();

                // read all data from server
                while(http.connected() && (len > 0 || len == -1)) {
                    // get available data size
                    size_t size = stream->available();

                    if(size) {
                        // read up to 128 byte
                        int c = stream->readBytes(buff, ((size > sizeof(buff)) ? sizeof(buff) : size));

                        // write it to Serial
                        f.write(buff, c);

                        if(len > 0) {
                            len -= c;
                        }
                        progressCallback(filename, total - len,total, false);
                        isFirstCall = false;
                    }
                    delay(1);
                }

                Serial.println();
                Serial.print("[HTTP] connection closed or file end.\n");

            }
            f.close();
        } else {
            Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
        }
        
        http.end();
    }
}


