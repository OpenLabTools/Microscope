

import os
import wx
import subprocess # needed to run external program raspistill
from wx.lib.pubsub import Publisher


defaultfilename = 'image.jpg'
        
########################################################################
class ViewerPanel(wx.Panel):
    """
creates the main screen
"""
    
    #----------------------------------------------------------------------
    def __init__(self, parent):
        """set up for playing with images"""
        wx.Panel.__init__(self, parent)
        
        width, height = wx.DisplaySize()
       
        bitmap = wx.Bitmap('/home/pi/GUI Test/foobar.bmp')

        control = wx.StaticBitmap(self, -1, bitmap)
        control.SetPosition((10,10))
        
        

