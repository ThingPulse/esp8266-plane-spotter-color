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

#include "AdsbExchangeClient.h"


AdsbExchangeClient::AdsbExchangeClient() {

}

void AdsbExchangeClient::updateVisibleAircraft(String searchQuery) {
  JsonStreamingParser parser;
  parser.setListener(this);
  WiFiClient client;

  // http://public-api.adsbexchange.com/VirtualRadar/AircraftList.json?lat=47.437691&lng=8.568854&fDstL=0&fDstU=20&fAltL=0&fAltU=5000
  const char host[] = "global.adsbexchange.com";
  String url = "/VirtualRadar/AircraftList.json?" + searchQuery;

  const int httpPort = 80;
  if (!client.connect(host, httpPort)) {
    Serial.println("connection failed");
    return;
  }


  Serial.print("Requesting URL: ");
  Serial.println(url);

  // This will send the request to the server
  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" +
               "Connection: close\r\n\r\n");

  int retryCounter = 0;
  while(!client.available()) {
    Serial.println(".");
    delay(1000);
    retryCounter++;
    if (retryCounter > 10) {
      return;
    }
  }

  int pos = 0;
  boolean isBody = false;
  char c;

  int size = 0;
  client.setNoDelay(false);
  while(client.connected()) {
    while((size = client.available()) > 0) {
      c = client.read();
      if (c == '{' || c == '[') {
        isBody = true;
      }
      if (isBody) {
        parser.parse(c);
      }
    }
  }
  endDocument();
}


void AdsbExchangeClient::whitespace(char c) {

}

void AdsbExchangeClient::startDocument() {
  counter = 0;
  index = -1;
}

void AdsbExchangeClient::key(String key) {
  currentKey = key;
}

void AdsbExchangeClient::value(String value) {
  /*String from = "";
  String to = "";
  String altitude = "";
  String aircraftType = "";
  String currentKey = "";
  String operator = "";


 "Type": "A319",
 "Mdl": "Airbus A319 112",

 "From": "LSZH Z\u00c3\u00bcrich, Zurich, Switzerland",
 "To": "LEMD Madrid Barajas, Spain",
 "Op": "Swiss International Air Lines",
 "OpIcao": "SWR",
 "Dst": 6.23,
 "Year": "1996"
 */
  if (counter >= MAX_AIRCRAFTS - 1) {
    Serial.println("Max Aircrafts reached....");
    return;
  }
  if (currentKey == "Id") {

      counter++;
      index = counter - 1;
      aircrafts[index] = {};
      histories[index] = {};
      
      trailIndex = 0;
      for (int i = 0; i < MAX_HISTORY_TEMP; i++) {
         positionTemp[i] = {};
      }
   
  } else if (currentKey == "From") {
    aircrafts[index].from = value;
    aircrafts[index].fromCode = value.substring(0,4);
    int indexOfFirstComma = value.indexOf(",");
    aircrafts[index].fromShort = value.substring(4, indexOfFirstComma);
  } else if (currentKey == "To") {
    aircrafts[index].to = value;
    aircrafts[index].toCode = value.substring(0,4);
    int indexOfFirstComma = value.indexOf(",");
    aircrafts[index].toShort = value.substring(4, indexOfFirstComma);
  } else if (currentKey == "OpIcao") {
    aircrafts[index].operatorCode = value;
  } else if (currentKey == "Dst") {
    aircrafts[index].distance = value.toFloat();
  } else if (currentKey == "Mdl") {
    aircrafts[index].aircraftType = value;
  } else if (currentKey == "Trak") {
    aircrafts[index].heading = value.toFloat();
  } else if (currentKey == "Alt") {
    aircrafts[index].altitude = value.toInt();;
  } else if (currentKey == "Lat") {
    aircrafts[index].lat = value.toFloat();
  } else if (currentKey == "Long") {
    aircrafts[index].lon = value.toFloat();
  } else if (currentKey == "Spd") {
    aircrafts[index].speed = value.toFloat();
  } else if (currentKey == "Icao") {
    aircrafts[index].icao = value;
  } else if (currentKey == "Call") {
    Serial.println("Saw " + value);
    aircrafts[index].call = value;
  } else if (currentKey == "PosStale") {
    aircrafts[index].posStall = (value == "true");
  } else if (currentKey == "Cos") {
    int tempIndex = trailIndex / 4;
    if (tempIndex < MAX_HISTORY_TEMP) {
      AircraftPosition position = positionTemp[tempIndex];
      Coordinates coordinates = position.coordinates;
      if (trailIndex % 4 == 0) {
        coordinates.lat = value.toFloat();
      } else if (trailIndex % 4 == 1) {
        coordinates.lon = value.toFloat();
      } else if (trailIndex % 4 == 3) {
        position.altitude = value.toInt();
      }
      position.coordinates = coordinates;
      positionTemp[tempIndex] = position;
      trailIndex++;
    }

  } else if (currentKey == "Trt") {
    if(aircrafts[index].posStall) {
      Serial.println("This aircraft is stalled. Ignoring it");
      counter--;
      index = counter - 1;
    }
  }

}

