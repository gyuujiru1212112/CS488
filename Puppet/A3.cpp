#include "A3.hpp"
#include "scene_lua.hpp"
using namespace std;

#include "cs488-framework/GlErrorCheck.hpp"
#include "cs488-framework/MathUtils.hpp"
#include "GeometryNode.hpp"
#include "JointNode.hpp"

#include <imgui/imgui.h>

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/io.hpp>
#include <glm/gtc/matrix_transform.hpp>

using namespace glm;

static bool show_gui = true;

const size_t CIRCLE_PTS = 48;

//----------------------------------------------------------------------------------------
// Constructor
A3::A3(const std::string & luaSceneFile)
	: m_luaSceneFile(luaSceneFile),
	  m_positionAttribLocation(0),
	  m_normalAttribLocation(0),
	  m_vao_meshData(0),
	  m_vbo_vertexPositions(0),
	  m_vbo_vertexNormals(0),
	  m_vao_arcCircle(0),
	  m_vbo_arcCircle(0)
{

}

//----------------------------------------------------------------------------------------
// Destructor
A3::~A3()
{

}

//----------------------------------------------------------------------------------------
/*
 * Called once, at program start.
 */
void A3::init()
{
	// Set the background colour.
	glClearColor(0.35, 0.35, 0.35, 1.0);

	createShaderProgram();

	glGenVertexArrays(1, &m_vao_arcCircle);
	glGenVertexArrays(1, &m_vao_meshData);
	enableVertexShaderInputSlots();

	processLuaSceneFile(m_luaSceneFile);

	// Load and decode all .obj files at once here.  You may add additional .obj files to
	// this list in order to support rendering additional mesh types.  All vertex
	// positions, and normals will be extracted and stored within the MeshConsolidator
	// class.
	unique_ptr<MeshConsolidator> meshConsolidator (new MeshConsolidator{
			getAssetFilePath("cube.obj"),
			getAssetFilePath("sphere.obj"),
			getAssetFilePath("suzanne.obj")
	});


	// Acquire the BatchInfoMap from the MeshConsolidator.
	meshConsolidator->getBatchInfoMap(m_batchInfoMap);

	// Take all vertex data within the MeshConsolidator and upload it to VBOs on the GPU.
	uploadVertexDataToVbos(*meshConsolidator);

	mapVboDataToVertexShaderInputLocations();

	initPerspectiveMatrix();

	initViewMatrix();

	initLightSources();


    m_rootNodeDefaultTrans = m_rootNode->get_transform();
    bMouseFirstClick = false;
    bMousePressed = false;
    
    m_translation = mat4();
    m_orientation = mat4();
    m_radioButtonChoice = 0;
    m_currentMode = "P";
    m_bCircle = m_bPrevCircle = false;
    m_bZbuffer = m_bPrevZbuffer = true;
    m_bBackCull = m_bPrevBackCull = false;
    m_bFrontCull = m_bPrevFrontCull = false;
    m_matrixStack = stack<glm::mat4>();
    
    m_bPicking = false;
    
    m_mapChildJoint.clear();
    m_jointNodes.clear();
    
    initSelectableNodeMap(*m_rootNode);
	// Exiting the current scope calls delete automatically on meshConsolidator freeing
	// all vertex data resources.  This is fine since we already copied this data to
	// VBOs on the GPU.  We have no use for storing vertex data on the CPU side beyond
	// this point.
}


//----------------------------------------------------------------------------------------
/** Picking related functions
 */

void A3::initSelectableNodeMap(const SceneNode &node) {
    if (node.m_nodeType == NodeType::JointNode) {
        
        m_jointNodes.push_back((JointNode *)&node);
        // Init a joint angle command for each joint node
        ChangeJointAngleCommand *cmd = new ChangeJointAngleCommand((JointNode *)&node);
        m_mapJointCmd[(JointNode *)&node] = cmd;
    }
    
    if (node.m_nodeType != NodeType::JointNode && node.parent != NULL)
    {
        // If parent is a joint node, then just store it
        if (node.parent->m_nodeType == NodeType::JointNode) {
            m_mapChildJoint[node.m_nodeId] = (JointNode *)node.parent;
        } else {
            unsigned int parentId = node.parent->m_nodeId;
            if (m_mapChildJoint.count(parentId)) {
                m_mapChildJoint[node.m_nodeId] = m_mapChildJoint[parentId];
            }
        }
    }
    
    for (const SceneNode * child : node.children) {
            initSelectableNodeMap(*child);
    }
}
    

//----------------------------------------------------------------------------------------
/** Reset functions
 */
void A3::resetPosition() {
    m_rootNode->set_transform(m_orientation * m_rootNodeDefaultTrans);
    m_translation = mat4();
}

