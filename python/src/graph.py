import pyray as pr
import cv2
from cffi import FFI
from threading import Thread, Event
import time
import canvas

class Graph:
	def __init__(self, size:list, pos:list) -> None:
		self.canvas = canvas.Canvas2D(size, pos)
		self.collect = list()
		self.camera = pr.Camera2D(0,0,0,1)
		
	def __del__(self) -> None:
		pass

	def update(self):
		pass