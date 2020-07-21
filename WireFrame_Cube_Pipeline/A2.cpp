#include "A2.hpp"
#include "cs488-framework/GlErrorCheck.hpp"

#include <iostream>
using namespace std;

#include <imgui/imgui.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/io.hpp>
using namespace glm;

//----------------------------------------------------------------------------------------
// Constructor
VertexData::VertexData()
	: numVertices(0),
	  index(0)
{
	positions.resize(kMaxVertices);
	colours.resize(kMaxVertices);
}


//----------------------------------------------------------------------------------------
// Constructor
A2::A2()
	: m_currentLineColour(vec3(0.0f))
{

}

//----------------------------------------------------------------------------------------
// Destructor
A2::~A2()
{

}

//----------------------------------------------------------------------------------------
/*
 * Called once, at program start.
 */
void A2::init()
{
	// Set the background colour.
	glClearColor(0.3, 0.3, 0.3, 1.0);

	createShaderProgram();

	glGenVertexArrays(1, &m_vao);

	enableVertexAttribIndices();

	generateVertexBuffers();

	mapVboDataToVertexAttributeLocation();
    
    reset();
}

//----------------------------------------------------------------------------------------
void A2::reset()
{
    {
        m_radioButtonChoice = 3;
        m_currentMode = "R"; // init current mode
    }
    
    {
        m_FOV = 30.0f;
        m_near = 3.0f; // init near plane
        m_far = 20.0f; // init far plane
    }
    
    {
        m_proj = makeProjMatrix(); // init a View-Projection Matrix
        /* init a World-View Matrix */
        // Eye pos is oriented towards the neg pos of z axis and 8 unit far away
        m_view = glm::mat4(vec4(1, 0, 0, 0), vec4(0, 1, 0, 0), vec4(0, 0, -1, 0), vec4(0, 0, 8, 1));
        m_model = mat4(); // init a Model-World Matrix; this case it is an indentity matrix
    }
    
    {
        m_viewportHalfWidth = 0.95f;
        m_viewportHalfHeight = 0.95f;
        m_viewportCenter = vec2(0.0f,0.0f);
    
        initViewport();
    }
    
    initGnomonsData();
    initCubeData();
}

//----------------------------------------------------------------------------------------
void A2::initGnomonsData()
{
    // model gnomon
    m_modelGnomon[0] = vec4(0.0f,0.0f,0.0f,1.0f); // x-axis
    m_modelGnomon[1] = vec4(1.0f,0.0f,0.0f,1.0f);
    m_modelGnomon[2] = vec4(0.0f,0.0f,0.0f,1.0f); // y-axis
    m_modelGnomon[3] = vec4(0.0f,1.0f,0.0f,1.0f);
    m_modelGnomon[4] = vec4(0.0f,0.0f,0.0f,1.0f); // z-axis
    m_modelGnomon[5] = vec4(0.0f,0.0f,1.0f,1.0f);

    // world gnomon
    m_worldGnomon[0] = vec4(0.0f,0.0f,0.0f,1.0f); // x-axis
    m_worldGnomon[1] = vec4(1.0f,0.0f,0.0f,1.0f);
    m_worldGnomon[2] = vec4(0.0f,0.0f,0.0f,1.0f); // y-axis
    m_worldGnomon[3] = vec4(0.0f,1.0f,0.0f,1.0f);
    m_worldGnomon[4] = vec4(0.0f,0.0f,0.0f,1.0f); // z-axis
    m_worldGnomon[5] = vec4(0.0f,0.0f,1.0f,1.0f);
}