void A3::resetOrientation() {
    m_rootNode->set_transform(m_translation * m_rootNodeDefaultTrans);
    m_orientation = mat4();
}

void A3::resetJoints() {
    for (JointNode *node: m_jointNodes) {
        // clear selection
        node->isSelected = false;
        
        node->set_transform(mat4());
        node->m_joint_x.current = node->m_joint_x.init;
        node->m_joint_y.current = node->m_joint_y.init;
        
        // clear undo/redo stack to initial state
        ChangeJointAngleCommand* cmd =  m_mapJointCmd[node];
        cmd->reset();
    }
    
}

void A3::resetAll() {
    resetPosition();
    resetOrientation();
    resetJoints();
}
//----------------------------------------------------------------------------------------
/** Undo/Redo functions
 */
void A3::undo()
{
    for(JointNode* node: m_jointNodes) {
        m_mapJointCmd[node]->undo();
    }
}

void A3::redo()
{
    for(JointNode* node: m_jointNodes) {
        m_mapJointCmd[node]->redo();
    }
}

//----------------------------------------------------------------------------------------
void A3::processLuaSceneFile(const std::string & filename) {
	// This version of the code treats the Lua file as an Asset,
	// so that you'd launch the program with just the filename
	// of a puppet in the Assets/ directory.
	// std::string assetFilePath = getAssetFilePath(filename.c_str());
	// m_rootNode = std::shared_ptr<SceneNode>(import_lua(assetFilePath));

	// This version of the code treats the main program argument
	// as a straightforward pathname.
	m_rootNode = std::shared_ptr<SceneNode>(import_lua(filename));
	if (!m_rootNode) {
		std::cerr << "Could not open " << filename << std::endl;
	}
}

//----------------------------------------------------------------------------------------
void A3::createShaderProgram()
{
	m_shader.generateProgramObject();
	m_shader.attachVertexShader( getAssetFilePath("VertexShader.vs").c_str() );
	m_shader.attachFragmentShader( getAssetFilePath("FragmentShader.fs").c_str() );
	m_shader.link();

	m_shader_arcCircle.generateProgramObject();
	m_shader_arcCircle.attachVertexShader( getAssetFilePath("arc_VertexShader.vs").c_str() );
	m_shader_arcCircle.attachFragmentShader( getAssetFilePath("arc_FragmentShader.fs").c_str() );
	m_shader_arcCircle.link();
}

//----------------------------------------------------------------------------------------
void A3::enableVertexShaderInputSlots()
{
	//-- Enable input slots for m_vao_meshData:
	{
		glBindVertexArray(m_vao_meshData);

		// Enable the vertex shader attribute location for "position" when rendering.
		m_positionAttribLocation = m_shader.getAttribLocation("position");
		glEnableVertexAttribArray(m_positionAttribLocation);

		// Enable the vertex shader attribute location for "normal" when rendering.
		m_normalAttribLocation = m_shader.getAttribLocation("normal");
		glEnableVertexAttribArray(m_normalAttribLocation);

		CHECK_GL_ERRORS;
	}


	//-- Enable input slots for m_vao_arcCircle:
	{
		glBindVertexArray(m_vao_arcCircle);

		// Enable the vertex shader attribute location for "position" when rendering.
		m_arc_positionAttribLocation = m_shader_arcCircle.getAttribLocation("position");
		glEnableVertexAttribArray(m_arc_positionAttribLocation);

		CHECK_GL_ERRORS;
	}

	// Restore defaults
	glBindVertexArray(0);
}

