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

//Define pins
#define Z_UP_SWITCH 7
#define Z_DOWN_SWITCH 6
#define Z_ULIMIT_SWITCH 5
#define Z_LLIMIT_SWITCH 4
#define ROTARY_ENCODER_CLOCKWISE 2
#define ROTARY_ENCODER_ANTICLOCKWISE 1
#define ROTARY_ENCODER_SWITCH 0

//Define motor selections
#define X_STEPPER 0
#define Y_STEPPER 1
#define Z_STEPPER 2

class Stage
{
  public:
    boolean manual_control;
    boolean calibrated;
   
    int re_selection;
    long re_debounce_time;
    int re_last_reading;
    int re_state;
    int re_last_state;
    int re_selection_changed;
    
    Stage();
    
    void begin();
    void loop();
    void manualControl();
    
    void calibrate();
    
    long getPosition(int stepper);
    long getDistanceToGo(int stepper);
    long getLength(int stepper);
    
    void Move(int stepper, long steps);
    void MoveTo(int stepper,long position);
    
    
    
  private:
    long _x_length;
    long _y_length;
    long _z_length;  
  
    AccelStepper _x_stepper;
    AccelStepper _y_stepper;
    AccelStepper _z_stepper;
};
  
#endif

