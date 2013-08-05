/*
  Microscope.ino - Firmware for the Arduino on the OpenLabTools microscope
  Written by James Ritchie for OpenLabTools
  github.com/OpenLabTools/Microscope
 
*/
#include <Wire.h>
#include <Adafruit_MotorShield.h>
#include <AccelStepper.h>
#include "SerialControl.h"
#include "Stage.h"

//Create objects for Microscope functions
SerialControl scontrol = SerialControl(Serial);
Stage stage = Stage();

//Calls associated functions and passes arguments for each command
void handle_command(char* cmd, char* arg)
{
  if (strcmp("z_move", cmd)==0)
  {
    //Relative z move
    long steps = atol(arg);
    stage.zMove(steps);
    Serial.println("OK");
  }
  else if(strcmp("z_move_to", cmd)==0)
  {
    //Absolute z move
    if(stage.calibrated)
    {
 
      long position = atol(arg);
      //Check position is in range
      if(position>=0 && position<=stage.z_length)
      {
        stage.zMoveTo(position);
        Serial.println("OK");
      }
      else
      {
        Serial.println("ERR: POSITION OUT OF RANGE");
      }
    }
    else
    {
      Serial.println("ERR: NOT CALIBRATED");
    }
  }
  else if(strcmp("calibrate", cmd)==0)
  {
    //Calibrate the stage
    stage.calibrate();
    Serial.println("OK");
  }
  else if(strcmp("get_z_length", cmd)==0)
  {
    //Return z length if calibrated
    if(stage.calibrated)
    {
      Serial.println(stage.z_length);
      Serial.println("OK");
    }
    else
    {
      Serial.println("ERR: NOT CALIBRATED");
    }
  }
  else if(strcmp("is_calibrated", cmd)==0)
  {
    //Test if calibrated
    Serial.println(stage.calibrated);
    Serial.println("OK");
  }
  else
  {
    //Print error message if command unknown.
    Serial.println("ERR: UNKNOWN COMMAND");
  }
}

void setup() {
  //Initialize
  scontrol.begin();
  stage.begin();
}

void loop() {
  if (scontrol.string_complete) {
    handle_command(scontrol.command, scontrol.arg);
    // Reset input str
    scontrol.string_complete = false;
  }
  stage.loop();
}

void serialEvent() {
  //Attach SerialControl to the Serial event
  scontrol.serialEvent();
}