//----------------------------------------------------------------------------------------
void A3::uploadVertexDataToVbos (
		const MeshConsolidator & meshConsolidator
) {
	// Generate VBO to store all vertex position data
	{
		glGenBuffers(1, &m_vbo_vertexPositions);

		glBindBuffer(GL_ARRAY_BUFFER, m_vbo_vertexPositions);

		glBufferData(GL_ARRAY_BUFFER, meshConsolidator.getNumVertexPositionBytes(),
				meshConsolidator.getVertexPositionDataPtr(), GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		CHECK_GL_ERRORS;
	}

	// Generate VBO to store all vertex normal data
	{
		glGenBuffers(1, &m_vbo_vertexNormals);

		glBindBuffer(GL_ARRAY_BUFFER, m_vbo_vertexNormals);

		glBufferData(GL_ARRAY_BUFFER, meshConsolidator.getNumVertexNormalBytes(),
				meshConsolidator.getVertexNormalDataPtr(), GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		CHECK_GL_ERRORS;
	}

	// Generate VBO to store the trackball circle.
	{
		glGenBuffers( 1, &m_vbo_arcCircle );
		glBindBuffer( GL_ARRAY_BUFFER, m_vbo_arcCircle );

		float *pts = new float[ 2 * CIRCLE_PTS ];
		for( size_t idx = 0; idx < CIRCLE_PTS; ++idx ) {
			float ang = 2.0 * M_PI * float(idx) / CIRCLE_PTS;
			pts[2*idx] = cos( ang );
			pts[2*idx+1] = sin( ang );
		}

		glBufferData(GL_ARRAY_BUFFER, 2*CIRCLE_PTS*sizeof(float), pts, GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		CHECK_GL_ERRORS;
	}
}

//----------------------------------------------------------------------------------------
void A3::mapVboDataToVertexShaderInputLocations()
{
	// Bind VAO in order to record the data mapping.
	glBindVertexArray(m_vao_meshData);

	// Tell GL how to map data from the vertex buffer "m_vbo_vertexPositions" into the
	// "position" vertex attribute location for any bound vertex shader program.
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo_vertexPositions);
	glVertexAttribPointer(m_positionAttribLocation, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

	// Tell GL how to map data from the vertex buffer "m_vbo_vertexNormals" into the
	// "normal" vertex attribute location for any bound vertex shader program.
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo_vertexNormals);
	glVertexAttribPointer(m_normalAttribLocation, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

	//-- Unbind target, and restore default values:
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	CHECK_GL_ERRORS;

	// Bind VAO in order to record the data mapping.
	glBindVertexArray(m_vao_arcCircle);

	// Tell GL how to map data from the vertex buffer "m_vbo_arcCircle" into the
	// "position" vertex attribute location for any bound vertex shader program.
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo_arcCircle);
	glVertexAttribPointer(m_arc_positionAttribLocation, 2, GL_FLOAT, GL_FALSE, 0, nullptr);

	//-- Unbind target, and restore default values:
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	CHECK_GL_ERRORS;
}

//----------------------------------------------------------------------------------------
void A3::initPerspectiveMatrix()
{
	float aspect = ((float)m_windowWidth) / m_windowHeight;
	m_perpsective = glm::perspective(degreesToRadians(60.0f), aspect, 0.1f, 100.0f);
}


//----------------------------------------------------------------------------------------
void A3::initViewMatrix() {
	m_view = glm::lookAt(vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 0.0f, -1.0f),
			vec3(0.0f, 1.0f, 0.0f));
}

//----------------------------------------------------------------------------------------
void A3::initLightSources() {
    // light pos is at eye pos
    
    m_light.position = vec3(m_view[0].w, m_view[1].w, m_view[2].w);
	m_light.rgbIntensity = vec3(0.8f); // White light
}

//----------------------------------------------------------------------------------------
void A3::uploadCommonSceneUniforms() {
	m_shader.enable();
	{
		//-- Set Perpsective matrix uniform for the scene:
		GLint location = m_shader.getUniformLocation("Perspective");
		glUniformMatrix4fv(location, 1, GL_FALSE, value_ptr(m_perpsective));
		CHECK_GL_ERRORS;

        location = m_shader.getUniformLocation("picking");
        glUniform1i( location, m_bPicking ? 1 : 0 );
        
        if( !m_bPicking ) {
        
            //-- Set LightSource uniform for the scene:
            {
                location = m_shader.getUniformLocation("light.position");
                glUniform3fv(location, 1, value_ptr(m_light.position));
                location = m_shader.getUniformLocation("light.rgbIntensity");
                glUniform3fv(location, 1, value_ptr(m_light.rgbIntensity));
                CHECK_GL_ERRORS;
            }

            //-- Set background light ambient intensity
            {
                location = m_shader.getUniformLocation("ambientIntensity");
                vec3 ambientIntensity(0.05f);
                glUniform3fv(location, 1, value_ptr(ambientIntensity));
                CHECK_GL_ERRORS;
            }
        }
	}
	m_shader.disable();
}

//----------------------------------------------------------------------------------------
/*
 * Called once per frame, before guiLogic().
 */
void A3::appLogic()
{
	// Place per frame, application logic here ...

	uploadCommonSceneUniforms();
}

//----------------------------------------------------------------------------------------
/*
 * Called once per frame, after appLogic(), but before the draw() method.
 */
void A3::guiLogic()
{
	if( !show_gui ) {
		return;
	}

	static bool firstRun(true);
	if (firstRun) {
		ImGui::SetNextWindowPos(ImVec2(50, 50));
		firstRun = false;
	}

	static bool showDebugWindow(true);
	ImGuiWindowFlags windowFlags(ImGuiWindowFlags_MenuBar);
    
	float opacity(0.5f);
    

	ImGui::Begin("Properties", &showDebugWindow, ImVec2(100,100), opacity,
			windowFlags);


		// Add more gui elements here here ...
        if( ImGui::BeginMenuBar() ) {
        
            if ( ImGui::BeginMenu("Application") ) {
                
                // Reset Position
                if( ImGui::MenuItem( "Reset Position", "I" ) ) {
                    resetPosition();
                }
                
                // Reset Orientation
                if( ImGui::MenuItem( "Reset Orientation", "O" ) ) {
                    resetOrientation();
                }
                
                // Reset Joints
                if( ImGui::MenuItem( "Reset Joints", "N" ) ) {
                    resetJoints();
                }
                
                // Reset All
                if( ImGui::MenuItem( "Reset All", "A" ) ) {
                    resetAll();
                }

                // Quit Application
                if( ImGui::MenuItem( "Quit Application", "Q" ) ) {
                    glfwSetWindowShouldClose(m_window, GL_TRUE);
                }
                
                ImGui::EndMenu();
            }
            
            if ( ImGui::BeginMenu("Edit") ) {
                // Undo
                if( ImGui::MenuItem( "Undo", "U" ) ) {
                    undo();
                }
                
                // Redo
                if( ImGui::MenuItem( "Redo", "R" ) ) {
                    redo();
                }
                
                ImGui::EndMenu();
            }
            if ( ImGui::BeginMenu("Options") ) {
                // Trackball Circle
               
                if( ImGui::Checkbox("Circle (C)", &m_bCircle) ) {
                    setOption("C");
                }
                
                
                // Z-buffer
                if( ImGui::Checkbox( "Z-buffer (Z)", &m_bZbuffer ) ) {
                    setOption("Z");
                }
                
                // Backface culling
                if( ImGui::Checkbox( "Backface culling (B)", &m_bBackCull ) ) {
                    setOption("B");
                }
                
                // Frontface culling
                if( ImGui::Checkbox( "Frontface culling (F)", &m_bFrontCull ) ) {
                   setOption("F");
                }
                
                ImGui::EndMenu();
            }
            
            ImGui::EndMenuBar();
        }
       
        // Radio Buttons for selecting modes
        ImGui::PushID( 0 );
        if( ImGui::RadioButton( "Position/Orientation (P)", &m_radioButtonChoice, 0 ) ) {
            m_currentMode = "P";
            // clear selection
            for (JointNode *node: m_jointNodes) {
                node->isSelected = false;
            }
        }
        ImGui::PopID();
    
        ImGui::PushID( 1 );
        if( ImGui::RadioButton( "Joints (J)", &m_radioButtonChoice, 1 ) ) {
            m_currentMode = "J";
        }
        ImGui::PopID();
    
    
		ImGui::Text( "Framerate: %.1f FPS", ImGui::GetIO().Framerate );

	ImGui::End();
}

//----------------------------------------------------------------------------------------
/*
 * Set option
 */

void A3::setOption(const std::string &option)
{
    if (option == "C") {
        if (m_bPrevCircle)
            m_bCircle = m_bPrevCircle =  false;
        else
            m_bCircle = m_bPrevCircle = true;
    } else if (option == "Z") {
        if (m_bPrevZbuffer)
            m_bZbuffer = m_bPrevZbuffer = false;
        else
            m_bZbuffer = m_bPrevZbuffer = true;
    } else if (option == "B") {
        if (m_bPrevBackCull) {
            m_bBackCull = m_bPrevBackCull = false;
            if (m_bFrontCull)
                glCullFace( GL_FRONT );
        }
        else {
            m_bBackCull = m_bPrevBackCull = true;
            if (m_bFrontCull)
                glCullFace( GL_FRONT_AND_BACK );
            else
                glCullFace(GL_BACK);
        }
    } else {
        // F
        if (m_bPrevFrontCull) {
            m_bFrontCull = m_bPrevFrontCull = false;
            if (m_bBackCull)
                glCullFace( GL_BACK );
        }
        else {
            m_bFrontCull = m_bPrevFrontCull = true;
            if (m_bBackCull)
                glCullFace( GL_FRONT_AND_BACK );
            else
                glCullFace( GL_FRONT );
        }
    }
}

//----------------------------------------------------------------------------------------
// Update mesh specific shader uniforms:
static void updateShaderUniforms(
		const ShaderProgram & shader,
		const GeometryNode & node,
		const glm::mat4 & viewMatrix,
        const glm::mat4 & transMatrix,
        unsigned int idx,
        bool do_picking,
        bool selected
) {

	shader.enable();
	{
		//-- Set ModelView matrix:
		GLint location = shader.getUniformLocation("ModelView");
		mat4 modelView = viewMatrix * transMatrix;
		glUniformMatrix4fv(location, 1, GL_FALSE, value_ptr(modelView));
		CHECK_GL_ERRORS;
        
        if( do_picking ) {
            // this code is credit from the Picking_Example Directory
            float r = float(idx&0xff) / 255.0f;
            float g = float((idx>>8)&0xff) / 255.0f;
            float b = float((idx>>16)&0xff) / 255.0f;
            
            location = shader.getUniformLocation("material.kd");
            glUniform3f( location, r, g, b );
            CHECK_GL_ERRORS;
        } else {

            //-- Set NormMatrix:
            location = shader.getUniformLocation("NormalMatrix");
            mat3 normalMatrix = glm::transpose(glm::inverse(mat3(modelView)));
            glUniformMatrix3fv(location, 1, GL_FALSE, value_ptr(normalMatrix));
            CHECK_GL_ERRORS;


            //-- Set Material values:
            location = shader.getUniformLocation("material.kd");
            vec3 kd = node.material.kd;
            
            if (selected) {
                kd = vec3(1.0f, 1.0f, 0.0f);
            }
            
            glUniform3fv(location, 1, value_ptr(kd));
            CHECK_GL_ERRORS;
            location = shader.getUniformLocation("material.ks");
            vec3 ks = node.material.ks;
            glUniform3fv(location, 1, value_ptr(ks));
            CHECK_GL_ERRORS;
            location = shader.getUniformLocation("material.shininess");
            glUniform1f(location, node.material.shininess);
            CHECK_GL_ERRORS;
        }

	}
	shader.disable();

}

//----------------------------------------------------------------------------------------
/*
 * Called once per frame, after guiLogic().
 */
void A3::draw() {

    if (m_bZbuffer)
        glEnable( GL_DEPTH_TEST );
    
    if (m_bBackCull || m_bFrontCull) {
        glEnable( GL_CULL_FACE);
    }
    
	traverseRenderGeometry(*m_rootNode);
    
    glDisable( GL_CULL_FACE);
    
    if (m_bZbuffer)
        glDisable( GL_DEPTH_TEST );
 
    if (m_bCircle) {
        renderArcCircle();
    }
}

//----------------------------------------------------------------------------------------
void A3::traverseRenderGeometry(const SceneNode &node) {

    if (m_matrixStack.empty()) {
        /* Empty stack, then just push the transformation to the stack */
        m_matrixStack.push(node.get_transform());
    } else {
        // Push the duplicate of top to the stack
        // Multiply Transform and replace the top
        m_matrixStack.push(m_matrixStack.top() * node.get_transform());
    }
    
    // If geometry node, just draw
    if (node.m_nodeType == NodeType::GeometryNode) {
        // draw
        renderGeometryNode(node);
    }
    
    // Traverse its children
    for (const SceneNode * child : node.children) {
        if (child->m_nodeType == NodeType::GeometryNode) {
            traverseRenderGeometry(*child);
        }
        else if (child->m_nodeType == NodeType::JointNode) {
            traverseRenderJoint(*child);
        }
    }
    
    // Pop the transformation matrix
    m_matrixStack.pop();
}

void A3::traverseRenderJoint(const SceneNode &node) {
    if (m_matrixStack.empty()) {
        /* Empty stack, then just push the transformation to the stack */
        m_matrixStack.push(node.get_transform());
    } else {
        // Push the duplicate of top to the stack
        // Multiply Transform and replace the top
        m_matrixStack.push(m_matrixStack.top() * node.get_transform());
    }
    
    // Traverse its children
    for (const SceneNode * child : node.children) {
        if (child->m_nodeType == NodeType::GeometryNode) {
            traverseRenderGeometry(*child);
        }
        else if (child->m_nodeType == NodeType::JointNode) {
            traverseRenderJoint(*child);
        }
    }
    // Pop the transformation matrix
    m_matrixStack.pop();
}


void A3::renderGeometryNode(const SceneNode & node) {

	// Bind the VAO once here, and reuse for all GeometryNode rendering below.
	glBindVertexArray(m_vao_meshData);

        const GeometryNode * geometryNode = static_cast<const GeometryNode *>(&node);
    
        bool selected = false;
        unsigned int nodeId = node.m_nodeId;
        if( m_mapChildJoint.count(nodeId) && m_mapChildJoint[nodeId]->isSelected) {
            selected = true;
        }
    
        updateShaderUniforms(m_shader, *geometryNode, m_view, m_matrixStack.top(), nodeId, m_bPicking, selected);
    
    
        // Get the BatchInfo corresponding to the GeometryNode's unique MeshId.
        BatchInfo batchInfo = m_batchInfoMap[geometryNode->meshId];
    
        //-- Now render the mesh:
        m_shader.enable();
        glDrawArrays(GL_TRIANGLES, batchInfo.startIndex, batchInfo.numIndices);
        m_shader.disable();
    

	glBindVertexArray(0);
	CHECK_GL_ERRORS;
}

//----------------------------------------------------------------------------------------
// Draw the trackball circle.
void A3::renderArcCircle() {
	glBindVertexArray(m_vao_arcCircle);

	m_shader_arcCircle.enable();
		GLint m_location = m_shader_arcCircle.getUniformLocation( "M" );
		float aspect = float(m_framebufferWidth)/float(m_framebufferHeight);
		glm::mat4 M;
		if( aspect > 1.0 ) {
			M = glm::scale( glm::mat4(), glm::vec3( 0.5/aspect, 0.5, 1.0 ) );
		} else {
			M = glm::scale( glm::mat4(), glm::vec3( 0.5, 0.5*aspect, 1.0 ) );
		}
    
		glUniformMatrix4fv( m_location, 1, GL_FALSE, value_ptr( M ) );
		glDrawArrays( GL_LINE_LOOP, 0, CIRCLE_PTS );
	m_shader_arcCircle.disable();

	glBindVertexArray(0);
	CHECK_GL_ERRORS;
}

//----------------------------------------------------------------------------------------
/*
 * Called once, after program is signaled to terminate.
 */
void A3::cleanup()
{

}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles cursor entering the window area events.
 */
bool A3::cursorEnterWindowEvent (
		int entered
) {
	bool eventHandled(false);

	// Fill in with event handling code...

	return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles mouse cursor movement events.
 */
bool A3::mouseMoveEvent (
		double xPos,
		double yPos
) {
	bool eventHandled(false);

    if (!ImGui::IsMouseHoveringAnyWindow()) {
        
        // Check Dragging
        if (bMousePressed) {
            double xDiff = xPos - prevMouseX;
            double yDiff = prevMouseY - yPos; // mouse yPos is opposite with the coordinate
            
            if (ImGui::IsMouseDown(GLFW_MOUSE_BUTTON_LEFT))
            {
                // x, y translation
                if (m_currentMode == "P") {
                    vec3 transl = vec3(xDiff*0.01f, yDiff*0.01f, 0.0f);
                    m_rootNode->translate(transl);
                    m_translation = glm::translate(m_translation, transl);
                    eventHandled = true;
                }
            }
            if (ImGui::IsMouseDown(GLFW_MOUSE_BUTTON_MIDDLE))
            {
                // z translation
                if (m_currentMode == "P") {
                    vec3 transl = vec3(0.0f, 0.0f, -yDiff*0.01f);
                    m_rootNode->translate(transl);
                    m_translation = glm::translate(m_translation, transl);
                    eventHandled = true;
                }
                
                // change joint angle for all
                if (m_currentMode == "J") {
                    
                    for (JointNode *jointNode: m_jointNodes) {
                        if (jointNode->isSelected) {
                            float angleX = 0.3f * yDiff;
                            float angleY = 0.3f * yDiff;
                            
                            m_mapJointCmd[jointNode]->rotateJoint(angleX, angleY);
                        }
                        
                    }
                }
            }
            if (ImGui::IsMouseDown(GLFW_MOUSE_BUTTON_RIGHT))
            {
                // trackball rotation
                if (m_currentMode == "P") {
                    
                    float fDiameter;
                    int iCenterX, iCenterY;
                    float fNewModX, fNewModY, fOldModX, fOldModY;
                    
                    fDiameter = (m_windowWidth < m_windowHeight) ? m_windowWidth * 0.5 : m_windowHeight * 0.5;
                    iCenterX = m_windowWidth / 2;
                    iCenterY = m_windowHeight / 2;
                    fOldModX = prevMouseX - iCenterX;
                    fOldModY = prevMouseY - iCenterY;
                    fNewModX = xPos - iCenterX;
                    fNewModY = yPos - iCenterY;
                    
                    float  fRotVecX, fRotVecY, fRotVecZ;
                    vCalcRotVec(fNewModX, fNewModY,
                                fOldModX, fOldModY,
                                fDiameter,
                                &fRotVecX, &fRotVecY, &fRotVecZ);
                    
                    /* Negate Y component since Y axis increases downwards
                     * in screen space and upwards in OpenGL.
                     */
                    
                    mat4 tmp = m_rootNode->get_transform();
                    mat4 rotatedMatrix = tmp;
                    rotatedMatrix = glm::rotate(rotatedMatrix, fRotVecX, vec3(1.0f, 0.0f, 0.0f));
                    rotatedMatrix = glm::rotate(rotatedMatrix, -fRotVecY, vec3(0.0f, 1.0f, 0.0f));
                    rotatedMatrix = glm::rotate(rotatedMatrix, fRotVecZ, vec3(0.0f, 0.0f, 1.0f));
                    m_rootNode->set_transform(rotatedMatrix);
                    
                    mat4 rotateMatrix = rotatedMatrix * inverse(tmp);
                    m_orientation = rotateMatrix * m_orientation;
                    
                    
                    eventHandled = true;
                }
                // change joint angle for only head about y axis (left - right)
                if (m_currentMode == "J") {
                    for (JointNode *jointNode: m_jointNodes) {
                        if (jointNode->m_name == "headJoint" && jointNode->isSelected) {
                            float angleX = 0.0f;
                            float angleY = 0.3f * yDiff;
                            
                            m_mapJointCmd[jointNode]->rotateJoint(angleX, angleY);
                            break;
                        }
                    }
                }
            }
            prevMouseX = xPos;
            prevMouseY = yPos;
        }
    }

	return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles mouse button events.
 */
bool A3::mouseButtonInputEvent (
		int button,
		int actions,
		int mods
) {
	bool eventHandled(false);

    if (!ImGui::IsMouseHoveringAnyWindow()) {
        if (actions == GLFW_PRESS) {
            if (!bMouseFirstClick) {
                double mouse_x;
                double mouse_y;
                glfwGetCursorPos(m_window, &mouse_x, &mouse_y);
                prevMouseX = mouse_x;
                prevMouseY = mouse_y;
                
                bMouseFirstClick = true;
            }
            bMousePressed = true;
            
            if (m_currentMode == "J" && button == GLFW_MOUSE_BUTTON_LEFT) {
                // this piece of codes credit from the Picking_Example Directory
                double xpos, ypos;
                glfwGetCursorPos( m_window, &xpos, &ypos );
                
                m_bPicking = true;
                
                uploadCommonSceneUniforms();
                glClearColor(1.0, 1.0, 1.0, 1.0 );
                glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
                glClearColor(0.35, 0.35, 0.35, 1.0);
                
                draw();
                
                CHECK_GL_ERRORS;
                
                xpos *= double(m_framebufferWidth) / double(m_windowWidth);
                
                ypos = m_windowHeight - ypos;
                ypos *= double(m_framebufferHeight) / double(m_windowHeight);
                
                GLubyte buffer[ 4 ] = { 0, 0, 0, 0 };
                // A bit ugly -- don't want to swap the just-drawn false colours
                // to the screen, so read from the back buffer.
                glReadBuffer( GL_BACK );
                // Actually read the pixel at the mouse location.
                glReadPixels( int(xpos), int(ypos), 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, buffer );
                CHECK_GL_ERRORS;
                
                // Reassemble the object ID.
                unsigned int objID = buffer[0] + (buffer[1] << 8) + (buffer[2] << 16);
                
                // Check whether selectable
                if( m_mapChildJoint.count(objID) ) {
                    m_mapChildJoint[objID]->isSelected = !m_mapChildJoint[objID]->isSelected;
                }
                
                m_bPicking = false;
                
                CHECK_GL_ERRORS;
            }
        
            
            eventHandled = true;
        }
        
        if (actions == GLFW_RELEASE) {
            bMousePressed = false;
            bMouseFirstClick = false;
            
            if (m_currentMode == "J" && button == GLFW_MOUSE_BUTTON_MIDDLE) {
                for (JointNode *jointNode: m_jointNodes) {
                    if (jointNode->isSelected) {
                        // update undo/redo stack
			cout << jointNode->m_name << " update" << endl;
                        m_mapJointCmd[jointNode]->updateStack();
                    }
                }
            }
            
            if (m_currentMode == "J" && button == GLFW_MOUSE_BUTTON_RIGHT) {
                std::vector<ChangeJointAngleCommand *> currentCommands;
                for (JointNode *jointNode: m_jointNodes) {
                    if (jointNode->m_name == "headJoint" && jointNode->isSelected) {
                        // update undo/redo stack
                        m_mapJointCmd[jointNode]->updateStack();
                        break;
                    }
                }
            }
            
            eventHandled = true;
        }
    }

	return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles mouse scroll wheel events.
 */
bool A3::mouseScrollEvent (
		double xOffSet,
		double yOffSet
) {
	bool eventHandled(false);

	// Fill in with event handling code...

	return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles window resize events.
 */
bool A3::windowResizeEvent (
		int width,
		int height
) {
	bool eventHandled(false);
	initPerspectiveMatrix();
	return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles key input events.
 */
bool A3::keyInputEvent (
		int key,
		int action,
		int mods
) {
	bool eventHandled(false);

	if( action == GLFW_PRESS ) {
		if( key == GLFW_KEY_M ) {
            cout << "M key pressed" << endl;
			show_gui = !show_gui;
			eventHandled = true;
		}
        
        if (key == GLFW_KEY_Q) {
            cout << "Q key pressed" << endl;
            // Quit Application
            glfwSetWindowShouldClose(m_window, GL_TRUE);
            eventHandled = true;
        }
        
        if (key == GLFW_KEY_I) {
            cout << "I key pressed" << endl;
            // Reset Position
            resetPosition();
            eventHandled = true;
        }
        if (key == GLFW_KEY_O) {
            cout << "O key pressed" << endl;
            // Reset Orientation
            resetOrientation();
            eventHandled = true;
        }
        if (key == GLFW_KEY_N) {
            cout << "N key pressed" << endl;
            // Reset Joints
            resetJoints();
            eventHandled = true;
        }
        if (key == GLFW_KEY_A) {
            cout << "A key pressed" << endl;
            // Reset All
            resetAll();
            eventHandled = true;
        }
        if (key == GLFW_KEY_U) {
            cout << "U key pressed" << endl;
            // Undo
            undo();
            eventHandled = true;
        }
        if (key == GLFW_KEY_R) {
            cout << "R key pressed" << endl;
            // Redo
            redo();
            eventHandled = true;
        }
        if (key == GLFW_KEY_C) {
            cout << "C key pressed" << endl;
            // Circle
            setOption("C");
            eventHandled = true;
        }
        if (key == GLFW_KEY_Z) {
            cout << "Z key pressed" << endl;
            // Z-buffer
            setOption("Z");
            eventHandled = true;
        }
        if (key == GLFW_KEY_B) {
            cout << "B key pressed" << endl;
            // Backface culling
            setOption("B");
            eventHandled = true;
        }
        if (key == GLFW_KEY_F) {
            cout << "F key pressed" << endl;
            // Frontface culling
            setOption("F");
            eventHandled = true;
        }
        if (key == GLFW_KEY_P) {
            cout << "P key pressed" << endl;
            // Position/Orientation Mode
            m_radioButtonChoice = 0;
            m_currentMode = "P";
            
            // clear selection
            for (JointNode *node: m_jointNodes) {
                node->isSelected = false;
            }
            eventHandled = true;
        }
        if (key == GLFW_KEY_J) {
            cout << "J key pressed" << endl;
            // Joints Mode
            m_radioButtonChoice = 1;
            m_currentMode = "J";
            eventHandled = true;
        }
        
        
	}

	return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Trackball functions credit from /u/gr/cs488/demo/trackball
 */


void A3::vCalcRotVec(float fNewX, float fNewY,
                 float fOldX, float fOldY,
                 float fDiameter,
                 float *fVecX, float *fVecY, float *fVecZ) {
    float fNewVecX, fNewVecY, fNewVecZ,        /* Vector corresponding to new mouse location */
    fOldVecX, fOldVecY, fOldVecZ,        /* Vector corresponding to old mouse location */
    fLength;
    
    /* Vector pointing from center of virtual trackball to
     * new mouse position
     */
    fNewVecX    = fNewX * 2.0 / fDiameter;
    fNewVecY    = fNewY * 2.0 / fDiameter;
    fNewVecZ    = (1.0 - fNewVecX * fNewVecX - fNewVecY * fNewVecY);
    
    /* If the Z component is less than 0, the mouse point
     * falls outside of the trackball which is interpreted
     * as rotation about the Z axis.
     */
    if (fNewVecZ < 0.0) {
        fLength = sqrt(1.0 - fNewVecZ);
        fNewVecZ  = 0.0;
        fNewVecX /= fLength;
        fNewVecY /= fLength;
    } else {
        fNewVecZ = sqrt(fNewVecZ);
    }
    
    /* Vector pointing from center of virtual trackball to
     * old mouse position
     */
    fOldVecX    = fOldX * 2.0 / fDiameter;
    fOldVecY    = fOldY * 2.0 / fDiameter;
    fOldVecZ    = (1.0 - fOldVecX * fOldVecX - fOldVecY * fOldVecY);
    
    /* If the Z component is less than 0, the mouse point
     * falls outside of the trackball which is interpreted
     * as rotation about the Z axis.
     */
    if (fOldVecZ < 0.0) {
        fLength = sqrt(1.0 - fOldVecZ);
        fOldVecZ  = 0.0;
        fOldVecX /= fLength;
        fOldVecY /= fLength;
    } else {
        fOldVecZ = sqrt(fOldVecZ);
    }
    
    /* Generate rotation vector by calculating cross product:
     *
     * fOldVec x fNewVec.
     *
     * The rotation vector is the axis of rotation
     * and is non-unit length since the length of a crossproduct
     * is related to the angle between fOldVec and fNewVec which we need
     * in order to perform the rotation.
     */
    *fVecX = fOldVecY * fNewVecZ - fNewVecY * fOldVecZ;
    *fVecY = fOldVecZ * fNewVecX - fNewVecZ * fOldVecX;
    *fVecZ = fOldVecX * fNewVecY - fNewVecX * fOldVecY;
}
