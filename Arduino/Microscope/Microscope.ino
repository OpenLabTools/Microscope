/*
  Microscope.ino - Firmware for the Arduino on the OpenLabTools microscope
  Written by James Ritchie for OpenLabTools
  github.com/OpenLabTools/Microscope
 
*/
#include <Wire.h>
#include <Adafruit_MotorShield.h>
#include <AccelStepper.h>
#include <Adafruit_NeoPixel.h>
#include "SerialControl.h"
#include "Stage.h"
#include "Lighting.h"

//Create objects for Microscope functions
SerialControl scontrol = SerialControl();
Stage stage = Stage();
Lighting lights = Lighting();

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
  else if(strcmp("get_z_position", cmd)==0)
  {
    if(stage.calibrated)
    {
      Serial.println(stage.getZPosition());
      Serial.println("OK");
    }
    else
    {
      Serial.println("ERR: NOT CALIBRATED");
    }
  }
  else if(strcmp("get_z_distance_to_go",cmd)==0)
  {
    Serial.println(stage.getZDistanceToGo());    
  }
  else if(strcmp("is_calibrated", cmd)==0)
  {
    //Test if calibrated
    Serial.println(stage.calibrated);
    Serial.println("OK");
  }
  else if(strcmp("set_ring_colour", cmd)==0)
  {
    uint32_t colour = strtoul(arg, NULL, 16);
    lights.setRingColour(colour);
    Serial.println("OK");
  }
  else if(strcmp("set_ring_brightness", cmd)==0)
  {
    uint8_t brightness = atoi(arg);
    lights.setRingBrightness(brightness);
    Serial.println("OK");
  }
  else if(strcmp("set_stage_led_brightness", cmd)==0)
  {
    uint8_t brightness = atoi(arg);
    lights.setStageLEDBrightness(brightness);
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
  lights.begin();
}

void loop() {
  if (scontrol.string_complete) {
    handle_command(scontrol.command, scontrol.arg);
    // Reset input str
    scontrol.string_complete = false;
  }
  stage.loop();
  lights.loop();
}

void serialEvent() {
  //Attach SerialControl to the Serial event
  scontrol.serialEvent();
}

void serialEventRun(void){
  //Hack to fix broken IDE functionality for Due
  if(Serial.available()) serialEvent();
}


