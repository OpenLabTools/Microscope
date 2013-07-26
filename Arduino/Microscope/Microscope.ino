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
    int steps = atoi(arg);
    stage.zMove(steps);
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


