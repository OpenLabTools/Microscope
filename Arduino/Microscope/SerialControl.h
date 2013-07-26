/*
  SerialControl.h - Library for implementing serial control on the OpenLabTools microscope
  Written by James Ritchie for OpenLabTools
  github.com/OpenLabTools/Microscope
*/
#include "Arduino.h"

#ifndef SerialControl_h
#define SerialControl_h

class SerialControl
{
  public:
    static const int max_length = 20;    //Maximum command length
  
    boolean string_complete;             //Flag to indicate command received
  
    char *input_string; //Raw string from serial
    char *command;
    char *arg; //Argument given with command
  
    int str_pos; //String position counter
  
    SerialControl(HardwareSerial &serial);
    void begin();
    void serialEvent();
  private:
    HardwareSerial* _serial; //Pointer to Arduino Serial object
  
    void _processString();
};

#endif