//----------------------------------------------------------------------------------------
void A2::initCubeData()
{
    // Front 4 lines
    m_cubeVertices.push_back(vec4(-1.0f,1.0f,1.0f,1.0f));
    m_cubeVertices.push_back(vec4(-1.0f,-1.0f,1.0f,1.0f));
    
    m_cubeVertices.push_back(vec4(-1.0f,-1.0f,1.0f,1.0f));
    m_cubeVertices.push_back(vec4(1.0f,-1.0f,1.0f,1.0f));
    
    m_cubeVertices.push_back(vec4(1.0f,-1.0f,1.0f,1.0f));
    m_cubeVertices.push_back(vec4(1.0f,1.0f,1.0f,1.0f));
    
    m_cubeVertices.push_back(vec4(1.0f,1.0f,1.0f,1.0f));
    m_cubeVertices.push_back(vec4(-1.0f,1.0f,1.0f,1.0f));
    
    
    // Back 4 lines
    m_cubeVertices.push_back(vec4(-1.0f,1.0f,-1.0f,1.0f));
    m_cubeVertices.push_back(vec4(-1.0f,-1.0f,-1.0f,1.0f));
    
    m_cubeVertices.push_back(vec4(-1.0f,-1.0f,-1.0f,1.0f));
    m_cubeVertices.push_back(vec4(1.0f,-1.0f,-1.0f,1.0f));
    
    m_cubeVertices.push_back(vec4(1.0f,-1.0f,-1.0f,1.0f));
    m_cubeVertices.push_back(vec4(1.0f,1.0f,-1.0f,1.0f));
    
    m_cubeVertices.push_back(vec4(1.0f,1.0f,-1.0f,1.0f));
    m_cubeVertices.push_back(vec4(-1.0f,1.0f,-1.0f,1.0f));
    
    
    // Left 2 lines
    m_cubeVertices.push_back(vec4(-1.0f,-1.0f,1.0f,1.0f));
    m_cubeVertices.push_back(vec4(-1.0f,-1.0f,-1.0f,1.0f));
    
    m_cubeVertices.push_back(vec4(-1.0f,1.0f,-1.0f,1.0f));
    m_cubeVertices.push_back(vec4(-1.0f,1.0f,1.0f,1.0f));
    
    
    // Right 2 lines
    
    m_cubeVertices.push_back(vec4(1.0f,-1.0f,1.0f,1.0f));
    m_cubeVertices.push_back(vec4(1.0f,-1.0f,-1.0f,1.0f));
    
    m_cubeVertices.push_back(vec4(1.0f,1.0f,-1.0f,1.0f));
    m_cubeVertices.push_back(vec4(1.0f,1.0f,1.0f,1.0f));
}

//----------------------------------------------------------------------------------------
void A2::initViewport()
{
    m_viewportVertices[0] = vec2(m_viewportCenter.x - m_viewportHalfWidth, // Left line
                                 m_viewportCenter.y + m_viewportHalfHeight);
    m_viewportVertices[1] = vec2(m_viewportCenter.x - m_viewportHalfWidth,
                                 m_viewportCenter.y - m_viewportHalfHeight);
    
    m_viewportVertices[2] = vec2(m_viewportCenter.x - m_viewportHalfWidth, // Bottom line
                                 m_viewportCenter.y - m_viewportHalfHeight);
    m_viewportVertices[3] = vec2(m_viewportCenter.x + m_viewportHalfWidth,
                                 m_viewportCenter.y - m_viewportHalfHeight);
    
    
    m_viewportVertices[4] = vec2(m_viewportCenter.x + m_viewportHalfWidth, // Right line
                                 m_viewportCenter.y - m_viewportHalfHeight);
    m_viewportVertices[5] = vec2(m_viewportCenter.x + m_viewportHalfWidth,
                                 m_viewportCenter.y + m_viewportHalfHeight);
    
    m_viewportVertices[6] = vec2(m_viewportCenter.x + m_viewportHalfWidth, // Top line
                                 m_viewportCenter.y + m_viewportHalfHeight);
    m_viewportVertices[7] = vec2(m_viewportCenter.x - m_viewportHalfWidth,
                                 m_viewportCenter.y + m_viewportHalfHeight);
}


//----------------------------------------------------------------------------------------

 mat4 A2::makeProjMatrix()
{
    mat4 projMatrix = {
        vec4(1/tan(radians(m_FOV/2)), 0.0f, 0.0f, 0.0f),
        vec4(0.0f, 1/tan(radians(m_FOV/2)), 0.0f, 0.0f),
        vec4(0.0f, 0.0f, (m_far + m_near) / (m_far - m_near), 1.0f),
        vec4(0.0f, 0.0f, -2 * m_far * m_near / (m_far - m_near), 0.0f)
    };
    cout << projMatrix << endl;
    cout << projMatrix[3] << endl;
    return projMatrix;
}

