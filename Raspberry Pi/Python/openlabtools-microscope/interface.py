__author__ = 'james'

import serial
from time import sleep


class Microscope:
    """Abstracts the serial command interface to the OpenLabTools microscope"""

    def __init__(self, port):
        """Opens the serial connection given a port name"""

        #Open connection
        self.ser = serial.Serial(port, 9600)

        #Flash the DTR pin to reset the Arduino (Needed so the Arduino is in a known state)
        self.ser.setDTR(False)
        sleep(0.22)
        self.ser.setDTR(True)

        #Read the first four lines from the Stepper shield initialization
        for x in range(4):
            self.ser.readline()

    def __del__(self):
        """Closes the serial connection on object deletion"""
        self.ser.close()

    def run_command(self, command):
        """Writes command to the interface and returns any response.

        Raises an exception if any error returned from Arduino

        """

        self.ser.write(command)

        line = ''
        ret = ''

        #Keep reading lines until the Arduino indicates command finished
        while line != 'OK\r\n':
            line = self.ser.readline()
            if line.startswith('ERR:'):
                raise Exception(line)
            if line.startswith('RETURN:'):
                ret = line[8:]

        return ret

    @staticmethod
    def check_axis(axis):
        """Checks that the axis string corresponds to a valid axis"""
        if axis not in ['x', 'y', 'z']:
            raise Exception('Not a valid axis!')

    def calibrate(self):
        """Runs the calibration procedure for the Microscope"""

        #The only blocking command(and takes for forever), so longer timeout
        self.ser.timeout = 30
        command = 'calibrate\n'
        self.run_command(command)
        self.ser.timeout = 1

    def is_calibrated(self):
        """Check if the microscope is calibrated"""
        command = 'is_calibrated\n'
        line = self.run_command(command)
        if line == '1':
            return True
        else:
            return False

    def get_length(self, axis):
        """Returns the length of the specified axis"""
        command = axis + '_get_length\n'
        length = self.run_command(command)
        return int(length)

    def get_position(self, axis):
        """Get the current position of the specified axis"""
        self.check_axis(axis)
        command = axis + '_get_position\n'
        position = self.run_command(command)
        return int(position)

    def get_distance_to_go(self, axis):
        """Get the distance between current and target position of the specified axis"""
        self.check_axis(axis)
        command = axis + '_get_distance_to_go\n'
        distance = self.run_command(command)
        return int(distance)

    def move(self, axis, steps):
        """Move the specified axis relative to current position"""
        self.check_axis(axis)
        command = axis + '_move ' + str(steps) + '\n'
        self.run_command(command)

    def move_to(self, axis, position):
        """Move the specified axis to an absolute position"""
        self.check_axis(axis)
        command = axis + '_move_to' + str(position) + '\n'
        self.run_command(command)

    def set_ring_colour(self, colour):
        """Set the colour of the ring LED"""
        command = 'set_ring_colour ' + str(colour) + '\n'
        self.run_command(command)

    def set_ring_brightness(self, brightness):
        """Set the brightness of the ring LED"""
        command = 'set_ring_brightness ' + str(brightness) + '\n'
        self.run_command(command)

    def set_stage_led_brightness(self, brightness):
        """Set the brightness of the stage LED"""
        command = 'set_stage_led_brightness ' + str(brightness) + '\n'
        self.run_command(command)