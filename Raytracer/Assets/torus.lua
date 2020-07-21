-- A simple scene showing my torus

rootnode = gr.node('root')
rootnode:rotate('y', -20.0)
rootnode:scale( 3.5, 3.5, 3.5 )
rootnode:translate(0.0, 0.0, -30.0)

orange = gr.material({1.0, 0.6, 0.1}, {0.5, 0.7, 0.5}, 25, 0, 0, 0)
text1 = gr.texture('Assets/neko.png', {0.5, 0.7, 0.5}, 25, 0, 0, 0)

-- a torus
--torus = gr.torus('s5')
torus = gr.nh_torus('torus', {0, 2, 0}, 4.0, 2.0)
rootnode:add_child(torus)
torus:set_material(orange)
torus:set_texture(text1)
torus:translate(1,2,5)
torus:scale(0.3, 0.3, 0.3)


white_light = gr.light({-100.0, 150.0, 400.0}, {0.9, 0.9, 0.9}, {1, 0, 0})

gr.render(rootnode,
	  'torus.png', 500, 500,
	  {0, 0, 0}, {0, 0, -1}, {0, 1, 0}, 30,
	  {0.3, 0.3, 0.3}, {white_light})
