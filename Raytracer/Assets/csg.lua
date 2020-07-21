green = gr.material({0, 1, 0}, {0, 0, 0}, 0, 0, 0, 0)
white = gr.material({1, 1, 1}, {0, 0, 0}, 0, 0, 0, 0)
red = gr.material({1, 0, 0}, {0, 0, 0}, 0, 0, 0, 0)
black = gr.material({0, 0, 0}, {0, 0, 0}, 0, 0, 0, 0)
blue = gr.material({0, 0, 1}, {0, 0, 0}, 0, 0, 0, 0)

bluegreen = gr.material({0.1, 1, 0.6}, {0.5, 0.7, 0.5}, 25, 0, 0, 0)

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

-- sphere
s1 = gr.nh_sphere('s1', {0, -100, 300}, 60)
s1:set_material(bluegreen)

-- box
b1 = gr.nh_box('b1', {-50, -150, 250}, 100)
b1:set_material(red)


intersection = gr.csg(b1, s1, 'intersection')
scene_root:add_child(intersection)
intersection:rotate('y', -20)
intersection:translate(0, 0, 20)

-- cylinder 1-3
cy1 = gr.nh_cylinder('cy1', {0, -100, 300}, 15, 130)
cy1:set_material(green)

cy2 = gr.nh_cylinder('cy2', {0, -100, 300}, 15, 130)
cy2:set_material(green)
cy2:rotate('z', 90)
cy2:translate(-100, -100, 0)

cy3 = gr.nh_cylinder('cy3', {0, -100, 300}, 15, 130)
cy3:set_material(green)
cy3:rotate('x', 90)
cy3:translate(0, 200, 400)

union1 = gr.csg(cy1, cy2, 'union')
union2 = gr.csg(union1, cy3, 'union')
scene_root:add_child(union2)
union2:translate(20, 0, 0)

-- box 2
b2 = gr.nh_box('b2', {-50, -150, 250}, 100)
b2:set_material(red)

cy4 = gr.nh_cylinder('cy4', {0, -100, 300}, 15, 130)
cy4:set_material(blue)
cy4:rotate('x', 90)
cy4:translate(0, 200, 400)
diff = gr.csg(b2, cy4, 'diff')
scene_root:add_child(diff)
diff:translate(70, 150, 0)


white_light = gr.light({0, 100, 400}, {0.9, 0.9, 0.9}, {1, 0, 0})


gr.render(scene_root, 'csg.png', 500, 500,
	  {0, 0, 800}, {0, 0, -1}, {0, 1, 0}, 50,
	  {0.3, 0.3, 0.3}, {white_light})