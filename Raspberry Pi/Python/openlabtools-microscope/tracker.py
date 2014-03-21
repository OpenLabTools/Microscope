from datetime import datetime
import numpy as np
import cv2
from interface import Microscope


class WormTracker():
    """Class for worm tracking algorithm"""

    def nothing(x, y):
        #Placeholder as trackbar requires callback
        pass

    def __init__(self, serial_port, camera):
        """Initializes the class with serial interface/camera names,
        algorithm control parameters.

        Arguments:
        serial_port -- String of name for the serial interface for the
            microscope Arduino - e.g. 'dev/tty.usb'
        camera -- Integer for the V4L2 video device name e.g. 0 from
            '/dev/video0'

        """
        self.create_gui()

        #Setup camera and video recording
        self.camera = cv2.VideoCapture(camera)
        self.width = int(self.camera.get(cv2.cv.CV_CAP_PROP_FRAME_WIDTH))
        self.height = int(self.camera.get(cv2.cv.CV_CAP_PROP_FRAME_HEIGHT))

        #Kernel for morphological opening/closing operation
        self.kernel = np.ones((3, 3), np.uint8)

        self.microscope = Microscope(serial_port)
        self.microscope.set_ring_colour('FF0000')
        self.last_step_time = datetime.now()

    def create_gui(self):
        """"Defines the HighGUI elements """
        cv2.namedWindow('Preview')
        cv2.createTrackbar('Tracking Off/On', 'Preview', 0, 1, self.nothing)
        cv2.createTrackbar('Threshold', 'Preview', 0, 1, self.nothing)
        cv2.createTrackbar('Threshold Value', 'Preview', 0, 255, self.nothing)
        cv2.createTrackbar('Step Interval', 'Preview', 0, 1000, self.nothing)
        cv2.createTrackbar('Opening Rounds', 'Preview', 0, 10, self.nothing)
        cv2.createTrackbar('Closing Rounds', 'Preview', 0, 10, self.nothing)
        cv2.createTrackbar('Margin', 'Preview', 0, 200, self.nothing)
        cv2.createTrackbar('Step Size', 'Preview', 0, 20, self.nothing)
        cv2.createTrackbar('Contour', 'Preview', 0, 1, self.nothing)
        cv2.createTrackbar('Canny', 'Preview', 0, 1, self.nothing)
        cv2.createTrackbar('Upper', 'Preview', 0, 255, self.nothing)
        cv2.createTrackbar('Lower', 'Preview', 0, 255, self.nothing)
        cv2.createTrackbar('PolyDP', 'Preview', 0, 1, self.nothing)
        cv2.createTrackbar('Precision', 'Preview', 0, 100, self.nothing)

        #Set default values
        cv2.setTrackbarPos('Threshold Value', 'Preview', 100)
        cv2.setTrackbarPos('Step Interval', 'Preview', 500)
        cv2.setTrackbarPos('Margin', 'Preview', 100)
        cv2.setTrackbarPos('Step Size', 'Preview', 2)
        cv2.setTrackbarPos('Upper', 'Preview', 200)
        cv2.setTrackbarPos('Lower', 'Preview', 100)

    def read_trackbars(self):
        """Read trackbar values"""
        self.tracking = bool(cv2.getTrackbarPos('Tracking Off/On', 'Preview'))
        self.show_threshold = bool(cv2.getTrackbarPos('Threshold', 'Preview'))
        self.threshold = cv2.getTrackbarPos('Threshold Value', 'Preview')
        self.step_interval = cv2.getTrackbarPos('Step Interval', 'Preview')
        self.opening_rounds = cv2.getTrackbarPos('Opening Rounds', 'Preview')
        self.closing_rounds = cv2.getTrackbarPos('Closing Rounds', 'Preview')
        self.margin = cv2.getTrackbarPos('Margin', 'Preview')
        self.step_size = cv2.getTrackbarPos('Step Size', 'Preview')
        self.draw_contour = bool(cv2.getTrackbarPos('Contour', 'Preview'))
        self.canny = bool(cv2.getTrackbarPos('Canny', 'Preview'))
        self.upper_canny = cv2.getTrackbarPos('Upper', 'Preview')
        self.lower_canny = cv2.getTrackbarPos('Lower', 'Preview')
        self.poly_dp = bool(cv2.getTrackbarPos('PolyDP', 'Preview'))
        self.precision = cv2.getTrackbarPos('Precision', 'Preview')

    def find_worm(self):
        """Threshold and contouring algorithm to find centroid of worm"""
        self.img_gray = cv2.cvtColor(self.img, cv2.COLOR_BGR2GRAY)

        if self.canny:
            self.img_thresh = cv2.Canny(self.img, self.lower_canny,
                                        self.upper_canny)
        else:
            #Threshold
            ret, self.img_thresh = cv2.threshold(self.img_gray, self.threshold,
                                                 255, cv2.THRESH_BINARY_INV)

            for i in range(self.opening_rounds):
                self.img_thresh = cv2.erode(self.img_thresh, self.kernel, 1)
                self.img_thresh = cv2.dilate(self.img_thresh, self.kernel, 1)

            for i in range(self.closing_rounds):
                self.img_thresh = cv2.dilate(self.img_thresh, self.kernel, 1)
                self.img_thresh = cv2.erode(self.img_thresh, self.kernel, 1)

        #Copy image to allow displaying later
        img_contour = self.img_thresh.copy()
        contours, hierarchy = cv2.findContours(img_contour, cv2.RETR_TREE,
                                               cv2.CHAIN_APPROX_NONE)

        #Find the biggest contour
        worm_area = 0
        for contour in contours:
            area = cv2.contourArea(contour)
            if area > worm_area:
                self.worm = contour
                worm_area = area

        if self.poly_dp:
            worm = cv2.approxPolyDP(self.worm, self.precision, True)
            self.worm = worm

        #Compute the centroid of the worm contour
        moments = cv2.moments(self.worm)
        self.x = int(moments['m10']/moments['m00'])
        self.y = int(moments['m01']/moments['m00'])

    def move_stage(self):
        now = datetime.now()
        elapsed_time = ((now - self.last_step_time).microseconds)/1000

        if self.tracking and elapsed_time > self.step_interval:
            self.last_step_time = now
            if self.x < self.margin:
                self.microscope.move('x', -1*self.step_size)
                print 'Moving Left'

            if self.x > (self.width - self.margin):
                self.microscope.move('x', self.step_size)
                print 'Moving Right'

            if self.y < self.margin:
                self.microscope.move('y', -1*self.step_size)
                print 'Moving Up'

            if self.y > (self.height - self.margin):
                self.microscope.move('y', self.step_size)
                print 'Moving Down'

    def update_gui(self):
        if self.show_threshold:
            self.img = cv2.cvtColor(self.img_thresh, cv2.COLOR_GRAY2BGR)

        if self.draw_contour:
            cv2.drawContours(self.img, [self.worm], -1, (255, 0, 0), 2)

        #Draw markers for centroid and boundry
        cv2.circle(self.img, (self.x, self.y), 5, (0, 0, 255), -1)
        cv2.rectangle(self.img, (self.margin, self.margin),
                      (self.width-self.margin, self.height-self.margin),
                      (0, 255, 0),
                      2)

        #Show image
        cv2.imshow('Preview', self.img)
        cv2.waitKey(1)

    def run(self):
        """Runs the worm tracking algorithm indefinitely"""

        for i in range(100):
            #Spool off 100 images to allow camera to auto-adjust
            self.img = self.camera.read()

        while True:

            self.read_trackbars()
            ret, self.img = self.camera.read()
            self.find_worm()
            self.update_gui()
            self.move_stage()
