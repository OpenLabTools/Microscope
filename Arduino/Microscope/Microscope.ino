#include <Wire.h>
#include <Adafruit_MotorShield.h>
#include <AccelStepper.h>

//Variables and constants for serial command handling
const int max_length = 20;
char *inputString = (char *)malloc(max_length);
int str_pos = 0;
boolean stringComplete = false;

// Create the motor shield object with the default I2C address
Adafruit_MotorShield AFMS = Adafruit_MotorShield();

// Create stepper motors for each axis (200 steps per rev)
Adafruit_StepperMotor *zMotor = AFMS.getStepper(200, 2); //Z-axis Port 2

//Wrapper functions around Adafruit stepper objects for use with
//AccelStepper library
void z_forward(){
  zMotor->onestep(FORWARD, DOUBLE);
}
void z_backward(){
  zMotor->onestep(BACKWARD, DOUBLE);
}

//Stepper objects from AccelStepper Library
AccelStepper zStepper(z_forward, z_backward);

//Calls associated functions and passes arguments for each command
void handle_command(char* cmd, char* arg)
{
  //Print out the received command and argument
  Serial.print("Command: ");
  Serial.println(cmd);
  Serial.print("Argument: ");
  Serial.println(arg);
  if (strcmp("z_move", cmd)==0)
  {
    int steps = atoi(arg);
    z_move(steps);
  }
  else
  {
    //Print error message if command unknown.
    Serial.println("ERR: UNKNOWN COMMAND");
  }
}

void setup() {
  // initialize serial
  Serial.begin(9600);
  
  // initialize motor shiled
  AFMS.begin();
  
  //Set stepper motor speed and acceleration
  zStepper.setMaxSpeed(500.0);
  zStepper.setAcceleration(100.0);
}

void z_move(int steps) {
  //Move the z stepper motor and print message
  zStepper.move(steps);
  Serial.print("Moving z axis ");
  Serial.print(steps);
  Serial.println(" steps.");
}

void loop() {
  // print the string when a newline arrives:
  if (stringComplete) {
    char *command = inputString;
    char *arg = inputString;
    for (int i=0; i<max_length; i++)
    {
      if (inputString[i]=='\0')
        break;
      if (inputString[i]==' ')
      {
        inputString[i] = '\0';
        arg++;
        break;
      }
      arg++;
    }
    handle_command(command, arg);
    // Reset input str
    str_pos = 0;
    stringComplete = false;
  }
  
  zStepper.run();
}

void serialEvent() {
  while (Serial.available()) {
    // get the new byte:
    char inChar = (char)Serial.read(); 
    // add it to the inputString:
    if ((str_pos<max_length) && (inChar!='\n'))
    {
      inputString[str_pos] = inChar;
      str_pos++;
    }
    // if the incoming character is a newline, set a flag
    // so the main loop can do something about it:
    if (inChar == '\n') {
      stringComplete = true;
      // End a string with a null
      if (str_pos<max_length)
        inputString[str_pos]='\0';
      else
        inputString[max_length-1]='\0';
      str_pos = 0;
    }
  }
}