//----------------------------------------------------------------------------------------
void A2::createShaderProgram()
{
	m_shader.generateProgramObject();
	m_shader.attachVertexShader( getAssetFilePath("VertexShader.vs").c_str() );
	m_shader.attachFragmentShader( getAssetFilePath("FragmentShader.fs").c_str() );
	m_shader.link();
}

//----------------------------------------------------------------------------------------
void A2::enableVertexAttribIndices()
{
	glBindVertexArray(m_vao);

	// Enable the attribute index location for "position" when rendering.
	GLint positionAttribLocation = m_shader.getAttribLocation( "position" );
	glEnableVertexAttribArray(positionAttribLocation);

	// Enable the attribute index location for "colour" when rendering.
	GLint colourAttribLocation = m_shader.getAttribLocation( "colour" );
	glEnableVertexAttribArray(colourAttribLocation);

	// Restore defaults
	glBindVertexArray(0);

	CHECK_GL_ERRORS;
}

//----------------------------------------------------------------------------------------
void A2::generateVertexBuffers()
{
	// Generate a vertex buffer to store line vertex positions
	{
		glGenBuffers(1, &m_vbo_positions);

		glBindBuffer(GL_ARRAY_BUFFER, m_vbo_positions);

		// Set to GL_DYNAMIC_DRAW because the data store will be modified frequently.
		glBufferData(GL_ARRAY_BUFFER, sizeof(vec2) * kMaxVertices, nullptr,
				GL_DYNAMIC_DRAW);


		// Unbind the target GL_ARRAY_BUFFER, now that we are finished using it.
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		CHECK_GL_ERRORS;
	}

	// Generate a vertex buffer to store line colors
	{
		glGenBuffers(1, &m_vbo_colours);

		glBindBuffer(GL_ARRAY_BUFFER, m_vbo_colours);

		// Set to GL_DYNAMIC_DRAW because the data store will be modified frequently.
		glBufferData(GL_ARRAY_BUFFER, sizeof(vec3) * kMaxVertices, nullptr,
				GL_DYNAMIC_DRAW);


		// Unbind the target GL_ARRAY_BUFFER, now that we are finished using it.
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		CHECK_GL_ERRORS;
	}
}

//----------------------------------------------------------------------------------------
void A2::mapVboDataToVertexAttributeLocation()
{
	// Bind VAO in order to record the data mapping.
	glBindVertexArray(m_vao);

	// Tell GL how to map data from the vertex buffer "m_vbo_positions" into the
	// "position" vertex attribute index for any bound shader program.
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo_positions);
	GLint positionAttribLocation = m_shader.getAttribLocation( "position" );
	glVertexAttribPointer(positionAttribLocation, 2, GL_FLOAT, GL_FALSE, 0, nullptr);

	// Tell GL how to map data from the vertex buffer "m_vbo_colours" into the
	// "colour" vertex attribute index for any bound shader program.
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo_colours);
	GLint colorAttribLocation = m_shader.getAttribLocation( "colour" );
	glVertexAttribPointer(colorAttribLocation, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

	//-- Unbind target, and restore default values:
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	CHECK_GL_ERRORS;
}

//---------------------------------------------------------------------------------------
void A2::initLineData()
{
	m_vertexData.numVertices = 0;
	m_vertexData.index = 0;
}

//---------------------------------------------------------------------------------------
void A2::setLineColour (
		const glm::vec3 & colour
) {
	m_currentLineColour = colour;
}

//---------------------------------------------------------------------------------------
void A2::drawLine(
		const glm::vec2 & v0,   // Line Start (NDC coordinate)
		const glm::vec2 & v1    // Line End (NDC coordinate)
) {

	m_vertexData.positions[m_vertexData.index] = v0;
	m_vertexData.colours[m_vertexData.index] = m_currentLineColour;
	++m_vertexData.index;
	m_vertexData.positions[m_vertexData.index] = v1;
	m_vertexData.colours[m_vertexData.index] = m_currentLineColour;
	++m_vertexData.index;

	m_vertexData.numVertices += 2;
}

//----------------------------------------------------------------------------------------
/*
 * Called once per frame, before guiLogic().
 */
