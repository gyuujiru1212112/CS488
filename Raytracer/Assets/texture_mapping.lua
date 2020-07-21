-- cylinder, cone, sphere, box, torus

green = gr.material({0, 1, 0}, {0, 0, 0}, 0, 0, 0, 0)
white = gr.material({1, 1, 1}, {0, 0, 0}, 0, 0, 0, 0)
red = gr.material({1, 0, 0}, {0, 0, 0}, 0, 0, 0, 0)
black = gr.material({0, 0, 0}, {0, 0, 0}, 0, 0, 0, 0)
blue = gr.material({0, 0, 1}, {0, 0, 0}, 0, 0, 0, 0)

-- primitive texture
earth = gr.texture('Assets/earth.png', {0.5, 0.4, 0.8}, 25, 0, 0, 0)
wood = gr.texture('Assets/wood.png', {0.5, 0.4, 0.8}, 25, 0, 0, 0)
neko = gr.texture('Assets/neko.png', {0.5, 0.7, 0.5}, 25, 0, 0, 0)
blackdot = gr.texture('Assets/blackdot.png', {0.5, 0.4, 0.8}, 25, 0, 0, 0)
donut = gr.texture('Assets/donut.png', {0.5, 0.4, 0.8}, 25, 0, 0, 0)

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
s1 = gr.nh_sphere('s1', {-50, -100, 300}, 50)
scene_root:add_child(s1)
s1:set_texture(earth)

-- box
b1 = gr.nh_box('b1', {-50, -200, 300}, 50)
scene_root:add_child(b1)
b1:set_texture(wood)

-- cylinder
cy1 = gr.nh_cylinder('cy1', {30, -130, 310}, 35, 70)
scene_root:add_child(cy1)
cy1:set_texture(neko)
cy1:rotate('z', 20)

-- cone
co1 = gr.nh_cone('co1', {50, -200, 330}, 35, 55)
scene_root:add_child(co1)
co1:set_texture(blackdot)

-- torus
t1 = gr.nh_torus('t1', {30, -100, 250}, 30, 15)
scene_root:add_child(t1)
t1:set_texture(donut)
t1:rotate('x', -20)


white_light = gr.light({0, 100, 400}, {0.9, 0.9, 0.9}, {1, 0, 0})


gr.render(scene_root, 'textures.png', 500, 500,
	  {0, 0, 800}, {0, 0, -1}, {0, 1, 0}, 50,
	  {0.3, 0.3, 0.3}, {white_light})