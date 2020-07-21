-- puppet.lua
-- My puppet is a Japanese mascot called Kumamon. It is a black bear.

rootnode = gr.node('root')
rootnode:scale( 0.25, 0.25, 0.25 )
rootnode:translate(0.0, 0.0, -1.0)

red = gr.material({1.0, 0.0, 0.0}, {0.1, 0.1, 0.1}, 10)
black = gr.material({0.0, 0.0, 0.0}, {0.1, 0.1, 0.1}, 5)
white = gr.material({1.0, 1.0, 1.0}, {1.0, 1.0, 1.0}, 3)

-- Torso
torso = gr.mesh('sphere', 'torso')
rootnode:add_child(torso)
torso:set_material(black)
torso:scale( 0.8, 0.8, 0.7 )

-- Head Joint between head and torso
headJoint = gr.joint('headJoint', {0, 0, 0.0}, {-15.0, 0, 15.0})
torso:add_child(headJoint)

-- Head
head = gr.mesh('sphere', 'head')
headJoint:add_child(head)
head:scale(0.8, 0.7, 0.7)
head:translate(0.0, 1.0, 0.1)
head:rotate('x', -11)
head:set_material(black)


-- Middle
middle = gr.mesh('sphere', 'middle')
head:add_child(middle)
middle:translate(0, -0.7, 3.3)
middle:scale(0.5, 0.4, 0.3)
middle:rotate('x', 4)
middle:set_material(white)

-- Nose
nose = gr.mesh('sphere', 'nose')
middle:add_child(nose)
nose:scale(0.3, 0.3, 0.7)
nose:translate(0, 0.4, 0.4)
nose:set_material(black)

-- Mouth
mouth = gr.mesh('sphere', 'mouth')
middle:add_child(mouth)
mouth:scale(0.3, 0.5, 0.7)
mouth:translate(0, -0.2, 0.4)
mouth:set_material(black)


-- Left Eye Brow
leftEyebrow = gr.mesh('sphere', 'leftEyebrow')
head:add_child(leftEyebrow)
leftEyebrow:scale(0.2, 0.2, 0.05)
leftEyebrow:rotate('y', -30.0)
leftEyebrow:translate(-0.5, 0.3, 0.7)
leftEyebrow:set_material(white)

-- Right Eye Brow
rightEyebrow = gr.mesh('sphere', 'rightEyebrow')
head:add_child(rightEyebrow)
rightEyebrow:scale(0.2, 0.2, 0.05)
rightEyebrow:rotate('y', 30.0)
rightEyebrow:translate(0.5, 0.3, 0.7)
rightEyebrow:set_material(white)

-- Left Eye
leftEye = gr.mesh('sphere', 'leftEye')
head:add_child(leftEye)
leftEye:scale(0.2, 0.2, 0.05)
leftEye:rotate('y', -30.0)
leftEye:translate(-0.4, 0.1, 0.9)
leftEye:set_material(white)

-- Right Eye
rightEye = gr.mesh('sphere', 'rightEye')
head:add_child(rightEye)
rightEye:scale(0.2, 0.2, 0.05)
rightEye:rotate('y', 30.0)
rightEye:translate(0.4, 0.1, 0.9)
rightEye:set_material(white)

-- Left Eyeball
leftEyeball = gr.mesh('sphere', 'leftEyeball')
leftEye:add_child(leftEyeball)
leftEyeball:scale(0.2, 0.3, 0.1)
leftEyeball:translate(0.2, 0, 1)
leftEyeball:set_material(black)


-- Right Eyeball
rightEyeball = gr.mesh('sphere', 'rightEyeball')
rightEye:add_child(rightEyeball)
rightEyeball:scale(0.2, 0.3, 0.1)
rightEyeball:translate(-0.2, 0, 1)
rightEyeball:set_material(black)


-- Left Cheek
leftCheek = gr.mesh('sphere', 'leftCheek')
head:add_child(leftCheek)
leftCheek:scale(0.25, 0.25, 0.05)
leftCheek:rotate('y', -40.0)
leftCheek:translate(-0.7, -0.1, 0.7)
leftCheek:set_material(red)

-- Right Cheek
rightCheek = gr.mesh('sphere', 'rightCheek')
head:add_child(rightCheek)
rightCheek:scale(0.25, 0.25, 0.05)
rightCheek:rotate('y', 40.0)
rightCheek:translate(0.7, -0.1, 0.7)
rightCheek:set_material(red)

-- Outer Left ear
outerLeftEar = gr.mesh('sphere', 'outerLeftEar')
head:add_child(outerLeftEar)
outerLeftEar:scale(0.15, 0.15, 0.15)
outerLeftEar:translate(-0.8, 0.7, 0.2)
outerLeftEar:set_material(black)

-- Inner Left ear
innerLeftEar = gr.mesh('sphere', 'innerLeftEar')
outerLeftEar:add_child(innerLeftEar)
innerLeftEar:scale(0.5, 0.5, 0.1)
innerLeftEar:translate(0, -0.1, 1)
innerLeftEar:set_material(white)


-- Outer Right ear
outerRightEar = gr.mesh('sphere', 'outerRightEar')
head:add_child(outerRightEar)
outerRightEar:scale(0.15, 0.15, 0.15)
outerRightEar:translate(0.8, 0.7, 0.2)
outerRightEar:set_material(black)

