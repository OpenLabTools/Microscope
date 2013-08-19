/*
  Lighting.h - Library for implementing lighting on the OpenLabTools microscope
  Written by James Ritchie for OpenLabTools
  github.com/OpenLabTools/Microscope
*/

#include "Arduino.h"
#include <Adafruit_NeoPixel.h>

#ifndef Lighting_h
#define Lighting_h

class Lighting
{
  public:
    Lighting();
    void begin();
    void loop();
    void setRingColour(uint32_t rgb);
    void setRingBrightness(uint8_t b);
    void setStageLEDBrightness(uint8_t b);
};


#endif


