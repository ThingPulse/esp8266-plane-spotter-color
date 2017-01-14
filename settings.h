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

// Tft library connectors
#define TFT_DC D2
#define TFT_CS D1
#define TOUCH_CS D3
#define TOUCH_IRQ  D4
#define LED_PIN D8

// Needed for loading the map. If you want to be save better get your own key here:
// https://developer.mapquest.com/plan_purchase/steps/business_edition/business_edition_free/register
#define MAP_QUEST_API_KEY "r19I8UVBfwIkmE4EZR9S6yMR43eMiRDZ"

// Need for loading map by google static map api. If you want to be save better create one here
// https://developers.google.com/maps/documentation/static-maps/get-api-key?hl=de
#define GOOGLE_API_KEY "AIzaSyBw0G8jCBry0IATNmysuyPd2fBblndS3jU"

#define MAP_ZOOM 11
#define MAP_WIDTH 320
#define MAP_HEIGHT 192
// How many pixels outside the visible map should planes be requested
#define MAP_REQUEST_MARGIN 40