void A2::appLogic()
{
	// Place per frame, application logic here ...

	// Call at the beginning of frame, before drawing lines:
	initLineData();

    /* Draw viewport */
    setLineColour(vec3(1.0f,1.0f,0.0f));
    for (int i = 0; i < 4; ++i) {
        drawLine(m_viewportVertices[2 * i], m_viewportVertices[2 * i + 1]);
    }
    
    /* Draw world gnomons */
    for (int i = 0; i < 3; ++i) {
        if (i == 0) {
             setLineColour(vec3(1.0f, 0.0f, 0.0f)); // x-axis, red color
        }
        else if (i == 1) {
            setLineColour(vec3(0.0f, 1.0f, 0.0f)); // y-axis, green color
        } else {
            setLineColour(vec3(0.0f, 0.0f, 1.0f)); // z-axis, blue color
        }
        vec4 lineV1 =  m_proj * m_view * m_worldGnomon[2*i];
        vec4 lineV2 =  m_proj * m_view * m_worldGnomon[2*i+1];
        
        
        // do liang barsky 3d clipping
        if (clipping(lineV1, lineV2)) {
            // draw
            drawInsideViewport(lineV1,lineV2);
        }
    }
    
    
    
    /* Draw model gnomons */
    for (int i = 0; i < 3; ++i) {
        if (i == 0) {
            setLineColour(vec3(1.0f, 0.0f, 0.0f)); // x-axis, red color
        }
        else if (i == 1) {
            setLineColour(vec3(0.0f, 1.0f, 0.0f)); // y-axis, green color
        } else {
            setLineColour(vec3(0.0f, 0.0f, 1.0f)); // z-axis, blue color
        }
        vec4 lineV1 = m_proj * m_view * m_model * m_modelGnomon[2*i];
        vec4 lineV2 = m_proj * m_view * m_model * m_modelGnomon[2*i+1];
        
        // do liang barsky 3d clipping
        if (clipping(lineV1, lineV2)) {
            // draw
            drawInsideViewport(lineV1,lineV2);
        }
    }


    /* Draw cube */
    for (int i = 0; i < 12; ++i) {
        setLineColour(vec3(1.0f, 0.0f, 1.0f));
        
        vec4 lineV1 = m_proj * m_view * m_model * m_cubeVertices[2*i];
        vec4 lineV2 = m_proj * m_view * m_model * m_cubeVertices[2*i+1];
    
        // do liang barsky 3d clipping
        if (clipping(lineV1, lineV2)) {
            // draw
            drawInsideViewport(lineV1,lineV2);
        }
    }
    
}

//----------------------------------------------------------------------------------------
/*
 * Clipping using Liang Barsky 3D Clipping Algorithm
 */
bool A2::clipping(glm::vec4 & v1,glm::vec4 & v2)
{
    float v1Face[6];
    float v2Face[6];
    
    v1Face[0] = v1.w + v1.x; //  x = -1 plane
    v1Face[1] = v1.w - v1.x; //  x = 1 plane
    v1Face[2] = v1.w + v1.y; //  y = -1 plane
    v1Face[3] = v1.w - v1.y; //  y = 1 plane
    v1Face[4] = v1.w + v1.z; //  z = -1 plane (near plane)
    v1Face[5] = v1.w - v1.z; //  z = 1 plane (far plane)
    
    v2Face[0] = v2.w + v2.x; //  x = -1 plane
    v2Face[1] = v2.w - v2.x; //  x = 1 plane
    v2Face[2] = v2.w + v2.y; //  y = -1 plane
    v2Face[3] = v2.w - v2.y; //  y = 1 plane
    v2Face[4] = v2.w + v2.z; //  z = -1 plane (near plane)
    v2Face[5] = v2.w - v2.z; //  z = 1 plane (far plane)
    
    if ((v1Face[0] < 0 && v2Face[0] < 0) ||
        (v1Face[1] < 0 && v2Face[1] < 0) ||
        (v1Face[2] < 0 && v2Face[2] < 0) ||
        (v1Face[3] < 0 && v2Face[3] < 0) ||
        (v1Face[4] < 0 && v2Face[4] < 0) ||
        (v1Face[5] < 0 && v2Face[5] < 0))
    {
        return false;
    }
    
    if (v1Face[0] >= 0 && v2Face[0] >= 0 &&
        v1Face[1] >= 0 && v2Face[1] >= 0 &&
        v1Face[2] >= 0 && v2Face[2] >= 0 &&
        v1Face[3] >= 0 && v2Face[3] >= 0 &&
        v1Face[4] >= 0 && v2Face[4] >= 0 &&
        v1Face[5] >= 0 && v2Face[5] >= 0)
    {
        return true;
    }
    
    float a = 0.0f;
    float a0 = 0.0f;
    float a1 = 1.0f;
    
    for (int i=0; i < 6; ++i) {
        if (v1Face[i] < 0)
        {
            a = v1Face[i] / (v1Face[i] - v2Face[i]);
            a0 = std::max(a, a0);
        }
        if (v2Face[i] < 0)
        {
            a = v1Face[i] / (v1Face[i] - v2Face[i]);
            a1 = std::min(a, a1);
        }
        if (a0 > a1)
            return false;
    }
    
    vec4 tmp = v1;
    if (a0 != 0.0f) {
        tmp = v1 + a0 * (v2 - v1);
    }
    if (a1 != 1.0f) {
        v2 = v1 + a1 * (v2 - v1);
    }
    v1 = tmp;
    
    return true;
}
//----------------------------------------------------------------------------------------

