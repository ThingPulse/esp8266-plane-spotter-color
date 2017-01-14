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

#include <Arduino.h>
#include <Adafruit_GFX.h>

//SPIFFS stuff
#include <FS.h>
#include <SPI.h>

// Wifi Libraries
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>




// Easy Wifi Setup
#include <WiFiManager.h>

// Go to settings to change important parameters
#include "settings.h"

// Project libraries
#include "WifiLocator.h"
#include "PlaneSpotter.h"
#include "ILI9341.h"
#include "artwork.h"
#include "AdsbExchangeClient.h"
#include "GeoMap.h"

// Initialize the TFT
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC);
WifiLocator locator;
AdsbExchangeClient adsbClient;
GeoMap geoMap(MapProvider::Google, GOOGLE_API_KEY, MAP_WIDTH, MAP_HEIGHT);
//GeoMap geoMap(MapProvider::MapQuest, MAP_QUEST_API_KEY, MAP_WIDTH, MAP_HEIGHT);
PlaneSpotter planeSpotter(&tft, &geoMap);


XPT2046_Touchscreen ts(TOUCH_CS);  // Param 2 - NULL - No interrupts
//XPT2046_Touchscreen ts(TOUCH_CS, 255);  // Param 2 - 255 - No interrupts
//XPT2046_Touchscreen ts(TOUCH_CS, TOUCH_IRQ);  // Param 2 - Touch IRQ Pin - interrupt enabled polling


Coordinates mapCenter;

// Check http://www.virtualradarserver.co.uk/Documentation/Formats/AircraftList.aspx
// to craft this query to your needs
// lat=47.424341887&lng=8.56877803&fDstL=0&fDstU=10&fAltL=0&fAltL=1500&fAltU=10000
//const String QUERY_STRING = "fDstL=0&fDstU=20&fAltL=0&fAltL=1000&fAltU=10000";
// airport zürich is on 1410ft => hide landed airplanes
const String QUERY_STRING = "fAltL=1500&trFmt=sa";

void downloadCallback(String filename, uint32_t bytesDownloaded, uint32_t bytesTotal, boolean isFirstCall);
ProgressCallback _downloadCallback = downloadCallback;

void updatePlanesAndDrawMap();
void calibrateTouchScreen();

Coordinates northWestBound;
Coordinates southEastBound;

long millisAtLastUpdate = 0;
long millisAtLastTouch = 0;
uint8_t currentZoom = MAP_ZOOM;


void setup() {

  // Start serial communication
  Serial.begin(115200);
  Serial.println("Free Heap: " + String(ESP.getFreeHeap()));
  // The LED pin needs to set HIGH
  // Use this pin to save energy
  pinMode(LED_PIN, D8);
  digitalWrite(LED_PIN, HIGH);

  // Init TFT
  tft.begin();
  tft.setRotation(3);  // landscape
  tft.cp437(false);
  tft.setFont(&Dialog_plain_9);
  tft.fillScreen(TFT_BLACK);
  planeSpotter.setTextColor(TFT_WHITE, TFT_BLACK);
  planeSpotter.setTextAlignment(CENTER);
  planeSpotter.drawString(160, 200, "     Loading Splash...     ");

  // Init file system
  if (!SPIFFS.begin()) { Serial.println("initialisation failed!"); return;}
  //SPIFFS.format();

  // copy files from code to SPIFFS
  planeSpotter.copyProgmemToSpiffs(splash, splash_len, "/splash.jpg");
  planeSpotter.copyProgmemToSpiffs(plane, plane_len, "/plane.jpg");
  planeSpotter.drawSPIFFSJpeg("/splash.jpg", 30, 75);
  planeSpotter.setTextColor(TFT_WHITE, TFT_BLACK);
  planeSpotter.setTextAlignment(CENTER);
  planeSpotter.drawString(160, 200, "     Connecting to WiFi..     ");
  
  
  WiFiManager wifiManager;
  // Uncomment for testing wifi manager
  //wifiManager.resetSettings();
  
  //wifiManager.setAPCallback(configModeCallback);

  //or use this for auto generated name ESP + ChipID
  wifiManager.autoConnect();

  // Set center of the map by using a WiFi fingerprint
  // Hardcode the values if it doesn't work or you want another location
  locator.updateLocation();
  mapCenter.lat = locator.getLat().toFloat();
  mapCenter.lon = locator.getLon().toFloat();
  mapCenter.lat = 47.424341887;
  mapCenter.lon = 8.56877803;

  planeSpotter.setTextColor(TFT_WHITE, TFT_BLACK);
  planeSpotter.setTextAlignment(CENTER);
  planeSpotter.drawString(160, 200, "          Loading map...          ");
  geoMap.downloadMap(mapCenter, currentZoom, _downloadCallback);

  northWestBound = geoMap.convertToCoordinates({-MAP_REQUEST_MARGIN,-MAP_REQUEST_MARGIN});
  southEastBound = geoMap.convertToCoordinates({MAP_WIDTH + MAP_REQUEST_MARGIN, MAP_HEIGHT + MAP_REQUEST_MARGIN});

  ts.begin();
  planeSpotter.setTouchScreen(&ts);
  
}


