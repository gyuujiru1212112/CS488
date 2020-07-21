-- material
green = gr.material({0, 1, 0}, {0, 0, 0}, 0, 0, 0, 0)
white = gr.material({1, 1, 1}, {0, 0, 0}, 0, 0, 0, 0)
red = gr.material({1, 0, 0}, {0, 0, 0}, 0, 0, 0, 0)
black = gr.material({0, 0, 0}, {0, 0, 0}, 0, 0, 0, 0)
blue = gr.material({0, 0, 1}, {0, 0, 0}, 0, 0, 0, 0)

pink = gr.material({1, 0.3, 0.5}, {0.5, 0.7, 0.5}, 25, 0, 0, 0)
yello = gr.material({1, 1, 0.1}, {0.5, 0.7, 0.5}, 25, 0, 0, 0)

glass = gr.material({0, 0, 0}, {1, 1, 1}, 10000000, 0.2, 1, 1.5)
mirror = gr.material({0, 0, 0}, {1, 1, 1}, 10000, 0.8, 0, 0)

-- texture
wood = gr.texture('Assets/wood.png', {0, 0, 0}, 25, 0, 0, 0)
tableWood = gr.texture('Assets/tableWood.png', {0, 0, 0}, 2500, 0.2, 0, 0)
waffleCone = gr.texture('Assets/waffle_cone.png', {0, 0, 0}, 25, 0, 0, 0)

scene_root = gr.node('root')

ceiling = gr.nh_box('ceiling', {-200, 200, 0}, 400)
ceiling:set_material(white)
scene_root:add_child(ceiling)

right_wall = gr.nh_box('right_wall', {200, -200,0}, 400)
right_wall:set_material(white)
scene_root:add_child(right_wall)

left_wall = gr.nh_box('left_wall', {-600, -200, 0}, 400)
left_wall:set_material(white)
scene_root:add_child(left_wall)

back_wall = gr.nh_box('back_wall', {-200, -200, -400}, 400)
back_wall:set_material(mirror)
scene_root:add_child(back_wall)

floor = gr.nh_box('floor', {-200, -600, 0}, 400)
floor:set_texture(wood)
scene_root:add_child(floor)

-- table
table = gr.cube('table')
--scene_root:add_child(table)
table:scale(300, 200, 100)
table:translate(-150, -200, 300)
table:set_texture(tableWood)
table:rotate('x', 270)
table:translate(0, -500, 200)

-- sphere hole
s1 = gr.nh_sphere('s1', {0, -230, 330}, 100)
s1:set_texture(tableWood)
diff = gr.csg(table, s1, 'diff')
scene_root:add_child(diff)

--------------- mug
-- cylinder1
cy1 = gr.nh_cylinder('cy1', {30, -130, 300}, 25, 60)
cy1:set_material(blue)
cy1:translate(-120, 60, 50)

cy2 = gr.nh_cylinder('cy2', {30, -120, 300}, 20, 50)
cy2:set_material(green)
cy2:translate(-120, 60, 50)

diffCy = gr.csg(cy1, cy2, 'diff')
scene_root:add_child(diffCy)

 

t1 = gr.nh_torus('t1', {-60, -70, 350}, 13, 6)
t1:set_material(green)
scene_root:add_child(t1)

--------------- bowl
s2 = gr.nh_sphere('s2', {0, -70, 350}, 30)
s2:set_material(red)

s3 = gr.nh_sphere('s3', {0, -60, 350}, 27)
s3:set_material(green)
diffs2s3 = gr.csg(s2, s3, 'diff')
scene_root:add_child(diffs2s3)
diffs2s3:translate(90, 0, 0)


--------------- teapot

teapot = gr.mesh('teapot', 'Assets/teapot.obj')
scene_root:add_child(teapot)
teapot:set_material(pink)
teapot:scale(20, 20, 20)
teapot:translate(60, -100, 300)

--------------- ice cream
cone = gr.nh_cone('cone', {30, -130, 300}, 25, 70)
cone:set_texture(waffleCone)
scene_root:add_child(cone)
cone:translate(0, 100, 0)
cone:rotate('z', 180)
cone:translate(-10, -50, 0)

ice1 = gr.nh_sphere('ice1', {-40, -20, 300}, 20)
ice1:set_material(pink)
scene_root:add_child(ice1)

--ice cream base
base = gr.nh_torus('ice1', {-110, -20, 300}, 17, 3)
base:set_material(white)
scene_root:add_child(base)
base:rotate('x', -90)
base:translate(70, -340, 280)

base2 = gr.nh_torus('ice1', {-110, -20, 300}, 8, 3)
base2:set_material(white)
scene_root:add_child(base2)
base2:rotate('x', -90)
base2:translate(70, -370, 280)

base3 = gr.nh_torus('ice1', {-110, -20, 300}, 5, 3)
base3:set_material(white)
scene_root:add_child(base3)
base3:rotate('x', -90)
base3:translate(70, -390, 280)

white_light = gr.light({0, 100, 400}, {0.9, 0.9, 0.9}, {1, 0, 0}, 50, 50)


gr.render(scene_root, 'final_scene.png', 500, 500,
	  {0, 0, 800}, {0, 0, -1}, {0, 1, 0}, 50,
	  {0.3, 0.3, 0.3}, {white_light}, 50, 1, 40)