void A2::drawInsideViewport(const glm::vec4 & v1,
                          const glm::vec4 & v2)
{
    // Homogeneous coordinates
    vec2 lineV1(v1.x/v1.w, v1.y/v1.w);
    vec2 lineV2(v2.x/v2.w, v2.y/v2.w);
  
    // Xv = Sx * Xw + Tx
    // Yv = Sy * Yw + Ty
    vec2 v2d1(lineV1.x * m_viewportHalfWidth + m_viewportCenter.x, lineV1.y * m_viewportHalfHeight + m_viewportCenter.y);
    vec2 v2d2(lineV2.x * m_viewportHalfWidth + m_viewportCenter.x, lineV2.y * m_viewportHalfHeight + m_viewportCenter.y);
    
    drawLine(v2d1,v2d2);
}

//----------------------------------------------------------------------------------------
/* Transformation Matrix */

mat4 A2::rotateMatrix (const glm::vec3 & rotation)
{
    // rotate about x-axis
     mat4 xRotateMatrix = {
            vec4(1.0f,0.0f,0.0f,0.0f),
            vec4(0.0f,cos(rotation.x),sin(rotation.x),0.0f),
            vec4(0.0f,-sin(rotation.x),cos(rotation.x),0.0f),
            vec4(0.0f,0.0f,0.0f,1.0f)
     };
    
    // rotate about y-axis
    mat4 yRotateMatrix = {
            vec4(cos(rotation.y),0.0f,-sin(rotation.y),0.0f),
            vec4(0.0f,1.0f,0.0f,0.0f),
            vec4(sin(rotation.y),0.0f,cos(rotation.y),0.0f),
            vec4(0.0f,0.0f,0.0f,1.0f)
    };
         
    // rotate about z-axis
    mat4 zRotateMatrix = {
            vec4(cos(rotation.z),sin(rotation.z),0.0f,0.0f),
            vec4(-sin(rotation.z),cos(rotation.z),0.0f,0.0f),
            vec4(0.0f,0.0f,1.0f,0.0f),
            vec4(0.0f,0.0f,0.0f,1.0f)
    };
         
    return xRotateMatrix * yRotateMatrix * zRotateMatrix;
}


mat4 A2::translateMatrix(const glm::vec3 &v)
{
    mat4 translateMatrix = {
        vec4(1.0f,0.0f,0.0f,0.0f),
        vec4(0.0f,1.0f,0.0f,0.0f),
        vec4(0.0f,0.0f,1.0f,0.0f),
        vec4(v.x,v.y,v.z,1.0f),
    };
    return translateMatrix;
}


mat4 A2::scaleMatrix(const glm::vec3 &v)
{
    mat4 scaleMatrix = {
        vec4(v.x,0.0f,0.0f,0.0f),
        vec4(0.0f,v.y,0.0f,0.0f),
        vec4(0.0f,0.0f,v.z,0.0f),
        vec4(0.0f,0.0f,0.0f,1.0f),
    };
    return scaleMatrix;
}

//----------------------------------------------------------------------------------------
/*
 * Called once per frame, after appLogic(), but before the draw() method.
 */
