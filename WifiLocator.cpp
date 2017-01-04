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

Base64 encoding code by Rene Nyfenegger:
http://www.adp-gmbh.ch/cpp/common/base64.html

*/

#include "WifiLocator.h"

WifiLocator::WifiLocator() {
  
}

void WifiLocator::updateLocation() {
  int n = min(WiFi.scanNetworks(false,true), MAX_SSIDS);
  String multiAPString = "";
  if (n > 0) {
    for (int i = 0; i < n; i++) {
      if (i > 0) {
        multiAPString += ",";
      }
      multiAPString += WiFi.BSSIDstr(i) + "," + WiFi.RSSI(i);
    }
    Serial.println(multiAPString);
    char multiAPs[multiAPString.length() + 1];
    multiAPString.toCharArray(multiAPs, multiAPString.length());
    multiAPString = encodeBase64(multiAPs, multiAPString.length());
    Serial.println(multiAPString);
    doUpdate("/wifi?v=1.1&search=" + multiAPString);
  }
}

void WifiLocator::updateLocation(String bssid) {
  doUpdate("/wifi?v=1.1&bssid=" + bssid);
}

void WifiLocator::doUpdate(String query) {
    JsonStreamingParser parser;
  parser.setListener(this);
  WiFiClient client;
  const int httpPort = 80;
  // http://api.mylnikov.org/geolocation/wifi?v=1.1&data=open&bssid=00:0C:42:1F:65:E9
  if (!client.connect("api.mylnikov.org", httpPort)) {
    Serial.println("connection failed");
    return;
  }
 
  Serial.print("Requesting URL: ");
  Serial.println(query);

  // This will send the request to the server
  client.print(String("GET ") + query + " HTTP/1.1\r\n" +
               "Host: api.mylnikov.org\r\n" +
               "Connection: close\r\n\r\n");
  int retryCounter = 0;
  while(!client.available()) {
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
        Serial.print(c);
        isBody = true;
      }
      if (isBody) {
        parser.parse(c);
      }
    }
  }  
}

void WifiLocator::key(String key) {
  key_ = key;
}

void WifiLocator::value(String value) {
  if (key_ == "result") {
    result_ = value;
  }
  if (key_ == "lat") {
    lat_ = value;
  }
  if (key_ == "lon") {
    lon_ = value;
  }
  if (key_ == "range") {
    range_ = value;
  }
  Serial.println("Key " + key_ + ", value: " + value);
}

String WifiLocator::getLon() {
  return lon_;
}

String WifiLocator::getLat() {
  return lat_;
}

String WifiLocator::getRange() {
  return range_;
}

String WifiLocator::getResult() {
  return result_;
}

void WifiLocator::whitespace(char c) {
  
}

void WifiLocator::startDocument() {
  
}

void WifiLocator::endArray() {
  
}

void WifiLocator::endObject() {
  
}

void WifiLocator::endDocument() {
  
}

void WifiLocator::startArray() {
  
}

void WifiLocator::startObject() {
  
}

/*
 * Base64 code by Rene Nyfenegger:
 * http://www.adp-gmbh.ch/cpp/common/base64.html
 */
String WifiLocator::encodeBase64(char* bytes_to_encode, unsigned int in_len) {
  String ret;
  int i = 0;
  int j = 0;
  unsigned char char_array_3[3];
  unsigned char char_array_4[4];

  while (in_len--) {
    char_array_3[i++] = *(bytes_to_encode++);
    if (i == 3) {
      char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
      char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
      char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
      char_array_4[3] = char_array_3[2] & 0x3f;

      for(i = 0; (i <4) ; i++)
        ret += base64_chars[char_array_4[i]];
      i = 0;
    }
  }

  if (i)
  {
    for(j = i; j < 3; j++)
      char_array_3[j] = '\0';

    char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
    char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
    char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
    char_array_4[3] = char_array_3[2] & 0x3f;

    for (j = 0; (j < i + 1); j++)
      ret += base64_chars[char_array_4[j]];

    while((i++ < 3))
      ret += '=';

  }

  return ret;

}