-- Inner Right ear
innerRightEar = gr.mesh('sphere', 'innerRightEar')
outerRightEar:add_child(innerRightEar)
innerRightEar:scale(0.5, 0.5, 0.1)
innerRightEar:translate(0, -0.1, 1)
innerRightEar:set_material(white)


-- Left Arm Joint between left arm and torso
leftArmJoint = gr.joint('leftArmJoint', {-15.0, 0, 15.0}, {0, 0, 0})
torso:add_child(leftArmJoint)

-- Left Arm
leftArm = gr.mesh('sphere', 'leftArm')
leftArmJoint:add_child(leftArm)
leftArm:set_material(black)
leftArm:scale(2.0, 1.1, 2.0);
leftArm:scale(0.2, 0.1, 0.1);
leftArm:rotate('z', -50);
leftArm:translate(-0.9, 0.7, 0.3)

-- Left Palm Joint between left arm and left palm
leftPalmJoint = gr.joint('leftPalmJoint', {0, 0, 90.0}, {0, 0, 0})
leftArm:add_child(leftPalmJoint)


-- Left Palm
leftPalm = gr.mesh('sphere', 'leftPalm')
leftPalmJoint:add_child(leftPalm)
leftPalm:scale(0.5, 1.0, 1.0)
leftPalm:translate(-1.2, 0, 0)
leftPalm:set_material(black)


-- Left Thumb
leftThumb = gr.mesh('sphere', 'leftThumb')
leftPalm:add_child(leftThumb)
leftThumb:translate(-0.8, 0, -3.0)
leftThumb:scale(0.6, 0.5, 0.3)
leftThumb:rotate('y', -30.0)
leftThumb:set_material(black)


-- Right Arm Joint between right arm and torso
rightArmJoint = gr.joint('rightArmJoint', {-15.0, 0, 15.0}, {0, 0, 0})
torso:add_child(rightArmJoint)


-- Right Arm
rightArm = gr.mesh('sphere', 'rightArm')
rightArmJoint:add_child(rightArm)
rightArm:set_material(black)
rightArm:scale(2.0, 1.1, 2.0);
rightArm:scale(0.2, 0.1, 0.1)
rightArm:rotate('z', 50);
rightArm:translate(0.9, 0.7, 0.3)

-- Right Palm Joint between right arm and right palm
rightPalmJoint = gr.joint('rightPalmJoint', {0, 0, 90.0}, {0, 0, 0})
rightArm:add_child(rightPalmJoint)

-- Right Palm
rightPalm = gr.mesh('sphere', 'rightPalm')
rightPalmJoint:add_child(rightPalm)
rightPalm:scale(0.5, 1.0, 1.0)
rightPalm:translate(1.2, 0, 0)
rightPalm:set_material(black)

-- Right Thumb
rightThumb = gr.mesh('sphere', 'rightThumb')
rightPalm:add_child(rightThumb)
rightThumb:translate(0.8, 0, -3.0)
rightThumb:scale(0.6, 0.5, 0.3)
rightThumb:rotate('y', 30.0)
rightThumb:set_material(black)

-- Hip
hip = gr.mesh('sphere', 'hip')
torso:add_child(hip)
hip:scale(1.1, 0.8, 1)
hip:translate(0.0, -0.5, 0.0)
hip:set_material(black)

-- Tail
tail = gr.mesh('sphere', 'tail')
hip:add_child(tail)
tail:scale(0.2, 0.2, 0.2)
tail:translate(0, 0, -1.0)
tail:set_material(black)

-- Left Leg Joint between left leg and hip
leftLegJoint = gr.joint('leftLegJoint', {-30.0, 0, 30.0}, {0, 0, 0})
hip:add_child(leftLegJoint)

-- Left Leg
leftLeg = gr.mesh('sphere', 'leftLeg')
leftLegJoint:add_child(leftLeg)
leftLeg:scale(0.2, 0.5, 0.3)
leftLeg:translate(-0.5, -0.7, 0)
leftLeg:set_material(black)

-- Left Foot Joint between left foot and left leg
leftFootJoint = gr.joint('leftFootJoint', {0, 0, 0}, {0, 0, 70.0})
leftLeg:add_child(leftFootJoint)

-- Left Foot
leftFoot = gr.mesh('sphere', 'leftFoot')
leftFootJoint:add_child(leftFoot)
leftFoot:scale(2, 0.5, 1)
leftFoot:translate(-1.2, -0.9, 0)
leftFoot:rotate('y', 20.0)
leftFoot:set_material(black)


-- Right Leg Joint between right leg and hip
rightLegJoint = gr.joint('rightLegJoint', {-30.0, 0, 30.0}, {0, 0, 0})
hip:add_child(rightLegJoint)


-- Right Leg
rightLeg = gr.mesh('sphere', 'rightLeg')
rightLegJoint:add_child(rightLeg)
rightLeg:scale(0.2, 0.5, 0.3)
rightLeg:translate(0.5, -0.7, 0)
rightLeg:set_material(black)

-- Right Foot Joint between right foot and right leg
rightFootJoint = gr.joint('rightFootJoint', {0, 0, 0}, {-70.0, 0, 0})
rightLeg:add_child(rightFootJoint)

-- Right Foot
rightFoot = gr.mesh('sphere', 'rightFoot')
rightFootJoint:add_child(rightFoot)
rightFoot:scale(2, 0.5, 1)
rightFoot:translate(1.2, -0.9, 0)
rightFoot:rotate('y', -20.0)
rightFoot:set_material(black)



return rootnode

