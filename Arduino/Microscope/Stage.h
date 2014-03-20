/*
  Stage.h - Library for controlling the stage on the OpenLabTools microscope
  Written by James Ritchie for OpenLabTools
  github.com/OpenLabTools/Microscope
*/
#include "Arduino.h"
#include <Wire.h>
#include <Adafruit_MotorShield.h>
#include <AccelStepper.h>
#include "TouchScreen.h"

#ifndef Stage_h
#define Stage_h

//Define pins
#define Z_ULIMIT_SWITCH 5
#define Z_LLIMIT_SWITCH 4

//Define motor selections
#define X_STEPPER 0
#define Y_STEPPER 1
#define Z_STEPPER 2

//Define Touchscreen pins
#define YP A2  // must be an analog pin, use "An" notation!
#define XM A3  // must be an analog pin, use "An" notation!
#define YM A0   // can be a digital pin
#define XP A1  // can be a digital pin

class Stage
{
  public:
    boolean manual_control;
    boolean calibrated;

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

    long _x_pos;
    long _y_pos;
    long _z_pos;

    long _x_length;
    long _y_length;
    long _z_length;

    long _x_target;
    long _z_target;
    long _y_target;

    long _z_last_step;
    long _xy_last_step;

    long _z_interval;
    long _xy_interval;

    Point p;

};

#endif
