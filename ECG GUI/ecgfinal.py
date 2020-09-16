import sys, serial, argparse
import numpy as np
from time import sleep
from collections import deque
from tkinter import *
import matplotlib.pyplot as plt 
import matplotlib.animation as animation

    
# plot class
class AnalogPlot:
  # constr
  def __init__(self, strPort, maxLen):
      # open serial port
      self.ser = serial.Serial(strPort, 115200)

      self.ax = deque([0.0]*maxLen)
      self.ay = deque([0.0]*maxLen)
      self.az = deque([0.0]*maxLen)
      self.maxLen = maxLen

  # add to buffer
  def addToBuf(self, buf, val):
      if len(buf) < self.maxLen:
          buf.append(val)
      else:
          buf.pop()
          buf.appendleft(val)

  # add data
  def add(self, data):
      assert(len(data) == 5)
      self.addToBuf(self.ax, data[0])
      self.addToBuf(self.ay, data[1])
      self.addToBuf(self.az, data[2])

  # update plot
  def update(self, frameNum, a0, a1, a2): 
      try:
          line = self.ser.readline()
          data = [float(val) for val in line.split()]
          # print data
        
          if(len(data) == 5):
              self.add(data)
              a0.set_data(range(self.maxLen), self.ax)
              a1.set_data(range(self.maxLen), self.ay)
              a2.set_data(range(self.maxLen), self.az)
              plt.title("ECG 6-LEAD\n"+"C1: "+str(data[0])+"  C2: "+str(data[1])+"  C3: "+str(data[2])+"  BPM: "+str(data[3])+"  Spo2: "+str(data[4]))
      except ValueError:
          print("Float error!!! ");
      except KeyboardInterrupt:
          print('exiting')
      
      return a0,

  # clean up
  def close(self):
      # close serial
      self.ser.flush()
      self.ser.close()    

# main() function
def main():
 
  analogPlot = AnalogPlot('COM3', 100)

  print('plotting data...')
  analogPlot.update
  
  # set up animation
  fig = plt.figure(figsize=(4, 4))
  ax = plt.axes(xlim=(100, 0), ylim=(5960000,6040000))
  ax.set_ylabel("ECG Output")
  ax.set_xlabel("Time")
  a0, = ax.plot([], [])
  a1, = ax.plot([], [])
  a2, = ax.plot([], [])
  anim = animation.FuncAnimation(fig, analogPlot.update, fargs=(a0, a1, a2), interval=25)

  # show plot
  plt.show()
  
  # clean up
  analogPlot.close()

  print('exiting.')
  

# call main
if __name__ == '__main__':
  main()
