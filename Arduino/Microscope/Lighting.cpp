/*
  Lighting.cpp - Library for implementing lighting on the OpenLabTools microscope
  Written by James Ritchie for OpenLabTools
  github.com/OpenLabTools/Microscope
*/

#include "Arduino.h"
#include <Adafruit_NeoPixel.h>
#include "Lighting.h"

#define RING_PIN 12
#define STAGE_LED 11

Adafruit_NeoPixel ring = Adafruit_NeoPixel(16, RING_PIN, NEO_GRB + NEO_KHZ800);

Lighting::Lighting()
{
  
}

void Lighting::begin()
{
  ring.begin();
  pinMode(STAGE_LED, OUTPUT);
}

void Lighting::loop()
{
  ring.show();
}

void Lighting::setRingColour(uint32_t rgb)
{
  for(uint16_t i=0;i<16;i++) {
    ring.setPixelColor(i, rgb);
  }
}
    
void Lighting::setRingBrightness(uint8_t b)
{
  ring.setBrightness(b);
}

void Lighting::setStageLEDBrightness(uint8_t b)
{
  analogWrite(STAGE_LED, b);
}


