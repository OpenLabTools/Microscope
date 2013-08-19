/*
  SerialControl.cpp - Library for implementing serial control on the OpenLabTools microscope
  Written by James Ritchie for OpenLabTools
  github.com/OpenLabTools/Microscope
*/
#include "Arduino.h"
#include "SerialControl.h"

#define MAX_LENGTH 40

SerialControl::SerialControl()
{
  
  //Initialize variables
  string_complete = false;
  input_string = (char *)malloc(MAX_LENGTH);
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
    if ((str_pos<MAX_LENGTH) && (in_char!='\n'))
    {
      input_string[str_pos] = in_char;
      str_pos++;
    }
    // if the incoming character is a newline, set a flag
    // so the main loop can do something about it:
    if (in_char == '\n') {
      string_complete = true;
      // End a string with a null
      if (str_pos<MAX_LENGTH)
        input_string[str_pos]='\0';
      else
        input_string[MAX_LENGTH-1]='\0';
      str_pos = 0;
      _processString();
    }
  }
}

void SerialControl::_processString(){
  
  //Split received string into command and argument
  command = input_string;
  arg = input_string;

  for (int i=0; i<MAX_LENGTH; i++)
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