void loop() {
  boolean isTouched = ts.touched();
  if(millis() - millisAtLastUpdate > 5000 && !isTouched) {
    updatePlanesAndDrawMap();
    millisAtLastUpdate = millis();
  }

  CoordinatesPixel pt = planeSpotter.getTouchPoint();
  if (isTouched && millis() - millisAtLastTouch > 4000) {
    //tft.fillCircle(pt.x, pt.y, 2, TFT_RED);
    //currentZoom--;
    /*mapCenter = geoMap.convertToCoordinates(pt);
    geoMap.downloadMap(mapCenter, currentZoom, _downloadCallback);
    northWestBound = geoMap.convertToCoordinates({-MAP_REQUEST_MARGIN,-MAP_REQUEST_MARGIN});
    southEastBound = geoMap.convertToCoordinates({MAP_WIDTH + MAP_REQUEST_MARGIN, MAP_HEIGHT + MAP_REQUEST_MARGIN});
    millisAtLastTouch = millis();*/
    planeSpotter.drawMenu();
  }
  
  delay(20);

}



void downloadCallback(String filename, uint32_t bytesDownloaded, uint32_t bytesTotal, boolean isFirstCall) {
  if (isFirstCall) {
      tft.fillRect(0, geoMap.getMapHeight(), tft.getWidth(), tft.getHeight() - geoMap.getMapHeight(), TFT_BLACK);
  }
  Serial.println(String(bytesDownloaded) + " / " + String(bytesTotal));
  int width = 320;
  int progress = width * bytesDownloaded / bytesTotal;
  tft.fillRect(10, 220, progress, 5, TFT_WHITE);
  planeSpotter.drawSPIFFSJpeg("/plane.jpg", 15 + progress, 220 - 15);
}

void updatePlanesAndDrawMap() {
  Serial.println("Heap: " + String(ESP.getFreeHeap()));
  adsbClient.updateVisibleAircraft(QUERY_STRING + "&lat=" + String(mapCenter.lat, 6) + "&lng=" + String(mapCenter.lon, 6) + "&fNBnd=" + String(northWestBound.lat, 9) + "&fWBnd=" + String(northWestBound.lon, 9) + "&fSBnd=" + String(southEastBound.lat, 9) + "&fEBnd=" + String(southEastBound.lon, 9));
  
  long startMillis = millis();
  planeSpotter.drawSPIFFSJpeg(geoMap.getMapName(), 0, 0);

  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  Aircraft closestAircraft = adsbClient.getClosestAircraft(mapCenter);
  for (int i = 0; i < adsbClient.getNumberOfAircrafts(); i++) {
    Aircraft aircraft = adsbClient.getAircraft(i);
    AircraftHistory history = adsbClient.getAircraftHistory(i);
    planeSpotter.drawAircraftHistory(aircraft, history);
    planeSpotter.drawPlane(aircraft, aircraft.call == closestAircraft.call);


  }
  planeSpotter.drawInfoBox(closestAircraft);

  // Draw center of map
  CoordinatesPixel p = geoMap.convertToPixel(mapCenter);
  tft.fillCircle(p.x, p.y, 2, TFT_BLUE); 

  Serial.println(String(millis()-startMillis) + "ms for drawing");
}




