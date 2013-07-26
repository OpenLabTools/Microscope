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


const int Stage::up_switch_pin;
const int Stage::down_switch_pin;

Stage::Stage() {
  //Initialize AccelStepper object with wrapper functions and parameters.
  _z_stepper = AccelStepper(zForward, zBackward);
  _z_stepper.setAcceleration(200.0);
  _z_stepper.setMaxSpeed(1000.0);
  
}

void Stage::begin()
{
  //Initiliaze the motor shield
  afms.begin();
  
  //Setup pins for manual control
  pinMode(up_switch_pin, INPUT);
  pinMode(down_switch_pin, OUTPUT);
}

void Stage::loop()
{
  
  //Called on every loop to enable non-blocking control of steppers
  manualControl();
  _z_stepper.run();
  
}

void Stage::zMove(int steps)
{
  //Move the z stepper motor 
  _z_stepper.move(steps);
}

void Stage::manualControl()
{
  boolean up_switch_state = digitalRead(up_switch_pin);
  boolean down_switch_state = digitalRead(down_switch_pin);
  
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


