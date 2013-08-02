

import wx, os
import NotebookDemo
import time

class DemoFrame(wx.Frame):
	def __init__(self):
         """Constructor"""
         wx.Frame.__init__(self, None, wx.ID_ANY,
                           "Microscope Example GUI",
                           size=(625,1000)
                          )
         panel = wx.Panel(self)
         
 
         notebook = NotebookDemo.NotebookDemo(panel)
         sizer = wx.BoxSizer(wx.VERTICAL)
         sizer.Add(notebook, 1, wx.ALL|wx.EXPAND, 5)
         panel.SetSizer(sizer)
         self.Layout()
         self.Move((0,0))
 
         self.Show()
	
if __name__ == "__main__":
	 
	app = wx.PySimpleApp()
	frame = DemoFrame()
	app.MainLoop()
