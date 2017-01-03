#ESP8266 Plane Spotter Color

This is the repository of the ESP8266 Plane Spotter Color. It downloads data from web APIs and displays aircrafts close
to your location on a map.

## The related blog post
https://blog.squix.org/2017/01/esp8266-planespotter-color.html

## Video
[![ESP8266 Plane Spotter Color](http://img.youtube.com/vi/4pTkoMsl1H4/0.jpg)](http://www.youtube.com/watch?v=4pTkoMsl1H4 "Plane Spotter Color")

## Features
* Beautiful startup splash screen
* Automatic geo location by using WiFi scanning. List of visible SSIDs identifies your location
* Automatic download of JPEGs from MapQuest
* Detailed information about the nearest aircraft
* Flight track: last 20 waypoints per aircraft displayed

## Planed Features
* Enable touch screen
  * Zoom in/out by button
  * shift map center
  * call location service again
  * select aircraft of interest
* only download map if center of map, scale or map type changed

## Known Issues
* Flickering with every update: not enough memory for frame/ double buffering
* Sometimes waypoints get lost
* Encoding problems when displaying airport names containing non-ASCII characters (e.g. Zürich)


## Hardware Requirements

This project was built for the following hardware:
* ESP8266 Wifi chip, especially with the Wemos D1 Mini, but all other ESP8266 modules should work as well. You can *support my blog* and buy it on my shop: https://blog.squix.org/product/d1-mini-nodemcu-lua-wifi-esp8266-development-board
* ILI9341/ XPT2046 TFT display with touch screen. At the moment the touch screen part is not used but I hope to extend it at a later time.
https://www.aliexpress.com/item/240x320-2-4-SPI-TFT-LCD-Touch-Panel-Serial-Port-Module-with-PBC-ILI9341-5-3/2031268807.html

Optionally you can get the connector PCB in the kicad sub directory. This allows for a easy soldering

## Wiring/ Schema

If you are currently prototyping this shows how to setup the connections for the above mentioned ILI9341 display

![Wiring](images/PlaneSpotterWiring.png)

![Schema](images/PlaneSpotterSchema.png)


## Libraries

Install the following libraries:

### WifiManager by tzapu

![WifiManager](images/WifiManagerLib.png)

### Json Streaming Parser by Daniel Eichhorn

![Json Streaming Parser] (images/JsonStreamingParserLib.png)

### JPEGDecoder, fork by Frederic Plante

This is (not yet?) available through the library manager. You have to download it from here and add it to the Arduino IDE
https://github.com/fredericplante/JPEGDecoder

*Attention:* You'll also have to open User_config.h in Arduino/libraries/JPEGDecoder-master and change
```
#define USE_SD_CARD
//#define USE_SPIFFS
```
into
```
//#define USE_SD_CARD
#define USE_SPIFFS
```
### Adafruit GFX by Adafruit

![Adafruit GFX Lib](images/AdafruitGFXLib.png)

## Credits

This project wouldn't be possible if not for many open source contributors. Here are some I'd like to mention:
* Frédéric Plante for his adaptations of the JPEGDecoder library
* tzapu for the WifiManager library
* Rene Nyfenegger for the base64 encoder I got from here: http://www.adp-gmbh.ch/cpp/common/base64.html
* Adafruit for the ILI9341 driver and potentially also for the original designs of the TFT display
