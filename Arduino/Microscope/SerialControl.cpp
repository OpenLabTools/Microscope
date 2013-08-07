/*
  SerialControl.cpp - Library for implementing serial control on the OpenLabTools microscope
  Written by James Ritchie for OpenLabTools
  github.com/OpenLabTools/Microscope
*/
#include "Arduino.h"
#include "SerialControl.h"

SerialControl::SerialControl()
{
  
  //Initialize variables
  string_complete = false;
  input_string = (char *)malloc(max_length);
  str_pos = 0;
}

void SerialControl::begin()
{
  //Open a serial connection
  Serial.begin(9600);
}

void SerialControl::serialEvent()
{
  while (Serial.available()) {
    // get the new byte:
    char in_char = (char)Serial.read(); 
    // add it to the inputString:
    if ((str_pos<max_length) && (in_char!='\n'))
    {
      input_string[str_pos] = in_char;
      str_pos++;
    }
    // if the incoming character is a newline, set a flag
    // so the main loop can do something about it:
    if (in_char == '\n') {
      string_complete = true;
      // End a string with a null
      if (str_pos<max_length)
        input_string[str_pos]='\0';
      else
        input_string[max_length-1]='\0';
      str_pos = 0;
      _processString();
    }
  }
}

void SerialControl::_processString(){
  
  //Split received string into command and argument
  command = input_string;
  arg = input_string;

  for (int i=0; i<max_length; i++)
  {
    if (input_string[i]=='\0')
      break;
    if (input_string[i]==' ')
    {
      input_string[i] = '\0';
      arg++;
      break;
    }
    arg++;
  }

  //Print out the received command and argument
  Serial.print("Command: ");
  Serial.println(command);
  Serial.print("Argument: ");
  Serial.println(arg);

}

const int SerialControl::max_length;

