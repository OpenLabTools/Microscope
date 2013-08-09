# Arduino

## Commands

The serial connection takes commands as an ASCII string of the form 

```
command argument
```

terminated with a newline(\n). Argument is optional for some commands. The
Arduino will return several lines:

```
Command: command
Argument: argument
Return: value
OK
```

The `Command:` and `Argument:` lines are always returned to confirm receipt of the command. `Return: value` is printed if the command returns a value.
`OK` is returned when the Arduino is ready to receive another command.
All commands except calibrate are asynchronous(i.e the Arduino will start
moving the steppers whilst continuing to receive commands) and so will
return OK almost immediately.

### calibrate

*Command*

```
calibrate
```

*Response*

```
Command: calibrate
Argument:
OK
```

`calibrate` runs the calibration routine for the stage, running the stepper
 motors on each axis to find where the limit switches are and establishing
an absolute positioning system. Other commands which rely on absolute 
positioning will return an error if this has not been run.

### is_calibrated

*Command*

```
is_calibrated
```

*Response*

```
Command: is_calibrated
Argument:
Return: 1
OK
```

`is_calibrated` returns 1 if `calibrated` has been run, and 0 if not. 
