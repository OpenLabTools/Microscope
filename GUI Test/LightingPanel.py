import wx
import os
import serial


maxi = 255
mini = 0 
sensitivity = 1
ser = serial.Serial('/dev/ttyUSB0', 9600)

class LightingPanel(wx.Panel):
     """
     This will be the first notebook tab
     """
     #----------------------------------------------------------------------
     def __init__(self, parent):
         """"""
 
         wx.Panel.__init__(self, parent=parent, id=wx.ID_ANY)
 
		
         gridsizer = wx.GridBagSizer(5, 25)
         
         #Panels and Labels
         pnl1 = wx.Panel(self, -1, style=wx.SIMPLE_BORDER)
         txt1 =  wx.StaticText(pnl1, id=-1, label=" LEFT", style=wx.ALIGN_CENTER, name="")
                  
         pnl2 = wx.Panel(self, -1, style=wx.SIMPLE_BORDER)
         txt2 = wx.StaticText(pnl2, id=-1, label = " RIGHT", style=wx.ALIGN_CENTER, name="")
         
         pnl3 = wx.Panel(self, -1, style=wx.SIMPLE_BORDER)
         txt3 = wx.StaticText(pnl3, id=-1, label = " FRONT", style=wx.ALIGN_CENTER, name="")
         
         pnl4 = wx.Panel(self, -1, style=wx.SIMPLE_BORDER)
         txt4 = wx.StaticText(pnl4, id=-1, label = " BACK", style=wx.ALIGN_CENTER, name="")
         
         pnl5 = wx.Panel(self, -1, style=wx.SUNKEN_BORDER)
         
         
         pnl6 = wx.Panel(self, -1, style=wx.NO_BORDER)
         
         #Buttons
         buttonbox = wx.BoxSizer(wx.VERTICAL)
         maxbutton = wx.Button(self, 1, 'MAX')
         minbutton = wx.Button(self, 2, 'MIN')
         self.Bind(wx.EVT_BUTTON, self.gotomax, id = 1)
         self.Bind(wx.EVT_BUTTON, self.gotomin, id = 2)
         buttonbox.Add(maxbutton, 1)
         buttonbox.Add(minbutton, 1)
         
         #Sensitivity Dropdown Box
         #sensitivity_text=wx.StaticText(buttonbox, id=-1, label = "Sensitivity:", style=wx.ALIGN_CENTER, name="")
         list1 = ['1x','2x','4x','10x']
         ddbox = wx.ComboBox(self, -1, pos=(50, 170), size=(150, -1), choices=list1, style=wx.CB_READONLY, value="Sensitivity")
         self.Bind(wx.EVT_COMBOBOX, self.OnSelection)
         buttonbox.Add(ddbox, 1)
         
         
         #Sliders and slider events
         self.sld_left = wx.Slider(pnl1, -1, 200, 0, 255, wx.DefaultPosition,
         (120,200), wx.SL_AUTOTICKS | wx.SL_VERTICAL | wx.SL_INVERSE)

         self.sld_right = wx.Slider(pnl2, -1, 150, 0, 255, wx.DefaultPosition,
         (120,200), wx.SL_AUTOTICKS | wx.SL_VERTICAL | wx.SL_INVERSE)

         self.sld_front = wx.Slider(pnl3, -1, 200, 0, 255, wx.DefaultPosition,
         (120,200), wx.SL_AUTOTICKS | wx.SL_VERTICAL | wx.SL_INVERSE)

         self.sld_back = wx.Slider(pnl4, -1, 200, 0, 255, wx.DefaultPosition,
         (120,200), wx.SL_AUTOTICKS | wx.SL_VERTICAL | wx.SL_INVERSE)

         self.Bind(wx.EVT_SCROLL_CHANGED, self.OnScroll)


         #Layout Panels in the window
         gridsizer.Add(pnl1, (2,0))
         gridsizer.Add(pnl2, (2,2))
         gridsizer.Add(pnl3, (3,1))
         gridsizer.Add(pnl4, (1,1))
         gridsizer.Add(buttonbox, (1,0))
         self.SetSize((750, 1000))
         self.SetSizer(gridsizer)
         #self.Centre()
         
     
     
     #Events    
     def OnScroll(self, event):
         left_level = chr( self.sld_left.GetValue() / sensitivity )
         right_level = chr( self.sld_right.GetValue() / sensitivity )
         front_level = chr( self.sld_front.GetValue() / sensitivity ) 
         back_level = chr( self.sld_back.GetValue() / sensitivity )
 
         ser.write(left_level)
         ser.write(right_level)
         ser.write(front_level)
         ser.write(back_level)

     def gotomax(self, event):
		 
		 for x in range(0,4):
			ser.write(chr(maxi / sensitivity))
		 
		 self.sld_left.SetValue(maxi)
		 self.sld_right.SetValue(maxi)
		 self.sld_front.SetValue(maxi)
		 self.sld_back.SetValue(maxi)
		 
     def gotomin(self, event):
		 for y in range(0,4):
			 ser.write(chr(mini/sensitivity))		 
		 
		 self.sld_left.SetValue(mini)
		 self.sld_right.SetValue(mini)
		 self.sld_front.SetValue(mini)
		 self.sld_back.SetValue(mini)
		 
     def OnSelection(self, event):
		 global sensitivity
		 sens = event.GetSelection()
		 
		 if(sens == 0 ):
			 sensitivity = 1
		 
		 if(sens == 1 ):
			 sensitivity = 2
		 
		 if(sens == 2 ):
			 sensitivity = 4
		 
		 if(sens == 3 ):
			 sensitivity = 10
		
		 
		 
		 	 



