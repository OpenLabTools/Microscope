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

// Create the motor shield objects
Adafruit_MotorShield bottom_afms = Adafruit_MotorShield(0x60);
Adafruit_MotorShield top_afms = Adafruit_MotorShield(0x61);

// Create stepper motors for each axis (200 steps per rev)
Adafruit_StepperMotor *x_motor = bottom_afms.getStepper(200,1);
Adafruit_StepperMotor *y_motor = top_afms.getStepper(200,2);
Adafruit_StepperMotor *z_motor = bottom_afms.getStepper(200, 2); 

//Wrapper functions around Adafruit stepper objects for use with
//AccelStepper library
void xForward() {
  x_motor->onestep(FORWARD, DOUBLE);
}
void xBackward() {
  x_motor->onestep(BACKWARD, DOUBLE);
}
void yForward() {
  y_motor->onestep(FORWARD, DOUBLE);
}
void yBackward() {
  y_motor->onestep(BACKWARD, DOUBLE);
}
void zForward() {
  z_motor->onestep(FORWARD, DOUBLE);
}
void zBackward() {
  z_motor->onestep(BACKWARD, DOUBLE);
}

Stage::Stage() {
  //Initialize AccelStepper object with wrapper functions and parameters.
  
  _x_stepper = AccelStepper(xForward, xBackward);
  _x_stepper.setAcceleration(1000.0);
  _x_stepper.setMaxSpeed(1000.0);
  
  _y_stepper = AccelStepper(yForward, yBackward);
  _y_stepper.setAcceleration(1000.0);
  _y_stepper.setMaxSpeed(1000.0);
  
  _z_stepper = AccelStepper(zForward, zBackward);
  _z_stepper.setAcceleration(1000.0);
  _z_stepper.setMaxSpeed(1000.0);
  
  calibrated = false;
  re_selection_changed = 1;
  re_a_last = HIGH;
  
}

void Stage::begin()
{
  //Initiliaze the motor shield
  bottom_afms.begin();
  top_afms.begin();
  
  //Setup pins for input
  pinMode(Z_ULIMIT_SWITCH, INPUT_PULLUP);
  pinMode(Z_LLIMIT_SWITCH, INPUT_PULLUP);
  pinMode(Z_UP_SWITCH, INPUT_PULLUP);
  pinMode(Z_DOWN_SWITCH, INPUT_PULLUP);
  
  pinMode(ROTARY_ENCODER_SWITCH, INPUT_PULLUP);
  pinMode(ROTARY_ENCODER_A, INPUT_PULLUP);
  pinMode(ROTARY_ENCODER_B, INPUT_PULLUP);
}

void Stage::loop()
{
  
  //Check for manual commands
  manualControl();
  
  //Test limit switches to prevent driving stage past limits
  if(!digitalRead(Z_ULIMIT_SWITCH) && (_z_stepper.distanceToGo() > 0)){
    _z_stepper.move(0);
  }
  if(!digitalRead(Z_LLIMIT_SWITCH) && (_z_stepper.distanceToGo() < 0)){
    _z_stepper.move(0);
  }
  
  //Called on every loop to enable non-blocking control of steppers
  _x_stepper.run();
  _y_stepper.run();
  _z_stepper.run();
  
}

void Stage::manualControl()
{
  //Allows manual control of the stage position using buttons
  boolean up_switch_state = digitalRead(Z_UP_SWITCH);
  boolean down_switch_state = digitalRead(Z_DOWN_SWITCH);
  
  //Do nothing if both switches pressed
  if(!down_switch_state && !up_switch_state) {
    manual_control = false;
    Move(Z_STEPPER, 0);
  }
  else if(!up_switch_state) {
    manual_control = true;
    Move(Z_STEPPER, 1000);    
  }
  else if(!down_switch_state) {
    manual_control = true;
    Move(Z_STEPPER, -1000);
  }
  else if(manual_control) {
    //Prevents overriding of serial commands
    manual_control = false;
    Move(Z_STEPPER, 0);
  }
  
  //Rotary Encoder Axis Selection
  //Debounce input, check if going from high to low, then cycle selection.
  int re_reading = digitalRead(ROTARY_ENCODER_SWITCH);
 
  if(re_reading != re_last_reading){
    re_debounce_time = millis();
  }  
  if ((millis() - re_debounce_time) > 50) {
    re_state = re_reading;
    if((re_state == 0) && (re_last_state == 1)) {
      if(re_selection == Z_STEPPER) {
        re_selection = X_STEPPER;
      }
      else {
        re_selection++;
      }
      re_selection_changed = 1;      
    }
    re_last_state=re_state;
  }
  
  re_last_reading = re_reading;
  
  //Rotary Encoder rotation reading
  int n = digitalRead(ROTARY_ENCODER_A);
  if ((re_a_last == HIGH) && (n == LOW)) {
    if (digitalRead(ROTARY_ENCODER_B) == HIGH) {
      Move(re_selection, -1);
    }
    else {
      Move(re_selection, 1);
    }
  }
  re_a_last = n;
  
}

void Stage::calibrate()
{
  //Run the motor down until we hit the bottom limit switch
  _z_stepper.setSpeed(-300);
  while(digitalRead(Z_LLIMIT_SWITCH))
  {
   _z_stepper.runSpeed();
  }
  _z_stepper.move(0);
  _z_stepper.run();
  _z_stepper.setCurrentPosition(0); //Set bottom as 0 position
  
  //Run the motor up until we hit the top limit switch
  _z_stepper.setSpeed(300);
  while(digitalRead(Z_ULIMIT_SWITCH))
  {
    _z_stepper.runSpeed();
  }
  _z_stepper.move(0);
  _z_stepper.run();
  _z_length = _z_stepper.currentPosition(); //Take this position as the length of the z-axis
  calibrated = true; //Set flag as calibrated.
}

long Stage::getPosition(int stepper)
{
  if(calibrated)
  {
    switch(stepper) {
      case X_STEPPER:
        return _x_stepper.currentPosition();
        break;
      case Y_STEPPER:
        return _y_stepper.currentPosition();
        break;
      case Z_STEPPER:
        return _z_stepper.currentPosition();
        break;      
    }
  }
}

long Stage::getDistanceToGo(int stepper)
{
  switch(stepper) {
    case X_STEPPER:
      return _x_stepper.distanceToGo();
      break;
    case Y_STEPPER:
      return _y_stepper.distanceToGo();
      break;
    case Z_STEPPER:
      return _z_stepper.distanceToGo();
      break;      
  }
}

long Stage::getLength(int stepper)
{
  switch(stepper) {
    case X_STEPPER:
      return _x_length;
      break;
    case Y_STEPPER:
      return _y_length;
      break;
    case Z_STEPPER:
      return _z_length;
      break;      
  }
}

void Stage::Move(int stepper, long steps)
{
  switch(stepper) {
    case X_STEPPER:
      _x_stepper.move(steps);
      break;
    case Y_STEPPER:
      _y_stepper.move(steps);
      break;
    case Z_STEPPER:
      _z_stepper.move(steps);
      break;      
  }
}

void Stage::MoveTo(int stepper, long position)
{
  //Move the z stepper to a position
  if(calibrated){
    switch(stepper) {
      case X_STEPPER:
        _x_stepper.moveTo(position);
        break;
      case Y_STEPPER:
        _y_stepper.moveTo(position);
        break;
      case Z_STEPPER:
        _z_stepper.moveTo(position);
        break;      
    }
  }
}
