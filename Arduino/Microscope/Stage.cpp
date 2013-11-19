/*
  Stage.cpp - Library for controlling the stage on the OpenLabTools microscope
  Written by James Ritchie for OpenLabTools
  github.com/OpenLabTools/Microscope
*/
#include "Arduino.h"
#include <Wire.h>
#include <Adafruit_MotorShield.h>
#include "Stage.h"

// Create the motor shield objects
Adafruit_MotorShield bottom_afms = Adafruit_MotorShield(0x60);
Adafruit_MotorShield top_afms = Adafruit_MotorShield(0x61);

// Create stepper motors for each axis (200 steps per rev)
Adafruit_StepperMotor *xy_a_motor = top_afms.getStepper(200,1);
Adafruit_StepperMotor *xy_b_motor = top_afms.getStepper(200,2);
Adafruit_StepperMotor *z_motor = bottom_afms.getStepper(200, 2); 

Stage::Stage() {
  //Initialize AccelStepper object with wrapper functions and parameters.
  calibrated = false;
  
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
  
  _x_pos = 0;
  _y_pos = 0;
  _z_pos = 0;
  
  _xy_interval = 30;
  _z_interval = 1;

}

void Stage::loop()
{
  
  //Check for manual commands
  manualControl();
  
  //Test limit switches to prevent driving stage past limits
  if(!digitalRead(Z_ULIMIT_SWITCH) && (getDistanceToGo(Z_STEPPER) > 0)){
    Move(Z_STEPPER,0);
  }
  if(!digitalRead(Z_LLIMIT_SWITCH) && (getDistanceToGo(Z_STEPPER) < 0)){
     Move(Z_STEPPER,0);;
  }
  
  //Called on every loop to enable non-blocking control of steppers
  if((millis()-_z_last_step)>_z_interval){
    if((_z_target-_z_pos)>0){
      z_motor->onestep(FORWARD, DOUBLE);
      _z_last_step = millis();
      _z_pos++;
    }
    else if((_z_target-_z_pos)<0){
       z_motor->onestep(BACKWARD, DOUBLE);
       _z_last_step = millis();
       _z_pos--;
    }
  }
  
  if((millis()-_xy_last_step)>_xy_interval){
    
    if((_x_target-_x_pos)>0 && (_y_target-_y_pos)>0){
      //Move up and right
      xy_a_motor->onestep(FORWARD, DOUBLE);
      _xy_last_step = millis();
      _x_pos++;
      _y_pos++;  
    }
    else if((_x_target-_x_pos)>0 && (_y_target-_y_pos)<0){
      //Move down and right
      xy_b_motor->onestep(FORWARD, DOUBLE);
      _xy_last_step = millis();
      _x_pos++;
      _y_pos--;  
    }
     else if((_x_target-_x_pos)<0 && (_y_target-_y_pos)>0){
      //Move up and left
      xy_b_motor->onestep(BACKWARD, DOUBLE);
      _xy_last_step = millis();
      _x_pos--;
      _y_pos++;
    }
     else if((_x_target-_x_pos)<0 && (_y_target-_y_pos)<0){
      //Move down and left
      xy_a_motor->onestep(BACKWARD, DOUBLE);
      _xy_last_step = millis();
      _x_pos--;
      _y_pos--;  
    }
     else if((_x_target-_x_pos)==0 && (_y_target-_y_pos)>0){
      //Move up
      xy_a_motor->onestep(FORWARD, DOUBLE);
      xy_b_motor->onestep(BACKWARD, DOUBLE);
      _xy_last_step = millis(); 
      _y_pos++;     
    }
    else if((_x_target-_x_pos)==0 && (_y_target-_y_pos)<0){
      //Move down
      xy_a_motor->onestep(BACKWARD, DOUBLE);
      xy_b_motor->onestep(FORWARD, DOUBLE);
      _xy_last_step = millis();
      _y_pos--; 
    }
    else if((_x_target-_x_pos)>0 && (_y_target-_y_pos)==0){
      //Move right
      xy_a_motor->onestep(FORWARD, DOUBLE);
      xy_b_motor->onestep(FORWARD, DOUBLE);
      _xy_last_step = millis();
      _x_pos++;      
    }
    else if((_x_target-_x_pos)<0 && (_y_target-_y_pos)==0){
      //Move left
      xy_a_motor->onestep(BACKWARD, DOUBLE);
      xy_b_motor->onestep(BACKWARD, DOUBLE);
      _xy_last_step = millis();
      _x_pos--;   
    }
    
  }
 
  
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
  
}

void Stage::calibrate()
{
 
}

long Stage::getPosition(int stepper)
{
  if(calibrated)
  {
    switch(stepper) {
      case X_STEPPER:
        return _x_pos;
        break;
      case Y_STEPPER:
        return _y_pos;
        break;
      case Z_STEPPER:
        return _z_pos;
        break;      
    }
  }
}

long Stage::getDistanceToGo(int stepper)
{
  switch(stepper) {
    case X_STEPPER:
      return _x_target - _x_pos;
      break;
    case Y_STEPPER:
      return _y_target - _y_pos;
      break;
    case Z_STEPPER:
      return _z_target - _z_pos;
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
      _x_target = _x_pos + steps;
      break;
    case Y_STEPPER:
     _y_target = _y_pos + steps;
      break;
    case Z_STEPPER:
      _z_target = _z_pos + steps;
      break;      
  }
}

void Stage::MoveTo(int stepper, long position)
{
  //Move the z stepper to a position
  if(calibrated){
    switch(stepper) {
      case X_STEPPER:
        _x_target = position;
        break;
      case Y_STEPPER:
        _y_target = position;
        break;
      case Z_STEPPER:
        _z_target = position;
        break;      
    }
  }
}
