import pyray as pr

class Canvas2D:
	def __init__(self, size, pos) -> None:
		self.render_texture = pr.load_render_texture( size[0], size[1] )
		self.size = size
		self.pos = pos
		self.rect = pr.Rectangle(0,0,size[0],size[1])
		self.tint = pr.WHITE
	
	def __del__(self):
		pr.unload_render_texture(self.render_texture)
	
	def begin(self) -> None: pr.begin_texture_mode(self.render_texture)
	def end(self) -> None: pr.end_texture_mode()
	def draw(self) -> None:
		pr.draw_texture_rec(
			self.render_texture.texture,
			pr.Rectangle(
				self.rect.x, self.rect.y,
				self.rect.width, -self.rect.height
			),
			self.pos,
			self.tint
		)
	def setpos(self, pos) -> None: self.pos = pos

	def is_mouse_in(self) -> bool:
		return pr.check_collision_point_rec(
			pr.get_mouse_position(),
			pr.Rectangle(
				self.pos.x, self.pos.y,
				self.rect.width, self.rect.height
			)
		)
