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

#pragma once

#include <Arduino.h>
#include <JsonListener.h>
#include <JsonStreamingParser.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include "GeoMap.h"

#define MAX_AIRCRAFTS 10
#define MAX_HISTORY 20
#define MAX_HISTORY_TEMP 80

#define MAX_AGE_MILLIS 15000
#define min(a,b) ((a)<(b)?(a):(b))

struct AircraftPosition {
  int altitude;
  Coordinates coordinates;
};

struct AircraftHistory {
  String call;
  AircraftPosition positions[MAX_HISTORY];
  int counter;
};

struct Aircraft {
    String from;
    String fromCode;
    String fromShort;
    String to;
    String toCode;
    String toShort;
    double speed;
    double lat;
    double lon;
    uint16_t altitude;
    double distance;
    String aircraftType;
    String operatorCode;
    double heading;
    String icao;
    String call;
    boolean posStall;
};




class AdsbExchangeClient: public JsonListener {
  private:
    int counter = 0;
    int index = 0;
    String currentKey = "";
    Aircraft aircrafts[MAX_AIRCRAFTS];
    AircraftHistory histories[MAX_AIRCRAFTS];
    AircraftPosition positionTemp[MAX_HISTORY_TEMP];
    long lastSightingMillis = 0;
    int trailIndex = 0;

  public:
    AdsbExchangeClient();

    void updateVisibleAircraft(String searchQuery);

    Aircraft getAircraft(int i);

    AircraftHistory getAircraftHistory(int i);
    
    int getNumberOfAircrafts();

    Aircraft getClosestAircraft(Coordinates coordinates);

    virtual void whitespace(char c);

    virtual void startDocument();

    virtual void key(String key);

    virtual void value(String value);

    virtual void endArray();

    virtual void endObject();

    virtual void endDocument();

    virtual void startArray();

    virtual void startObject();

};
