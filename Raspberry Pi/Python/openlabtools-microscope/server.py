__author__ = 'james'

import os
from datetime import datetime

from twisted.web import xmlrpc, server
from interface import Microscope


class MicroscopeServer(xmlrpc.XMLRPC):
    """ Implements an XML-RPC server allowing remote control of the OpenLabTools microscope over a network

    Exposes all Arduino commands, plus photo and video recording commands

    """

    def __init__(self, serial_port, allowNone=False, useDateTime=False):
        """Initializes the class with serial port name of Arduino"""
        xmlrpc.XMLRPC.__init__(self, allowNone, useDateTime)
        self.microscope = Microscope(serial_port)

    def xmlrpc_calibrate(self):
        self.microscope.calibrate()

    def xmlrpc_is_calibrated(self):
        calibrated = self.microscope.is_calibrated()
        return calibrated

    def xmlrpc_get_length(self, axis):
        axis = self.microscope.get_length(axis)
        return axis

    def xmlrpc_get_position(self, axis):
        position = self.microscope.get_position(axis)
        return position

    def xmlrpc_get_distance_to_go(self, axis):
        axis = self.microscope.get_distance_to_go(axis)
        return axis

    def xmlrpc_move(self, axis, position):
        self.microscope.move(axis, position)

    def xmlrpc_move_to(self, axis, position):
        self.microscope.move_to(self, axis, position)

    def xmlrpc_set_ring_colour(self, colour):
        self.microscope.set_ring_colour(colour)

    def xmlrpc_set_ring_brightness(self, brightness):
        self.microscope.set_ring_brightness(brightness)

    def xmlrpc_set_stage_led_brightness(self, brightness):
        self.microscope.set_stage_led_brightness(brightness)

    def xmlrpc_take_picture(self):
        """Takes a photograph with datetime string for filename"""

        #Not hugely portable, definitely unstable, need better method
        now = datetime.now()
        filename = ("%s_%s_%s__%s_%s_%s.jpg" % (now.year, now.month, now.day, now.hour, now.minute, now.second))
        os.system(("raspistill -n -t 0 -o %s &" % filename))

    def xmlrpc_take_video(self, duration):
        """Records a video of specified duration with datetime string for filename"""
        now = datetime.now()
        filename = ("%s_%s_%s__%s_%s_%s.h264" % (now.year, now.month, now.day, now.hour, now.minute, now.second))
        os.system(("raspivid  -n -t %s -o %s" %(duration, filename)))



#Start running the server
if __name__ == '__main__':
    from twisted.internet import reactor
    r = MicroscopeServer('/dev/tty.usbmodemfd121', True)
    reactor.listenTCP(7080, server.Site(r))
    reactor.run()


