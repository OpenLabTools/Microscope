__author__ = 'james'

import cv2
from interface import Microscope


class WormTracker():
    """Class for worm tracking algorithm

    Instance variables
    microscope - OpenLabTools Microscope  interface object
    camera - OpenCV2 camera object

    """

    def __init__(self, serial_port, camera, margin=10, threshold=127, step_size=10):
        """Initializes the class with serial interface/camera names, algorithm control parameters.

        Arguments:
        serial_port -- String of name for the serial interface for the microscope Arduino - e.g. 'dev/tty.usb'
        camera -- String of name for the V4L2 video device name e.g. '/dev/video0'
        margin -- Integer of pixel width of margin to keep worm centroid in - default 10
        threshold -- Integer of threshold value for thresholding operation between 0 and 255 - default 127.
        step_size -- Integer of size of step for XY stage to take when re-centering worm


        """
        cv2.namedWindow('Preview')
        self.microscope = Microscope(serial_port)
        self.camera = cv2.VideoCapture(camera)
        self.margin = margin
        self.threshold = threshold
        self.step_size = step_size

    def run(self):
        """Runs the worm tracking algorithm indefinitely"""

        while True:
            #Grab image and display
            ret, img = self.camera.read()
            cv2.imgshow('Preview', img)

            #Threshold then compute contours
            ret, img_thresh = cv2.threshold(img, self.threshold, 255, cv2.THRESH_BINARY_INV)
            contours, hierarchy = cv2.findContours(img_thresh, cv2.RETR_TREE, cv2.CHAIN_APPROX_NONE)

            #Find the biggest contour
            worm_area = 0
            for contour in contours:
                area = cv2.contourArea(contour)
                if area > worm_area:

                    worm = contour
                    worm_area = area

            #Compute the centroid of the worm contour
            moments = cv2.moments(worm)
            cx = int(moments['m10']/moments['m00'])
            cy = int(moments['m01']/moments['m00'])

            #If centroid within margin of image edge, move stage
            width = img.shape[0]
            height = img.shape[1]

            if cx < self.margin:
                self.microscope.move('x', -1*self.step_size)

            if cx > (width - self.margin):
                self.microscope.move('x', self.step_size)

            if cy < self.margin:
                self.microscope.move('y', -1*self.step_size)

            if cy > (height - self.margin):
                self.microscope.move('y', self.step_size)







