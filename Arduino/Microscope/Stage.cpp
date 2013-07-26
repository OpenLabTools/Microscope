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


Stage::Stage() {
  //Initialize AccelStepper object with wrapper functions and parameters.
  _z_stepper = AccelStepper(zForward, zBackward);
  _z_stepper.setMaxSpeed(500.0);
  _z_stepper.setAcceleration(100.0);
}

void Stage::begin()
{
  //Initiliaze the motor shield
  afms.begin();
}

void Stage::loop()
{
  //Called on every loop to enable non-blocking control of steppers
  _z_stepper.run();
}

void Stage::zMove(int steps)
{
  //Move the z stepper motor 
  _z_stepper.move(steps);
}