void A2::guiLogic()
{
	static bool firstRun(true);
	if (firstRun) {
		ImGui::SetNextWindowPos(ImVec2(50, 50));
		firstRun = false;
	}

	static bool showDebugWindow(true);
	ImGuiWindowFlags windowFlags(ImGuiWindowFlags_AlwaysAutoResize);
	float opacity(0.5f);

	ImGui::Begin("Properties", &showDebugWindow, ImVec2(100,100), opacity,
			windowFlags);


		// Add more gui elements here here ...

        // Create Button, and check if it was clicked:
        if( ImGui::Button( "Reset (A)" ) ) {
            reset();
        }


		// Create Button, and check if it was clicked:
		if( ImGui::Button( "Quit Application (Q)" ) ) {
			glfwSetWindowShouldClose(m_window, GL_TRUE);
		}
    
        // Radio Buttons for selecting modes
        ImGui::PushID( 0 );
        if( ImGui::RadioButton( "Rotate View (O)", &m_radioButtonChoice, 0 ) ) {
            m_currentMode = "O";
        }
        ImGui::PopID();
    
        ImGui::PushID( 1 );
        if( ImGui::RadioButton( "Translate View (N)", &m_radioButtonChoice, 1 ) ) {
            m_currentMode = "N";
        }
        ImGui::PopID();
    
        ImGui::PushID( 2 );
        if( ImGui::RadioButton( "Perspective (P)", &m_radioButtonChoice, 2 ) ) {
            m_currentMode = "P";
        }
        ImGui::PopID();
    
        ImGui::PushID( 3 );
        if( ImGui::RadioButton( "Rotate Model (R)", &m_radioButtonChoice, 3 ) ) {
            m_currentMode = "R";
        }
        ImGui::PopID();
    
        ImGui::PushID( 4 );
        if( ImGui::RadioButton( "Translate Model (T)", &m_radioButtonChoice, 4 ) ) {
            m_currentMode = "T";
        }
        ImGui::PopID();
    
        ImGui::PushID( 5 );
        if( ImGui::RadioButton( "Scale Model (S)", &m_radioButtonChoice, 5 ) ) {
            m_currentMode = "S";
        }
        ImGui::PopID();
    
        ImGui::PushID( 6 );
        if( ImGui::RadioButton( "Viewport (V)", &m_radioButtonChoice, 6 ) ) {
            m_currentMode = "V";
        }
        ImGui::PopID();
    
        // A status line indicating (at least) the locations of the near and far planes (i.e., their distances from the camera).
        ImGui::Text( "Near Plane is %.1f from the camera", m_near);
        ImGui::Text( "Far Plane is %.1f from the camera", m_far );
    
		ImGui::Text( "Framerate: %.1f FPS", ImGui::GetIO().Framerate );

	ImGui::End();
}

//----------------------------------------------------------------------------------------
void A2::uploadVertexDataToVbos() {

	//-- Copy vertex position data into VBO, m_vbo_positions:
	{
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo_positions);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vec2) * m_vertexData.numVertices,
				m_vertexData.positions.data());
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		CHECK_GL_ERRORS;
	}

	//-- Copy vertex colour data into VBO, m_vbo_colours:
	{
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo_colours);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vec3) * m_vertexData.numVertices,
				m_vertexData.colours.data());
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		CHECK_GL_ERRORS;
	}
}

//----------------------------------------------------------------------------------------
/*
 * Called once per frame, after guiLogic().
 */
void A2::draw()
{
	uploadVertexDataToVbos();

	glBindVertexArray(m_vao);

	m_shader.enable();
		glDrawArrays(GL_LINES, 0, m_vertexData.numVertices);
	m_shader.disable();

	// Restore defaults
	glBindVertexArray(0);

	CHECK_GL_ERRORS;
}

//----------------------------------------------------------------------------------------
/*
 * Called once, after program is signaled to terminate.
 */
void A2::cleanup()
{

}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles cursor entering the window area events.
 */
