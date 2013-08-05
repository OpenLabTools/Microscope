/*
  Stage.h - Library for controlling the stage on the OpenLabTools microscope
  Written by James Ritchie for OpenLabTools
  github.com/OpenLabTools/Microscope
*/
#include "Arduino.h"
#include <Wire.h>
#include <Adafruit_MotorShield.h>
#include <AccelStepper.h>

#ifndef Stage_h
#define Stage_h

class Stage
{
  public:
    Stage();
    
    void begin();
    void loop();
    void zMove(long steps);
    void zMoveTo(long position);
    void calibrate();
    static const int z_ulimit_switch = 4;
    static const int z_llimit_switch = 5;
    boolean calibrated;
    long z_length;
    
    static const int up_switch_pin = 2;
    static const int down_switch_pin = 3;
    boolean manual_control;
    void manualControl();
    
  private:
    AccelStepper _z_stepper;
};
  
#endif

