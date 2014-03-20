from datetime import datetime
import numpy as np
import cv2
from interface import Microscope


class WormTracker():
    """Class for worm tracking algorithm

    Instance variables
    microscope - OpenLabTools Microscope  interface object
    camera - OpenCV2 camera object

    """

    def nothing(x, y):
        pass

    def __init__(self, serial_port, camera, margin=100, threshold=90, step_size=2):
        """Initializes the class with serial interface/camera names,
        algorithm control parameters.

        Arguments:
        serial_port -- String of name for the serial interface for the microscope Arduino - e.g. 'dev/tty.usb'
        camera -- String of name for the V4L2 video device name e.g. '/dev/video0'
        margin -- Integer of pixel width of margin to keep worm centroid in - default 10
        threshold -- Integer of threcv2.cv.CV_CAP_shold value for thresholding operation between 0 and 255 - default 127.
        step_size -- Integer of size of step for XY stage to take when re-centering worm


        """
        cv2.namedWindow('Preview')
        cv2.createTrackbar('Tracking Off/On', 'Preview', 0, 1, self.nothing)
        cv2.createTrackbar('Show Threshold Image', 'Preview', 0, 1, self.nothing)
        cv2.createTrackbar('Threshold Value', 'Preview', 0, 255, self.nothing)
        cv2.createTrackbar('Step Interval', 'Preview', 0, 1000, self.nothing)
        cv2.createTrackbar('Opening Rounds', 'Preview', 0, 10, self.nothing)
        cv2.createTrackbar('Closing Rounds', 'Preview', 0, 10, self.nothing)
        cv2.createTrackbar('Record', 'Preview', 0, 1, self.nothing)

        cv2.setTrackbarPos('Threshold Value', 'Preview', 100)
        cv2.setTrackbarPos('Step Interval', 'Preview', 100)

        self.microscope = Microscope(serial_port)
        self.camera = cv2.VideoCapture(camera)
        self.margin = margin
        self.threshold = threshold
        self.step_size = step_size

        self.kernel = np.ones((3, 3), np.uint8)

        self.last_step_time = datetime.now()

        self.width = int(self.camera.get(cv2.cv.CV_CAP_PROP_FRAME_WIDTH))
        self.height = int(self.camera.get(cv2.cv.CV_CAP_PROP_FRAME_HEIGHT))

        self.microscope.set_ring_colour('FF0000')

        self.video = cv2.VideoWriter('output.avi', -1, 1, (self.width, self.height))

    def run(self):
        """Runs the worm tracking algorithm indefinitely"""

        for i in range(100):
            ret, img = self.camera.read()

        while True:
            #Grab image and display
            ret, img = self.camera.read()

            img_gray = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)

            threshold = cv2.getTrackbarPos('Threshold Value', 'Preview')
            #Threshold then compute contours
            ret, img_thresh = cv2.threshold(img_gray, threshold, 255, cv2.THRESH_BINARY_INV)

            opening_rounds = cv2.getTrackbarPos('Opening Rounds', 'Preview')

            for i in range(opening_rounds):
                img_thresh = cv2.erode(img_thresh, self.kernel, 1)
                img_thresh = cv2.dilate(img_thresh, self.kernel, 1)

            closing_rounds = cv2.getTrackbarPos('Closing Rounds', 'Preview')

            for i in range(closing_rounds):
                img_thresh = cv2.dilate(img_thresh, self.kernel, 1)
                img_thresh = cv2.erode(img_thresh, self.kernel, 1)

            img_contour = img_thresh.copy()
            contours, hierarchy = cv2.findContours(img_contour, cv2.RETR_TREE, cv2.CHAIN_APPROX_NONE)

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

            show_threshold = bool(cv2.getTrackbarPos('Show Threshold Image','Preview'))
            if show_threshold:
                img = cv2.cvtColor(img_thresh, cv2.COLOR_GRAY2BGR)

            cv2.circle(img, (cx, cy), 5, (0, 0, 255), -1)
            cv2.rectangle(img, (self.margin, self.margin), (self.width-self.margin, self.height-self.margin), (0,255,0),2)

            cv2.imshow('Preview', img)
            cv2.waitKey(1)
            #If centroid within margin of image edge, move stage

            v_record = bool(cv2.getTrackbarPos('Record', 'Preview'))

            if v_record:
                self.video.write(img)

            tracking = bool(cv2.getTrackbarPos('Tracking Off/On', 'Preview'))
            step_interval = cv2.getTrackbarPos('Step Interval', 'Preview')

            elapsed_time = ((datetime.now() - self.last_step_time).microseconds)/1000

            if tracking and elapsed_time > step_interval:
                self.last_step_time = datetime.now()
                if cx < self.margin:
                    self.microscope.move('x', -1*self.step_size)
                    print 'Moving Left'

                if cx > (self.width - self.margin):
                    self.microscope.move('x', self.step_size)
                    print 'Moving Right'

                if cy < self.margin:
                    self.microscope.move('y', -1*self.step_size)
                    print 'Moving Up'
                if cy > (self.height - self.margin):
                    self.microscope.move('y', self.step_size)
                    print 'Moving Down'
