from datetime import datetime

import numpy as np
from scipy import interpolate

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
        self.camera.set(cv2.cv.CV_CAP_PROP_FRAME_WIDTH, self.width)
        self.camera.set(cv2.cv.CV_CAP_PROP_FRAME_HEIGHT, self.height)

        #Kernel for morphological opening/closing operation
        self.kernel = np.ones((3, 3), np.uint8)

        self.microscope = Microscope(serial_port)
        self.microscope.set_ring_colour('FF0000')
        self.last_step_time = datetime.now()

        self.last_frame = cv2.getTickCount()

        self.worm_spline = np.zeros((1001, 1, 2), dtype=np.int)
        self.tail = np.zeros((2,), dtype=np.int)
        self.head = np.zeros((2,), dtype=np.int)

    def change_kernel(self, size):
        size = size*2 + 1
        self.kernel = np.ones((size, size), np.uint8)

    def create_gui(self):
        """"Defines the HighGUI elements """
        cv2.namedWindow('Preview')
        cv2.createTrackbar('Tracking Off/On', 'Preview', 0, 1, self.nothing)
        cv2.createTrackbar('Threshold', 'Preview', 0, 1, self.nothing)
        cv2.createTrackbar('Threshold Value', 'Preview', 0, 255, self.nothing)
        cv2.createTrackbar('Step Interval', 'Preview', 0, 1000, self.nothing)
        cv2.createTrackbar('Margin', 'Preview', 0, 200, self.nothing)
        cv2.createTrackbar('Step Size', 'Preview', 0, 20, self.nothing)
        cv2.createTrackbar('Contour', 'Preview', 0, 1, self.nothing)
        cv2.createTrackbar('Adaptive', 'Preview', 0, 1, self.nothing)
        cv2.createTrackbar('Adaptive Value', 'Preview', 0, 40, self.nothing)
        cv2.createTrackbar('Adaptive Size', 'Preview', 1, 40, self.nothing)
        cv2.createTrackbar('Gaussian', 'Preview', 0, 1, self.nothing)
        cv2.createTrackbar('Kernel Size', 'Preview', 1, 20, self.change_kernel)
        cv2.createTrackbar('Opening', 'Preview', 0, 20, self.nothing)
        cv2.createTrackbar('Closing', 'Preview', 0, 20, self.nothing)
        cv2.createTrackbar('Skeleton', 'Preview', 0, 1, self.nothing)
        cv2.createTrackbar('Smoothing', 'Preview', 0, 1000, self.nothing)

        #Set default values
        cv2.setTrackbarPos('Threshold Value', 'Preview', 100)
        cv2.setTrackbarPos('Step Interval', 'Preview', 500)
        cv2.setTrackbarPos('Margin', 'Preview', 100)
        cv2.setTrackbarPos('Step Size', 'Preview', 2)

    def read_trackbars(self):
        """Read trackbar values"""
        self.tracking = bool(cv2.getTrackbarPos('Tracking Off/On', 'Preview'))
        self.show_threshold = bool(cv2.getTrackbarPos('Threshold', 'Preview'))
        self.threshold = cv2.getTrackbarPos('Threshold Value', 'Preview')
        self.step_interval = cv2.getTrackbarPos('Step Interval', 'Preview')
        self.margin = cv2.getTrackbarPos('Margin', 'Preview')
        self.step_size = cv2.getTrackbarPos('Step Size', 'Preview')
        self.draw_contour = bool(cv2.getTrackbarPos('Contour', 'Preview'))
        self.precision = cv2.getTrackbarPos('Precision', 'Preview')
        self.adaptive = bool(cv2.getTrackbarPos('Adaptive', 'Preview'))
        self.adaptive_value = cv2.getTrackbarPos('Adaptive Value', 'Preview')
        self.adaptive_size = cv2.getTrackbarPos('Adaptive Size', 'Preview')
        self.adaptive_gauss = bool(cv2.getTrackbarPos('Gaussian', 'Preview'))
        self.opening = cv2.getTrackbarPos('Opening', 'Preview')
        self.closing = cv2.getTrackbarPos('Closing', 'Preview')
        self.skeleton = bool(cv2.getTrackbarPos('Skeleton', 'Preview'))
        self.smoothing = cv2.getTrackbarPos('Smoothing', 'Preview')

    def find_worm(self):
        """Threshold and contouring algorithm to find centroid of worm"""
        self.img_gray = cv2.cvtColor(self.img, cv2.COLOR_BGR2GRAY)

        if self.adaptive:
            size = self.adaptive_size*2 + 1

            if self.adaptive_gauss:
                mode = cv2.ADAPTIVE_THRESH_MEAN_C
            else:
                mode = cv2.ADAPTIVE_THRESH_GAUSSIAN_C

            self.img_thresh = cv2.adaptiveThreshold(self.img_gray, 255,
                                                    mode,
                                                    cv2.THRESH_BINARY_INV,
                                                    size,
                                                    self.adaptive_value)

        else:
            #Threshold
            ret, self.img_thresh = cv2.threshold(self.img_gray, self.threshold,
                                                 255, cv2.THRESH_BINARY_INV)

        if self.opening != 0:
            self.img_thresh = cv2.morphologyEx(self.img_thresh,
                                               cv2.MORPH_OPEN,
                                               self.kernel,
                                               iterations=self.opening)

        if self.closing != 0:
            self.img_thresh = cv2.morphologyEx(self.img_thresh,
                                               cv2.MORPH_CLOSE,
                                               self.kernel,
                                               iterations=self.closing)

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

        #Compute the centroid of the worm contour
        moments = cv2.moments(self.worm)
        self.x = int(moments['m10']/moments['m00'])
        self.y = int(moments['m01']/moments['m00'])

    def skeletonise(self):
        x_in = self.worm[:, 0, 0]
        y_in = self.worm[:, 0, 1]

        tck, u = interpolate.splprep([x_in, y_in], per=True, s=self.smoothing,
                                     quiet=1)

        points = np.arange(0, 1.001, 0.001)

        spline = interpolate.splev(points, tck, der=0)
        x = spline[0]
        y = spline[1]
        self.worm_spline[:, 0, 0] = x
        self.worm_spline[:, 0, 1] = y

        d = interpolate.splev(points, tck, der=1)
        dx = d[0]
        dy = d[1]

        dd = interpolate.splev(points, tck, der=2)
        ddx = dd[0]
        ddy = dd[1]

        k = dx*ddy - dy*ddx
        k = np.absolute(k)
        k = k/((dx**2 + dy**2)**1.5)

        tail_n = np.argmax(k)
        k[tail_n-125:tail_n+125] = 0
        head_n = np.argmax(k)

        self.tail[0] = int(x[tail_n])
        self.tail[1] = int(y[tail_n])

        self.head[0] = int(x[head_n])
        self.head[1] = int(y[head_n])

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
            if self.skeleton:
                cv2.drawContours(self.img, [self.worm_spline],
                                 -1, (255, 0, 0), 2)
            else:
                cv2.drawContours(self.img, [self.worm], -1, (255, 0, 0), 2)

        #Draw markers for centroid and boundry
        cv2.circle(self.img, (self.x, self.y), 5, (0, 0, 255), -1)
        cv2.rectangle(self.img, (self.margin, self.margin),
                      (self.width-self.margin, self.height-self.margin),
                      (0, 255, 0),
                      2)

        #If skeletonise draw markers for head and tail
        if self.skeleton:
            cv2.circle(self.img, (self.tail[0], self.tail[1]),
                       5, (0, 255, 255), -1)
            cv2.circle(self.img, (self.head[0], self.head[1]),
                       5, (255, 255, 0), -1)

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

            if self.skeleton:
                self.skeletonise()

            self.update_gui()
            self.move_stage()

            now = cv2.getTickCount()
            fps = cv2.getTickFrequency()/(now - self.last_frame)
            self.last_frame = now

            print fps
