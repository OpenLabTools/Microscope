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
    
    static const int up_switch_pin = 22;
    static const int down_switch_pin = 24;
    static const int z_ulimit_switch = 26;
    static const int z_llimit_switch = 28;
    
    boolean manual_control;
    boolean calibrated;
    long z_length;
    
    Stage();   
    void begin();
    void loop();
    void zMove(long steps);
    void zMoveTo(long position);
    void calibrate();
    void manualControl();
    long getZPosition();
    long getZDistanceToGo();
    
  private:
    AccelStepper _z_stepper;
};
  
#endif

