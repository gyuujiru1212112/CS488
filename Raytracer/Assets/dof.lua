green = gr.material({0, 1, 0}, {0, 0, 0}, 0, 0, 0, 0)
white = gr.material({1, 1, 1}, {0, 0, 0}, 0, 0, 0, 0)
red = gr.material({1, 0, 0}, {0, 0, 0}, 0, 0, 0, 0)
black = gr.material({0, 0, 0}, {0, 0, 0}, 0, 0, 0, 0)
blue = gr.material({0, 0, 1}, {0, 0, 0}, 0, 0, 0, 0)

bluegreen = gr.material({0.1, 1, 0.6}, {0.5, 0.7, 0.5}, 25, 0, 0, 0)
orange = gr.material({1, 0.6, 0}, {0.5, 0.7, 0.5}, 25, 0, 0, 0)
pink = gr.material({1, 0.3, 0.5}, {0.5, 0.7, 0.5}, 25, 0, 0, 0)

scene_root = gr.node('root')

ceiling = gr.nh_box('ceiling', {-200, 200, 0}, 400)
ceiling:set_material(black)
scene_root:add_child(ceiling)

right_wall = gr.nh_box('right_wall', {200, -200,0}, 400)
right_wall:set_material(green)
scene_root:add_child(right_wall)

left_wall = gr.nh_box('left_wall', {-600, -200, 0}, 400)
left_wall:set_material(red)
scene_root:add_child(left_wall)

back_wall = gr.nh_box('back_wall', {-200, -200, -400}, 400)
back_wall:set_material(blue)
scene_root:add_child(back_wall)

floor = gr.nh_box('floor', {-200, -600, 0}, 400)
floor:set_material(white)
scene_root:add_child(floor)

white_light = gr.light({0, 100, 400}, {0.9, 0.9, 0.9}, {1, 0, 0})


-- sphere
s1 = gr.nh_sphere('s1', {-90, -100, 300}, 50)
scene_root:add_child(s1)
s1:set_material(bluegreen)

s2 = gr.nh_sphere('s2', {0, -100, 100}, 50)
scene_root:add_child(s2)
s2:set_material(pink)


gr.render(scene_root, 'dof.png', 500, 500,
	  {0, 0, 800}, {0, 0, -1}, {0, 1, 0}, 50,
	  {0.3, 0.3, 0.3}, {white_light}, 100, 1, 1, 500, 35)
