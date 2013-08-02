import wx, os
import LightingPanel
import TabPanel
import Panel1
import PiGUI

class NotebookDemo(wx.Notebook):
     """
     Notebook class
     """
 
     #----------------------------------------------------------------------
     def __init__(self, parent):
         wx.Notebook.__init__(self, parent, id=wx.ID_ANY, style=
                              wx.BK_DEFAULT
                              #wx.BK_TOP 
                              #wx.BK_BOTTOM
                              #wx.BK_LEFT
                              #wx.BK_RIGHT
                              )
 
         # Create the first tab and add it to the notebook
         tabOne = LightingPanel.LightingPanel(self)
         
         self.AddPage(tabOne, "Lighting")
 
         # Show how to put an image on one of the notebook tabs,
         # first make the image list:
         il = wx.ImageList(16, 16)
         #idx1 = il.Add(images.Smiles.GetBitmap())
         self.AssignImageList(il)
 
         # now put an image on the first tab we just created:
         #self.SetPageImage(0, idx1)
 
         # Create and add the second tab
         #tabTwo = Panel1.Panel1(self,7)
         #self.AddPage(tabTwo, "Main")
 
         # Create and add the third tab
         self.AddPage(PiGUI.ViewerPanel(self), "Camera Settings")
 
         self.Bind(wx.EVT_NOTEBOOK_PAGE_CHANGED, self.OnPageChanged)
         self.Bind(wx.EVT_NOTEBOOK_PAGE_CHANGING, self.OnPageChanging)
         
         
 
     def OnPageChanged(self, event):
         old = event.GetOldSelection()
         new = event.GetSelection()
         sel = self.GetSelection()
         #print 'OnPageChanged,  old:%d, new:%d, sel:%d\n' % (old, new, sel)
         event.Skip()
 
     def OnPageChanging(self, event):
         old = event.GetOldSelection()
         new = event.GetSelection()
         sel = self.GetSelection()
         #print 'OnPageChanging, old:%d, new:%d, sel:%d\n' % (old, new, sel)
         event.Skip()
