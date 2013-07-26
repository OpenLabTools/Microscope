/*
  Stage.h - Library for controlling the stage on the OpenLabTools microscope
  Written by James Ritchie for OpenLabTools
  github.com/OpenLabTools/Microscope
*/
#include "Arduino.h"
#include <Wire.h>
#include <Adafruit_MotorShield.h>
#include <AccelStepper.h>

class Stage
{
  public:
    Stage();
    void begin();
    void loop();
    void zMove(int steps);
    
  private:
    AccelStepper _z_stepper;
};
  

