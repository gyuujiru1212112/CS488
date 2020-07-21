-- A simple scene showing my teapot

rootnode = gr.node('root')
--rootnode:rotate('y', -20.0)
rootnode:scale( 2, 2, 2 )
rootnode:translate(0.0, 0.0, -30.0)

orange = gr.material({1.0, 0.6, 0.1}, {0.5, 0.7, 0.5}, 25, 0, 0, 0)

-- a teapot
teapot = gr.mesh('teapot', 'Assets/teapot.obj')
rootnode:add_child(teapot)
teapot:set_material(orange)
teapot:rotate('y', -10)


white_light = gr.light({-100.0, 150.0, 400.0}, {0.9, 0.9, 0.9}, {1, 0, 0})

gr.render(rootnode,
	  'teapot_phong.png', 256, 256,
	  {0, 0, 0}, {0, 0, -1}, {0, 1, 0}, 30,
	  {0.3, 0.3, 0.3}, {white_light})
