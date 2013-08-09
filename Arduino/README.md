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

**Command**

```
calibrate
```

**Response**

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

**Command**

```
is_calibrated
```

**Response**

```
Command: is_calibrated
Argument:
Return: 1
OK
```

`is_calibrated` returns 1 if `calibrated` has been run, and 0 if not.

### get_z_length

**Command**

```
get_z_length
```

**Response**

```
Command: get__z_length
Argument: 
Return: 15381
OK
```

Returns the total length of the z axis in units of steps.

### get_z_position 

**Command**

```
get_z_position
```

**Response**

```
Command: get_z_position
Argument:
Return: 3651
OK
```

Returns the current z position of the stage, in units of steps, from the 
bottom of the axis.

### z_move

**Command**

```
z_move -500
```

**Response**

```
Command: z_move
Argument: -500
OK
```

Moves the stage from its current position the given number of steps along 
the z-axis. Positive for up, negative for down

### z__move_to

**Command**

```
z_move_to 1500
```

**Response**

```
Command: z_move_to
Argument: 1500
OK
```

Moves the stage to an absolute position along the z-axis, measured in
units of steps from the bottom of the axis. If given a position which is out
of the range of the axis (i.e. less than 0 or greater than the result of
'get_z_length'), will return an Out of Range error. 

### get_z_distance_to_go

**Command**

```
get_z_distance_to_go
```

**Response**

```
Command: get_z_distance_to_go
Argument:
Return: 130
OK
```

Gets the number of steps to go until the stage reaches its current target on
the z-axis(set by 'z_move' or 'z_move_to').
