import pyray as pr
import video

def testcallback():
	# print("yes")
	pass

pr.init_window(0,0, "raycv-py nail project")

size = [pr.get_screen_width()*3/4,pr.get_screen_height()*3/4 ]
# size = [ int(i) for i in size ]
size = [ int(i) for i in [640,480] ]

# fontsize = (size[0]*2)//100
fontsize = 10

print(fontsize)

pr.set_window_size(size[0], size[1])

pr.set_target_fps(60)
cap = video.Video([size[0]*40//100,size[1]*40//100], [10,10])

cap.callback = testcallback

timelimit = "0"
timelimit_i = 0
activate = False

while not pr.window_should_close():

	cap.update()

	pr.begin_drawing()
	pr.clear_background( pr.RAYWHITE )

	cap.draw()

	pr.draw_text(
		"DESIRED FRAMRATE : {}".format(int(cap.fps)),
		cap.canvas.pos[0] + cap.canvas.size[0] + 5,
		10 + 5,
		fontsize,
		pr.GRAY
	)

	cap.fps = pr.gui_slider_bar(
		pr.Rectangle(
			cap.canvas.pos[0] + cap.canvas.size[0] + 200,
			10 + 5,
			100,
			fontsize
		),
		"","", cap.fps, cap.fps_min, cap.fps_max
	)

	pr.draw_text(
		"INTERVAL : {}".format(1/cap.fps),
		cap.canvas.pos[0] + cap.canvas.size[0] + 5,
		10 + 5 + fontsize,
		fontsize,
		pr.GRAY
	)

	pr.draw_text(
		"ACTUAL FRAMRATE : {}".format(int(cap.actual_fps)),
		cap.canvas.pos[0] + cap.canvas.size[0] + 5,
		10 + 5 + fontsize*2,
		fontsize,
		pr.BLUE
	)

	pr.draw_text(
		"TIME LIMIT",
		cap.canvas.pos[0] + cap.canvas.size[0] + 5,
		10 + 5 + fontsize*3,
		fontsize,
		pr.BLUE
	)

	if pr.gui_text_box(
		pr.Rectangle(
			cap.canvas.pos[0] + cap.canvas.size[0] + 200,
			10 + 5 + fontsize*3,
			50,
			fontsize
		),
		timelimit,
		20,
		activate
	):
		timelimit_i = int(timelimit)
		activate = not activate

	pr.draw_text(
		"ZONE SIZE : {}".format(0),
		cap.canvas.pos[0] + cap.canvas.size[0] + 5,
		10 + 5 + fontsize*4,
		fontsize,
		pr.GRAY
	)

	pr.draw_line(10,10, 10,size[1]-10, pr.BLACK)
	pr.draw_line(10,10, size[0]-10,10, pr.BLACK)
	pr.draw_line(size[0]-10,size[1]-10, 10,size[1]-10, pr.BLACK)
	pr.draw_line(size[0]-10,size[1]-10, size[0]-10,10, pr.BLACK)
	pr.draw_line(
		cap.canvas.pos[0],
		cap.canvas.pos[1] + cap.canvas.size[1],
		size[0]-10,
		cap.canvas.pos[1] + cap.canvas.size[1],
		pr.BLACK
	)
	
	pr.end_drawing()

cap.end()
pr.close_window()