bool A2::cursorEnterWindowEvent (
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
bool A2::mouseMoveEvent (
		double xPos,
		double yPos
) {
	bool eventHandled(false);

    if (!ImGui::IsMouseHoveringAnyWindow()) {
        
        // Check Dragging
        if (bMousePressed) {
            double xDiff = xPos - prevMouseX;
            
            if (ImGui::IsMouseDown(GLFW_MOUSE_BUTTON_LEFT))
            {
                if (m_currentMode == "R") {
                    vec3 cube_rotation(0.0f);
                    cube_rotation.x = 0.3 * xDiff * 3.14 / 180;
                    m_model = m_model * rotateMatrix(cube_rotation);
                } else if (m_currentMode == "T") {
                    m_model = m_model * translateMatrix(vec3(xDiff*0.03f, 0.0f, 0.0f));
                } else if (m_currentMode == "S") {
                    // shape limit [0.0f,5.0f]
                    if ((xDiff < 0 && 1 + 0.01f * xDiff >= 0.0f) ||
                        (xDiff > 0 && 1 + 0.01f * xDiff <= 5.0f)) {
                        vec3 cube_size(1.0f);
                        cube_size.x = 1+ 0.01f * xDiff;
                        m_model = m_model * scaleMatrix(cube_size);
                        m_modelGnomon[1].x /= (1+ 0.01f * xDiff);
                    }
                } else if (m_currentMode == "V") {
                    int width, height;
                    glfwGetWindowSize(m_window, &width, &height);
                    /* if mouse pos exceeds the window, change it to the boundary points  */
                    if (xPos > width)
                        xPos = width;
                    if (yPos > height)
                        yPos = height;
                    if (xPos < 0)
                        xPos = 0;
                    if (yPos < 0)
                        yPos = 0;
                    
                    // Xwl = 0
                    // Xv = Lv / Lw * Xw + Xvl
                    
                    // Ywl = 0
                    // Yv = Hv / Hw * Xw + Yvl
                    // mouse yPos axis is opposite with the viewport y axis
                    // Yv = -1 * Yv
                    double tmpX, tmpY;
                    tmpX = prevMouseX / width * 2 - 1;
                    tmpY = 1 - (prevMouseY / height * 2);
                    
                    xPos = xPos / width * 2 - 1;
                    yPos = 1 - (yPos / height * 2);

                    m_viewportCenter.x = (xPos + tmpX) * 0.5f;
                    m_viewportCenter.y = (yPos + tmpY) * 0.5f;
                    
                    m_viewportHalfWidth = abs(xPos - tmpX) / 2.0f;
                    m_viewportHalfHeight = abs(yPos - tmpY) / 2.0f;
                    initViewport();
                } else if (m_currentMode == "O") {
                    vec3 cube_rotation(0.0f);
                    cube_rotation.x = 0.3 * xDiff * 3.14 / 180;
                    m_view = (rotateMatrix(-cube_rotation)) * m_view;
                } else if (m_currentMode == "N") {
                    m_view = translateMatrix(vec3(-xDiff*0.03f, 0.0f, 0.0f)) * m_view;
                } else if (m_currentMode == "P") {
                    m_FOV += xDiff * 0.03f;
                    if (m_FOV < 5.0f) m_FOV = 5.0f;
                    if (m_FOV > 160.f) m_FOV = 160.0f;
                    m_proj = makeProjMatrix();
                }
            }
            if (ImGui::IsMouseDown(GLFW_MOUSE_BUTTON_MIDDLE)) {
                if (m_currentMode == "R") {
                    vec3 cube_rotation(0.0f);
                    cube_rotation.y = 0.3 * xDiff * 3.14 / 180;
                    m_model = m_model * rotateMatrix(cube_rotation);
                } else if (m_currentMode == "T") {
                    m_model = m_model * translateMatrix(vec3(0.0f, xDiff*0.03f, 0.0f));
                } else if (m_currentMode == "S") {
                    // shape limit [0.0f,5.0f]
                    if ((xDiff < 0 && 1 + 0.01f * xDiff >= 0.0f) ||
                        (xDiff > 0 && 1 + 0.01f * xDiff <= 5.0f)) {
                        vec3 cube_size(1.0f);
                        cube_size.y = 1+ 0.01f * xDiff;
                        m_model = m_model * scaleMatrix(cube_size);
                        m_modelGnomon[3].y /= (1+ 0.01f * xDiff);
                    }
                } else if (m_currentMode == "O") {
                    vec3 cube_rotation(0.0f);
                    cube_rotation.y = 0.3 * xDiff * 3.14 / 180;
                    m_view = rotateMatrix(-cube_rotation) * m_view;
                } else if (m_currentMode == "N") {
                    m_view = translateMatrix(vec3(0.0f, -xDiff*0.03f, 0.0f)) * m_view;
                } else if (m_currentMode == "P") {
                    m_near += xDiff * 0.03f;
                    m_proj = makeProjMatrix();
                }
            }
            if (ImGui::IsMouseDown(GLFW_MOUSE_BUTTON_RIGHT)) {
                if (m_currentMode == "R") {
                    vec3 cube_rotation(0.0f);
                    cube_rotation.z = 0.3 * xDiff * 3.14 / 180;
                    m_model = m_model * rotateMatrix(cube_rotation);
                } else if (m_currentMode == "T") {
                    m_model = m_model * translateMatrix(vec3(0.0f, 0.0f, xDiff*0.03f));
                } else if (m_currentMode == "S") {
                    // shape limit [0.0f,5.0f]
                    if ((xDiff < 0 && 1 + 0.01f * xDiff >= 0.0f) ||
                        (xDiff > 0 && 1 + 0.01f * xDiff <= 5.0f)) {
                        vec3 cube_size(1.0f);
                        cube_size.z = 1+ 0.01f * xDiff;
                        m_model = m_model * scaleMatrix(cube_size);
                        m_modelGnomon[5].z /= (1+ 0.01f * xDiff);
                    }
                } else if (m_currentMode == "O") {
                    vec3 cube_rotation(0.0f);
                    cube_rotation.z = 0.3 * xDiff * 3.14 / 180;
                    m_view = rotateMatrix(-cube_rotation) * m_view;
                } else if (m_currentMode == "N") {
                    m_view = translateMatrix(vec3(0.0f, 0.0f, -xDiff*0.03f)) * m_view;
                } else if (m_currentMode == "P") {
                    m_far += xDiff * 0.03f;
                    m_proj = makeProjMatrix();
                }
            }
            if (m_currentMode != "V") {
                // Not update the previous mouse position if in viewport mode
                prevMouseX = xPos;
                prevMouseY = yPos;
            }
            eventHandled = true;
        }
    }

	return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles mouse button events.
 */
bool A2::mouseButtonInputEvent (
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
            
            eventHandled = true;
        }
        
        if (actions == GLFW_RELEASE) {
            bMousePressed = false;
            bMouseFirstClick = false;
    
            eventHandled = true;
        }
    }

	return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles mouse scroll wheel events.
 */
bool A2::mouseScrollEvent (
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
bool A2::windowResizeEvent (
		int width,
		int height
) {
	bool eventHandled(false);

	// Fill in with event handling code...

	return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles key input events.
 */
bool A2::keyInputEvent (
		int key,
		int action,
		int mods
) {
	bool eventHandled(false);
    if (action == GLFW_PRESS) {
        if (key == GLFW_KEY_Q) {
            cout << "Q key pressed" << endl;
            // Quit Application
            glfwSetWindowShouldClose(m_window, GL_TRUE);
            eventHandled = true;
        }
        if (key == GLFW_KEY_A) {
            cout << "A key pressed" << endl;
            reset();
            eventHandled = true;
        }
        if (key == GLFW_KEY_R) {
            cout << "R key pressed" << endl;
            m_currentMode = "R";
            m_radioButtonChoice = 3;
            eventHandled = true;
        }
        if (key == GLFW_KEY_T) {
            cout << "T key pressed" << endl;
            m_currentMode = "T";
            m_radioButtonChoice = 4;
            eventHandled = true;
        }
        if (key == GLFW_KEY_S) {
            cout << "S key pressed" << endl;
            m_currentMode = "S";
            m_radioButtonChoice = 5;
            eventHandled = true;
        }
        if (key == GLFW_KEY_V) {
            cout << "V key pressed" << endl;
            m_currentMode = "V";
            m_radioButtonChoice = 6;
            eventHandled = true;
        }
        if (key == GLFW_KEY_O) {
            cout << "O key pressed" << endl;
            m_currentMode = "O";
            m_radioButtonChoice = 0;
            eventHandled = true;
        }
        if (key == GLFW_KEY_N) {
            cout << "N key pressed" << endl;
            m_currentMode = "N";
            m_radioButtonChoice = 1;
            eventHandled = true;
        }
        if (key == GLFW_KEY_P) {
            cout << "P key pressed" << endl;
            m_currentMode = "P";
            m_radioButtonChoice = 2;
            eventHandled = true;
        }

    }

	return eventHandled;
}