Aircraft AdsbExchangeClient::getAircraft(int i) {
  if (i < MAX_AIRCRAFTS) {
    return aircrafts[i];
  }
  //return nullptr;
}

AircraftHistory AdsbExchangeClient::getAircraftHistory(int i) {
  return histories[i];
}

int AdsbExchangeClient::getNumberOfAircrafts() {
  return counter;
}

Aircraft AdsbExchangeClient::getClosestAircraft(Coordinates coordinates) {
  double minDistance = 999999.0;
  Aircraft closestAircraft = aircrafts[0];
  for (int i = 0; i < getNumberOfAircrafts(); i++) {
    Aircraft currentAircraft = aircrafts[i];

    if (currentAircraft.distance < minDistance) {
      minDistance = currentAircraft.distance;
      closestAircraft = currentAircraft;
    }
  }
  return closestAircraft;
}

void AdsbExchangeClient::endArray() {
  if (counter >= MAX_AIRCRAFTS - 1) {
    Serial.println("MAx Aircrafts reached:end array");
    return;
  }
  if (currentKey == "Cos" && trailIndex > 0) {
    AircraftHistory history = {};
    uint16_t items = (trailIndex / 4);
    Serial.println("Finished history array: " + String(items) + " elements");
    int historyCounter = 0;
    for (int i = 0; i < min(items, MAX_HISTORY); i++) {
      AircraftPosition position = {};
      Coordinates coordinates = position.coordinates;
      coordinates.lat = positionTemp[items - i - 1].coordinates.lat;
      coordinates.lon = positionTemp[items - i - 1].coordinates.lon;
      position.coordinates = coordinates;
      position.altitude = positionTemp[items - i - 1].altitude;
      history.positions[i] = position;
      Serial.println(String(i) + ": " + String(items - i -1) + ", " + String(history.positions[i].coordinates.lat, 9) + ", " + String(history.positions[i].coordinates.lon, 9));
      historyCounter++;
    }
    history.call = aircrafts[index].call;
    history.counter = historyCounter;
    histories[index] = history;
    currentKey = "";
  }
}

void AdsbExchangeClient::endObject() {

}

void AdsbExchangeClient::endDocument() {
  /*Serial.println("End of document:");
  for (int i = 0; i < getNumberOfAircrafts(); i++) {
    AircraftHistory history = histories[i];
    for (int j = 0; j < history.counter; j++) {
      AircraftPosition position = history.positions[j];
      Serial.println(history.call + ": " + String(j) + ". " + String(position.coordinates.lat, 9) + ", " + String(position.coordinates.lon, 9) + ", " + String(position.altitude));
    }
  }*/
}

void AdsbExchangeClient::startArray() {

}

void AdsbExchangeClient::startObject() {

}
