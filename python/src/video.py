import pyray as pr
import cv2
from cffi import FFI
from threading import Thread, Event
import time
import canvas

class Video:

	def grab(self):
		while not self.done.is_set():
			self.cap.grab()

	def get(self):
		while not self.done.is_set():
			now = time.time()
			# print( now-self.time, 1/self.fps, self.fps_max )
			if now - self.time >= 1/self.fps:
				self.count +=1
				ret, self.frame = self.cap.retrieve()

				self.time = now
				self.new_frame.set()

				if self.callback is not None: self.callback()
			
			if now - self.second >= 1:
				self.actual_fps = self.count
				self.count = 0
				self.second = now
	
	def __init__(self, size:list, pos:list) -> None:
		self.canvas = canvas.Canvas2D( size, pos )
		self.count = 0
		self.ffi = FFI()
		self.cap = cv2.VideoCapture(0)

		self.fps_max = self.cap.get(cv2.CAP_PROP_FPS)
		self.fps_min = 1
		self.fps = self.fps_max
		self.actual_fps = 0
		self.cap.set( cv2.CAP_PROP_BUFFERSIZE, 1 )
		ret, self.frame = self.cap.read()

		self.gray = cv2.cvtColor( self.frame, cv2.COLOR_BGR2GRAY )

		self.image = pr.Image(
			self.ffi.cast( 'void *', self.gray.ctypes.data ),
			self.gray.shape[1],
			self.gray.shape[0],
			1,
			pr.PixelFormat.PIXELFORMAT_UNCOMPRESSED_GRAYSCALE)

		self.texture = pr.load_texture_from_image(self.image)
		self.done = Event()
		self.done.clear()

		self.new_frame = Event()
		self.new_frame.clear()

		self.callback = None
		self.time = self.second = time.time()

		self.g_t = Thread(target=self.grab)
		self.c_t = Thread(target=self.get)
		
		self.g_t.start()
		self.c_t.start()

	def __del__(self) -> None:
		self.g_t.join()
		self.c_t.join()
		pr.unload_texture(self.texture)
		self.cap.release()

	def end(self): self.done.set()

	def update(self) -> None:
		if self.new_frame.is_set():
			self.gray = cv2.cvtColor( self.frame, cv2.COLOR_BGR2GRAY )
			# self.image.data = self.ffi.cast( 'void *', self.gray.ctypes.data )
			# pr.image_draw_rectangle_lines(
			# 	self.image,
			# 	pr.Rectangle(
			# 		self.image.width//2 - 10,
			# 		self.image.height//2 - 10,
			# 		20,20, 
			# 	),
			# 	1,
			# 	pr.WHITE
			# )
			pr.update_texture(self.texture, self.ffi.cast( 'void *', self.gray.ctypes.data ))
			# pr.update_texture(self.texture, self.image.data )
			self.new_frame.clear()

			self.canvas.begin()
			pr.clear_background(pr.BLACK)

			pr.draw_texture_rec(
				self.texture,
				pr.Rectangle(
					0,0,
					self.image.width, self.image.height
				),
				pr.Vector2(
					-self.image.width//2 + self.canvas.size[0]//2,
					-self.image.height//2 + self.canvas.size[1]//2
				),
				pr.WHITE
			)

			# pr.draw_circle(
			# 	self.canvas.size[0]//2,
			# 	self.canvas.size[1]//2,
			# 	20, pr.RED
			# )
			self.canvas.end()
	
	def draw(self): self.canvas.draw()