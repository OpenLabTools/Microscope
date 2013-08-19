/*
  Stage.cpp - Library for controlling the stage on the OpenLabTools microscope
  Written by James Ritchie for OpenLabTools
  github.com/OpenLabTools/Microscope
*/
#include "Arduino.h"
#include <Wire.h>
#include <Adafruit_MotorShield.h>
#include <AccelStepper.h>
#include "Stage.h"

//Define pins
#define Z_UP_SWITCH 7
#define Z_DOWN_SWITCH 6
#define Z_ULIMIT_SWITCH 5
#define Z_LLIMIT_SWITCH 4

// Create the motor shield object with the default I2C address
Adafruit_MotorShield afms = Adafruit_MotorShield();

// Create stepper motors for each axis (200 steps per rev)
Adafruit_StepperMotor *z_motor = afms.getStepper(200, 2); //Z-axis Port 2

//Wrapper functions around Adafruit stepper objects for use with
//AccelStepper library
void zForward() {
  z_motor->onestep(FORWARD, DOUBLE);
}
void zBackward() {
  z_motor->onestep(BACKWARD, DOUBLE);
}

Stage::Stage() {
  //Initialize AccelStepper object with wrapper functions and parameters.
  _z_stepper = AccelStepper(zForward, zBackward);
  _z_stepper.setAcceleration(1000.0);
  _z_stepper.setMaxSpeed(1000.0);
  
  calibrated = false;
  
}

void Stage::begin()
{
  //Initiliaze the motor shield
  afms.begin();
  
  //Setup pins for input
  pinMode(Z_ULIMIT_SWITCH, INPUT);
  pinMode(Z_LLIMIT_SWITCH, INPUT);
  pinMode(Z_UP_SWITCH, INPUT);
  pinMode(Z_DOWN_SWITCH, INPUT);
}

void Stage::loop()
{
  
  //Check for manual commands
  manualControl();
  
  //Test limit switches to prevent driving stage past limits
  if(digitalRead(Z_ULIMIT_SWITCH) && (_z_stepper.distanceToGo() > 0)){
    _z_stepper.move(0);
  }
  if(digitalRead(Z_LLIMIT_SWITCH) && (_z_stepper.distanceToGo() < 0)){
    _z_stepper.move(0);
  }
  
  //Called on every loop to enable non-blocking control of steppers
  _z_stepper.run();
  
}

void Stage::zMove(long steps)
{
  //Move the z stepper motor 
  _z_stepper.move(steps);
}

void Stage::zMoveTo(long position)
{
  //Move the z stepper to a position
  if(calibrated){
    _z_stepper.moveTo(position);
  }
}

void Stage::calibrate()
{
  //Run the motor down until we hit the bottom limit switch
  _z_stepper.setSpeed(-300);
  while(!digitalRead(Z_LLIMIT_SWITCH))
  {
   _z_stepper.runSpeed();
  }
  _z_stepper.move(0);
  _z_stepper.run();
  _z_stepper.setCurrentPosition(0); //Set bottom as 0 position
  
  //Run the motor up until we hit the top limit switch
  _z_stepper.setSpeed(300);
  while(!digitalRead(Z_ULIMIT_SWITCH))
  {
    _z_stepper.runSpeed();
  }
  _z_stepper.move(0);
  _z_stepper.run();
  z_length = _z_stepper.currentPosition(); //Take this position as the length of the z-axis
  calibrated = true; //Set flag as calibrated.
}

void Stage::manualControl()
{
  //Allows manual control of the stage position using buttons
  boolean up_switch_state = digitalRead(Z_UP_SWITCH);
  boolean down_switch_state = digitalRead(Z_DOWN_SWITCH);
  
  //Do nothing if both switches pressed
  if(down_switch_state && up_switch_state) {
    manual_control = false;
    zMove(0);
  }
  else if(up_switch_state) {
    manual_control = true;
    zMove(1000);    
  }
  else if(down_switch_state) {
    manual_control = true;
    zMove(-1000);
  }
  else if(manual_control) {
    //Prevents overriding of serial commands
    manual_control = false;
    zMove(0);
  }
}

long Stage::getZPosition()
{
  if(calibrated)
  {
    return _z_stepper.currentPosition();
  }
}

long Stage::getZDistanceToGo()
{
  return _z_stepper.distanceToGo();
}


