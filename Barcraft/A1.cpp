#include "A1.hpp"
#include "cs488-framework/GlErrorCheck.hpp"

#include <iostream>

#include <imgui/imgui.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
using namespace glm;
using namespace std;

static const size_t DIM = 16;

//----------------------------------------------------------------------------------------
// Constructor
A1::A1()
	: current_col( 0 ),
    bShiftKeyPressed(false),
    bMouseFirstClick(false),
    bMousePressed(false),
    m_shape_rotation(0.0f),
    m_shape_size(1.0f)
{
	colour[0] = 0.0f;
	colour[1] = 0.0f;
	colour[2] = 0.0f;
    
    colour1[0] = 0.0f;
    colour1[1] = 0.0f;
    colour1[2] = 0.0f;
    
    colour2[0] = 0.0f;
    colour2[1] = 0.0f;
    colour2[2] = 0.0f;
    
    colour3[0] = 0.0f;
    colour3[1] = 0.0f;
    colour3[2] = 0.0f;
    
    colour4[0] = 0.0f;
    colour4[1] = 0.0f;
    colour4[2] = 0.0f;
    
    colour5[0] = 0.0f;
    colour5[1] = 0.0f;
    colour5[2] = 0.0f;
    
    colour6[0] = 0.0f;
    colour6[1] = 0.0f;
    colour6[2] = 0.0f;
    
    colour7[0] = 0.0f;
    colour7[1] = 0.0f;
    colour7[2] = 0.0f;
}

//----------------------------------------------------------------------------------------
// Destructor
A1::~A1()
{}

//----------------------------------------------------------------------------------------
/*
 * Called once, at program start.
 */
void A1::init()
{
	// Set the background colour.
	glClearColor( 0.3, 0.5, 0.7, 1.0 );

	// Build the shader
	m_shader.generateProgramObject();
	m_shader.attachVertexShader(
		getAssetFilePath( "VertexShader.vs" ).c_str() );
	m_shader.attachFragmentShader(
		getAssetFilePath( "FragmentShader.fs" ).c_str() );
	m_shader.link();

	// Set up the uniforms
	P_uni = m_shader.getUniformLocation( "P" );
	V_uni = m_shader.getUniformLocation( "V" );
	M_uni = m_shader.getUniformLocation( "M" );
	col_uni = m_shader.getUniformLocation( "colour" );

	initGrid();
    initActiveCell(1.0f, 0.0f, 1.0f);

	// Set up initial view and projection matrices (need to do this here,
	// since it depends on the GLFW window being set up correctly).
	view = glm::lookAt( 
		glm::vec3( 0.0f, float(DIM)*2.0*M_SQRT1_2, float(DIM)*2.0*M_SQRT1_2 ),
		glm::vec3( 0.0f, 0.0f, 0.0f ),
		glm::vec3( 0.0f, 1.0f, 0.0f ) );

	proj = glm::perspective( 
		glm::radians( 45.0f ),
		float( m_framebufferWidth ) / float( m_framebufferHeight ),
		1.0f, 1000.0f );
}

void A1::initGrid()
{
	size_t sz = 3 * 2 * 2 * (DIM+3);

	float *verts = new float[ sz ];
	size_t ct = 0;
	for( int idx = 0; idx < DIM+3; ++idx ) {
		verts[ ct ] = -1;
		verts[ ct+1 ] = 0;
		verts[ ct+2 ] = idx-1;
		verts[ ct+3 ] = DIM+1;
		verts[ ct+4 ] = 0;
		verts[ ct+5 ] = idx-1;
		ct += 6;

		verts[ ct ] = idx-1;
		verts[ ct+1 ] = 0;
		verts[ ct+2 ] = -1;
		verts[ ct+3 ] = idx-1;
		verts[ ct+4 ] = 0;
		verts[ ct+5 ] = DIM+1;
		ct += 6;
	}

	// Create the vertex array to record buffer assignments.
	glGenVertexArrays( 1, &m_grid_vao );
	glBindVertexArray( m_grid_vao );

	// Create the cube vertex buffer
	glGenBuffers( 1, &m_grid_vbo );
	glBindBuffer( GL_ARRAY_BUFFER, m_grid_vbo );
	glBufferData( GL_ARRAY_BUFFER, sz*sizeof(float),
		verts, GL_STATIC_DRAW );

	// Specify the means of extracting the position values properly.
	GLint posAttrib = m_shader.getAttribLocation( "position" );
	glEnableVertexAttribArray( posAttrib );
	glVertexAttribPointer( posAttrib, 3, GL_FLOAT, GL_FALSE, 0, nullptr );

	// Reset state to prevent rogue code from messing with *my* 
	// stuff!
	glBindVertexArray( 0 );
	glBindBuffer( GL_ARRAY_BUFFER, 0 );
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );

	// OpenGL has the buffer now, there's no need for us to keep a copy.
	delete [] verts;

	CHECK_GL_ERRORS;
}

//----------------------------------------------------------------------------------------
/*
 * Called when program start and update after cube changing
 */
// TODO code organize??
void A1::initActiveCell(float startX, float startY, float startZ)
{
    m_activeCell_color = glm::vec3(1.0f, 0.0f, 0.0f);
    activeCell[0] = startX; /* top right */
    activeCell[1] = startY;
    activeCell[2] = startZ;
    activeCell[3] = startX; /* bottom right */
    activeCell[4] = startY;
    activeCell[5] = startZ - 1;
    activeCell[6] = startX - 1; /* bottom left */
    activeCell[7] = startY;
    activeCell[8] = startZ - 1;
    activeCell[9] = startX - 1; /* top left */
    activeCell[10] = startY;
    activeCell[11] = startZ;
    
    activeCell[12] = startX; /* top right */
    activeCell[13] = startY;
    activeCell[14] = startZ;
    activeCell[15] = startX - 1; /* top left */
    activeCell[16] = startY;
    activeCell[17] = startZ;
    activeCell[18] = startX - 1; /* bottom left */
    activeCell[19] = startY;
    activeCell[20] = startZ - 1;
    activeCell[21] = startX; /* bottom right */
    activeCell[22] = startY;
    activeCell[23] = startZ - 1;
    
    
    // Create the vertex array to record buffer assignments.
    glGenVertexArrays( 1, &m_activeCell_vao );
    glBindVertexArray( m_activeCell_vao );
    
    // Create the cube vertex buffer
    glGenBuffers( 1, &m_activeCell_vbo );
    glBindBuffer( GL_ARRAY_BUFFER, m_activeCell_vbo );
    glBufferData( GL_ARRAY_BUFFER, 24*sizeof(float),
                 activeCell, GL_STATIC_DRAW );
    
    // Specify the means of extracting the position values properly.
    GLint posAttrib = m_shader.getAttribLocation( "position" );
    glEnableVertexAttribArray( posAttrib );
    glVertexAttribPointer( posAttrib, 3, GL_FLOAT, GL_FALSE, 0, nullptr );
    
    // Reset state to prevent rogue code from messing with *my*
    // stuff!
    glBindVertexArray( 0 );
    glBindBuffer( GL_ARRAY_BUFFER, 0 );
    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
    
    
    CHECK_GL_ERRORS;
    
}

void A1::updateActiveCell(bool bShiftKeyPressed, std::pair<float, float> prevActive)
{
    // TODO testing more for the shiftkey situation
    pair<float, float> active = make_pair(activeCell[0], activeCell[2]);
    if (bShiftKeyPressed) {
        float temp = activeCell[1];
        activeCell[1] = 0;
        m_cube_color[active] = m_cube_color[prevActive];
        changeCube(temp);
    } else {
        if (m_cube_height.count(active))
            activeCell[1] = m_cube_height[active];
        else
            activeCell[1] = 0;
        initActiveCell(activeCell[0], activeCell[1], activeCell[2]); // Update active cell

        if (m_cube_height.count(active) == 0)
            m_cube_color[active] = current_col;
    }
}

//----------------------------------------------------------------------------------------
/*
 * Called when space or backspace got fired.
 */

void A1::changeCube(float growNum)
{
    float growLevel = activeCell[1] + growNum;
    size_t cubeDimPair = 3;
    size_t sz = 4 * 3 * 2 * cubeDimPair * growLevel;
    size_t ct = 0;
    float *verts = new float[ sz ];
    
    size_t idxSz = 12 * 3 * growLevel;
    GLuint *idxArray = new GLuint[ idxSz ];
    size_t idxCt = 0;
    GLuint idxCt2 = 0;
    
    
    for (int i=0; i < growLevel; ++i) {
        float startX = activeCell[0];
        float startY = i;
        float startZ = activeCell[2];
        
    /* bottom rectangle */
    verts[ ct ] = startX; /* top right */
    verts[ ct+1 ] = startY;
    verts[ ct+2 ] = startZ;
    verts[ ct+3 ] = startX; /* bottom right */
    verts[ ct+4 ] = startY;
    verts[ ct+5 ] = startZ - 1;
    verts[ ct+6 ] = startX - 1; /* bottom left */
    verts[ ct+7 ] = startY;
    verts[ ct+8 ] = startZ - 1;
    verts[ ct+9 ] = startX - 1; /* top left */
    verts[ ct+10 ] = startY;
    verts[ ct+11 ] = startZ;
    
    ct += 12;
        
    /* top rectangle */
    verts[ ct ] = startX; /* top right */
    verts[ ct+1 ] = startY + 1;
    verts[ ct+2 ] = startZ;
    verts[ ct+3 ] = startX; /* bottom right */
    verts[ ct+4 ] = startY + 1;
    verts[ ct+5 ] = startZ - 1;
    verts[ ct+6 ] = startX - 1; /* bottom left */
    verts[ ct+7 ] = startY + 1;
    verts[ ct+8 ] = startZ - 1;
    verts[ ct+9 ] = startX - 1; /* top left */
    verts[ ct+10 ] = startY + 1;
    verts[ ct+11 ] = startZ;
        
    ct += 12;
    
    startX -= 1;
    startY += 1;
    
    /* left rectangle */
    verts[ ct ] = startX; /* top right */
    verts[ ct+1 ] = startY;
    verts[ ct+2 ] = startZ;
    verts[ ct+3 ] = startX; /* bottom right */
    verts[ ct+4 ] = startY - 1;
    verts[ ct+5 ] = startZ;
    verts[ ct+6 ] = startX; /* bottom left */
    verts[ ct+7 ] = startY - 1;
    verts[ ct+8 ] = startZ - 1;
    verts[ ct+9 ] = startX; /* top left */
    verts[ ct+10 ] = startY;
    verts[ ct+11 ] = startZ - 1;
    
    ct += 12;
    
    /* right rectangle */
    verts[ ct ] = startX + 1; /* top right */
    verts[ ct+1 ] = startY;
    verts[ ct+2 ] = startZ;
    verts[ ct+3 ] = startX + 1; /* bottom right */
    verts[ ct+4 ] = startY - 1;
    verts[ ct+5 ] = startZ;
    verts[ ct+6 ] = startX + 1; /* bottom left */
    verts[ ct+7 ] = startY - 1;
    verts[ ct+8 ] = startZ - 1;
    verts[ ct+9 ] = startX + 1; /* top left */
    verts[ ct+10 ] = startY;
    verts[ ct+11 ] = startZ - 1;
    
    ct += 12;
    
    startX += 1;
    startZ -= 1;
    
    /* front rectangle */
    verts[ ct ] = startX; /* top right */
    verts[ ct+1 ] = startY;
    verts[ ct+2 ] = startZ;
    verts[ ct+3 ] = startX; /* bottom right */
    verts[ ct+4 ] = startY - 1;
    verts[ ct+5 ] = startZ;
    verts[ ct+6 ] = startX - 1; /* bottom left */
    verts[ ct+7 ] = startY - 1;
    verts[ ct+8 ] = startZ;
    verts[ ct+9 ] = startX - 1; /* top left */
    verts[ ct+10 ] = startY;
    verts[ ct+11 ] = startZ;
    
    ct += 12;
    
    /* back rectangle */
    verts[ ct ] = startX; /* top right */
    verts[ ct+1 ] = startY;
    verts[ ct+2 ] = startZ;
    verts[ ct+3 ] = startX; /* bottom right */
    verts[ ct+4 ] = startY - 1;
    verts[ ct+5 ] = startZ;
    verts[ ct+6 ] = startX - 1; /* bottom left */
    verts[ ct+7 ] = startY - 1;
    verts[ ct+8 ] = startZ;
    verts[ ct+9 ] = startX - 1; /* top left */
    verts[ ct+10 ] = startY;
    verts[ ct+11 ] = startZ;
        
    ct += 12;
    
        for (int j = 0; j < 6; ++j ) {
            idxArray[idxCt + j * 6] = idxCt2 + j * 4;
            idxArray[idxCt + j * 6 + 1] = idxCt2 + j * 4 + 3;
            idxArray[idxCt + j * 6 + 2] = idxCt2 + j * 4 + 1;
            idxArray[idxCt + j * 6 + 3] = idxCt2 + j * 4 + 3;
            idxArray[idxCt + j * 6 + 4] = idxCt2 + j * 4 + 2;
            idxArray[idxCt + j * 6 + 5] = idxCt2 + j * 4 + 1;
        }
        idxCt += 36;
        idxCt2 += 24;
        
    }
    
    
    GLuint cube_vao;
    GLuint cube_vbo;
    GLuint cube_ebo;
    
    // Create the vertex array to record buffer assignments.
    glGenVertexArrays( 1, &cube_vao );
    glBindVertexArray( cube_vao );
    
    // Create the cube vertex buffer
    glGenBuffers( 1, &cube_vbo );
    glBindBuffer( GL_ARRAY_BUFFER, cube_vbo );
    glBufferData( GL_ARRAY_BUFFER, sz*sizeof(float),
                 verts, GL_STATIC_DRAW );
    
    // Create the element buffer to record buffer index arrays
    glGenBuffers( 1, &cube_ebo );
    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, cube_ebo );
    glBufferData( GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint)*idxSz,
                 idxArray, GL_STATIC_DRAW );
    
    // Specify the means of extracting the position values properly.
    GLint posAttrib = m_shader.getAttribLocation( "position" );
    glEnableVertexAttribArray( posAttrib );
    glVertexAttribPointer( posAttrib, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr );
    
    
    // Reset state to prevent rogue code from messing with *my*
    // stuff!
    glBindVertexArray( 0 );
    glBindBuffer( GL_ARRAY_BUFFER, 0 );
    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
    
    // OpenGL has the buffer now, there's no need for us to keep a copy.
    delete [] verts;
    delete [] idxArray;
    
    
    // Update cube fields & active cell
    pair<float, float> active = make_pair(activeCell[0], activeCell[2]);
   // m_cube_ordered.push_back(active);
    m_cube_vaos[active] = cube_vao;
    m_cube_vbos[active] = cube_vbo;
    m_cube_ebos[active] = cube_ebo;
    
    
    activeCell[1] += growNum;
    initActiveCell(activeCell[0], activeCell[1], activeCell[2]);
    
    m_cube_height[active] = activeCell[1];
    
    // TODO check code clean
    if ( activeCell[1] == 0 ) {
        m_cube_vaos.erase(active);
        m_cube_vbos.erase(active);
        m_cube_ebos.erase(active);
        m_cube_height.erase(active);
      //  auto it = std::find(m_cube_ordered.begin(), m_cube_ordered.end(), active);
      //  if(it != m_cube_ordered.end())
      //      m_cube_ordered.erase(it);
    }
    
    CHECK_GL_ERRORS;
}

//----------------------------------------------------------------------------------------
/*
 * Called once per frame, before guiLogic().
 */
void A1::appLogic()
{
	// Place per frame, application logic here ...
}

//----------------------------------------------------------------------------------------
/*
 * Called once per frame, after appLogic(), but before the draw() method.
 */
void A1::guiLogic()
{
	// We already know there's only going to be one window, so for 
	// simplicity we'll store button states in static local variables.
	// If there was ever a possibility of having multiple instances of
	// A1 running simultaneously, this would break; you'd want to make
	// this into instance fields of A1.
	static bool showTestWindow(false);
	static bool showDebugWindow(true);

	ImGuiWindowFlags windowFlags(ImGuiWindowFlags_AlwaysAutoResize);
	float opacity(0.5f);

	ImGui::Begin("Debug Window", &showDebugWindow, ImVec2(100,100), opacity, windowFlags);
		if( ImGui::Button( "Quit Application" ) ) {
			glfwSetWindowShouldClose(m_window, GL_TRUE);
        }
    
        // TODO9 Reset Button
        if( ImGui::Button( "Reset" ) ) {
            reset();
        }

        // TODO6 Color stuff (FIXING & PIAZZA QUESTION && edging color problem)
 		// Eventually you'll create multiple colour widgets with
		// radio buttons.  If you use PushID/PopID to give them all
		// unique IDs, then ImGui will be able to keep them separate.
		// This is unnecessary with a single colour selector and
		// radio button, but I'm leaving it in as an example.

		// Prefixing a widget name with "##" keeps it from being
		// displayed.

    
        ImGui::PushID( 0 );
        ImGui::ColorEdit3( "Colour", colour );
        ImGui::SameLine();
        if( ImGui::RadioButton( "Col", &current_col, 0 ) ) {
            pair<float, float> active = make_pair(activeCell[0], activeCell[2]);
            m_cube_color[active] = current_col;
        }
        ImGui::PopID();
    
        ImGui::PushID( 1 );
        ImGui::ColorEdit3( "Colour", colour1 );
        ImGui::SameLine();
        if( ImGui::RadioButton( "Col", &current_col, 1 ) ) {
            pair<float, float> active = make_pair(activeCell[0], activeCell[2]);
            m_cube_color[active] = current_col;
        }
        ImGui::PopID();
    
        ImGui::PushID( 2 );
        ImGui::ColorEdit3( "Colour", colour2 );
        ImGui::SameLine();
        if( ImGui::RadioButton( "Col", &current_col, 2 ) ) {
            pair<float, float> active = make_pair(activeCell[0], activeCell[2]);
            m_cube_color[active] = current_col;
            
        }
        ImGui::PopID();
    
        ImGui::PushID( 3 );
        ImGui::ColorEdit3( "Colour", colour3 );
        ImGui::SameLine();
        if( ImGui::RadioButton( "Col", &current_col, 3 ) ) {
            pair<float, float> active = make_pair(activeCell[0], activeCell[2]);
            m_cube_color[active] = current_col;
            
        }
        ImGui::PopID();
    
        ImGui::PushID( 4 );
        ImGui::ColorEdit3( "Colour", colour4 );
        ImGui::SameLine();
        if( ImGui::RadioButton( "Col", &current_col, 4 ) ) {
            pair<float, float> active = make_pair(activeCell[0], activeCell[2]);
            m_cube_color[active] = current_col;
            
        }
        ImGui::PopID();
    
        ImGui::PushID( 5 );
        ImGui::ColorEdit3( "Colour", colour5 );
        ImGui::SameLine();
        if( ImGui::RadioButton( "Col", &current_col, 5 ) ) {
            pair<float, float> active = make_pair(activeCell[0], activeCell[2]);
            m_cube_color[active] = current_col;
            
        }
        ImGui::PopID();
    
        ImGui::PushID( 6 );
        ImGui::ColorEdit3( "Colour", colour6 );
        ImGui::SameLine();
        if( ImGui::RadioButton( "Col", &current_col, 6 ) ) {
            pair<float, float> active = make_pair(activeCell[0], activeCell[2]);
            m_cube_color[active] = current_col;
            
        }
        ImGui::PopID();
    
        ImGui::PushID( 7 );
        ImGui::ColorEdit3( "Colour", colour7 );
        ImGui::SameLine();
        if( ImGui::RadioButton( "Col", &current_col, 7 ) ) {
            pair<float, float> active = make_pair(activeCell[0], activeCell[2]);
            m_cube_color[active] = current_col;
            
        }
        ImGui::PopID();
    
    
/*
		// For convenience, you can uncomment this to show ImGui's massive
		// demonstration window right in your application.  Very handy for
		// browsing around to get the widget you want.  Then look in 
		// shared/imgui/imgui_demo.cpp to see how it's done.
		if( ImGui::Button( "Test Window" ) ) {
			showTestWindow = !showTestWindow;
		}
*/
    
    ImGui::SliderAngle("Rotation Angle", &m_shape_rotation, -360.0f, 360.0f);
    
		ImGui::Text( "Framerate: %.1f FPS", ImGui::GetIO().Framerate );

    
    
	ImGui::End();

	if( showTestWindow ) {
		ImGui::ShowTestWindow( &showTestWindow );
	}
}

//----------------------------------------------------------------------------------------
/*
 * Called once per frame, after guiLogic().
 */
void A1::draw()
{
	// Create a global transformation for the model (centre it).
	mat4 W;
    
    vec4 dir(0.0f, 0.0f, 1.0f, 0.0f);
    
    vec4 axis = proj * view * dir;
    vec3 finalAxis(axis);
    
    
    W *= glm::scale(mat4(), vec3(m_shape_size));
    W *= glm::rotate(mat4(), m_shape_rotation, finalAxis);
    W *= glm::translate( W, vec3( -float(DIM)/2.0f, 0, -float(DIM)/2.0f ) );
    

	m_shader.enable();
		glEnable( GL_DEPTH_TEST );

		glUniformMatrix4fv( P_uni, 1, GL_FALSE, value_ptr( proj ) );
		glUniformMatrix4fv( V_uni, 1, GL_FALSE, value_ptr( view ) );
		glUniformMatrix4fv( M_uni, 1, GL_FALSE, value_ptr( W ) );
    

        

		// Just draw the grid for now.
		glBindVertexArray( m_grid_vao );
		glUniform3f( col_uni, 1, 1, 1 );
		glDrawArrays( GL_LINES, 0, (3+DIM)*4 );

    
        // Draw the cubes
        for (std::map<pair<float, float>,GLuint>::iterator it=m_cube_vaos.begin(); it!=m_cube_vaos.end(); ++it) {
            glBindVertexArray( it->second );
            int col = m_cube_color[it->first];
            
    
    //for (std::vector<pair<float, float>>::iterator it=m_cube_ordered.begin(); it!=m_cube_ordered.end(); ++it) {
        
     //        glBindVertexArray( m_cube_vaos[*it] );
     //       int col = m_cube_color[*it];
            
            
            if (col == 0) {
                glUniform3f( col_uni, colour[0], colour[1], colour[2] );
            } else if (col == 1) {
                glUniform3f( col_uni, colour1[0], colour1[1], colour1[2] );
            } else if (col == 2) {
                glUniform3f( col_uni, colour2[0], colour2[1], colour2[2] );
            } else if (col == 3) {
                glUniform3f( col_uni, colour3[0], colour3[1], colour3[2] );
            } else if (col == 4) {
                glUniform3f( col_uni, colour4[0], colour4[1], colour4[2] );
            } else if (col == 5) {
                glUniform3f( col_uni, colour5[0], colour5[1], colour5[2] );
            } else if (col == 6) {
                glUniform3f( col_uni, colour6[0], colour6[1], colour6[2] );
            } else if (col == 7) {
                glUniform3f( col_uni, colour7[0], colour7[1], colour7[2] );
            }
            
            float height = m_cube_height[it->first];
        //float height = m_cube_height[*it];
            
            //glUniform3f( col_uni, 1.0f, 0.0f, 0.0f);
            glDrawElements(GL_TRIANGLES, 36 * height, GL_UNSIGNED_INT, 0);
        }
    
		// Highlight the active square.
        glDisable( GL_DEPTH_TEST );
        glBindVertexArray( m_activeCell_vao );
        glUniform3f( col_uni, m_activeCell_color.r, m_activeCell_color.g, m_activeCell_color.b );
        glDrawArrays( GL_LINES, 0, 8 );
        glEnable( GL_DEPTH_TEST );
    
	m_shader.disable();

	// Restore defaults
	glBindVertexArray( 0 );

	CHECK_GL_ERRORS;
}


//----------------------------------------------------------------------------------------
/*
 * Called once when hitting Reset button or pressing R key
 */
void A1::reset()
{
    // TODO More Testing
    // bar reset
    m_cube_vaos.clear();
    m_cube_vbos.clear();
    m_cube_ebos.clear();
    m_cube_height.clear();
    m_cube_color.clear();
    
    // col reset
    colour[0] = 0.0f;
    colour[1] = 0.0f;
    colour[2] = 0.0f;
        
    colour1[0] = 0.0f;
    colour1[1] = 0.0f;
    colour1[2] = 0.0f;
        
    colour2[0] = 0.0f;
    colour2[1] = 0.0f;
    colour2[2] = 0.0f;
        
    colour3[0] = 0.0f;
    colour3[1] = 0.0f;
    colour3[2] = 0.0f;
        
    colour4[0] = 0.0f;
    colour4[1] = 0.0f;
    colour4[2] = 0.0f;
        
    colour5[0] = 0.0f;
    colour5[1] = 0.0f;
    colour5[2] = 0.0f;
        
    colour6[0] = 0.0f;
    colour6[1] = 0.0f;
    colour6[2] = 0.0f;
        
    colour7[0] = 0.0f;
    colour7[1] = 0.0f;
    colour7[2] = 0.0f;
    
    current_col = 0;
    
    // Active cell
    initActiveCell(1.0f, 0.0f, 1.0f);
    
    // key reset
    bShiftKeyPressed = false;
    
    // Field related to mouse event
    bMouseFirstClick = false;
    bMousePressed = false;
    prevMouseX = 0;
    
    // Shape rotation reset
    m_shape_rotation = 0.0f;
    
    // Shape size reset
    m_shape_size = 1.0f;
    
}

//----------------------------------------------------------------------------------------
/*
 * Called once, after program is signaled to terminate.
 */
void A1::cleanup()
{}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles cursor entering the window area events.
 */
bool A1::cursorEnterWindowEvent (
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
bool A1::mouseMoveEvent(double xPos, double yPos) 
{
	bool eventHandled(false);

	if (!ImGui::IsMouseHoveringAnyWindow()) {
		// Put some code here to handle rotations.  Probably need to
		// check whether we're *dragging*, not just moving the mouse.
		// Probably need some instance variables to track the current
		// rotation amount, and maybe the previous X position (so 
		// that you can rotate relative to the *change* in X.
        
        // Check Dragging
        if (bMousePressed) {
            double xDiff = xPos - prevMouseX;
            prevMouseX = xPos;
            
            if (m_shape_rotation < 6.28 || m_shape_rotation > -6.28) {
                m_shape_rotation += 0 - (0.3 * xDiff * 3.14 / 180);
                cout << "m_shape_rotation" << m_shape_rotation << endl;
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
bool A1::mouseButtonInputEvent(int button, int actions, int mods) {
	bool eventHandled(false);

	if (!ImGui::IsMouseHoveringAnyWindow()) {
		// The user clicked in the window.  If it's the left
		// mouse button, initiate a rotation.
        // TODO7 rotate
        if (actions == GLFW_PRESS && button == GLFW_MOUSE_BUTTON_LEFT) {
            if (!bMouseFirstClick) {
                double mouse_x;
                double mouse_y;
                glfwGetCursorPos(m_window, &mouse_x, &mouse_y);
                prevMouseX = mouse_x;
                bMouseFirstClick = true;
            }
            bMousePressed = true;
            eventHandled = true;
        }
        if (actions == GLFW_RELEASE && button == GLFW_MOUSE_BUTTON_LEFT) {
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
bool A1::mouseScrollEvent(double xOffSet, double yOffSet) {
	bool eventHandled(false);
    
    if (!bMousePressed) {
        // TODO8 scale Zoom in or out. More testing
    
        if (yOffSet < 0 && m_shape_size + 0.5 * yOffSet >= 0.0f) {
            m_shape_size = m_shape_size + 0.5 * yOffSet;
        } else if (yOffSet > 0 && m_shape_size + 0.5 * yOffSet <= 10.0f) {
            m_shape_size = m_shape_size + 0.5 * yOffSet;
        }
    
        eventHandled = true;
    }

	return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles window resize events.
 */
bool A1::windowResizeEvent(int width, int height) {
	bool eventHandled(false);

	// Fill in with event handling code...

	return eventHandled;
}

//----------------------------------------------------------------------------------------
/*
 * Event handler.  Handles key input events.
 */
bool A1::keyInputEvent(int key, int action, int mods) {
	bool eventHandled(false);

	// Fill in with event handling code...
	if( action == GLFW_PRESS ) {
		// Respond to some key events.
        if (key == GLFW_KEY_Q) {
            cout << "Q key pressed" << endl;
            // Quit Application
            glfwSetWindowShouldClose(m_window, GL_TRUE);
            eventHandled = true;
        }
        if (key == GLFW_KEY_SPACE) {
            cout << "Space key pressed" << endl;
            pair<float, float> active = make_pair(activeCell[0], activeCell[2]);
             m_cube_color[active] = current_col;
            // Grow Cube
            changeCube(1.0f);
            eventHandled = true;
        }
        if (key == GLFW_KEY_BACKSPACE) {
            cout << "Back Space key pressed" << endl;
            // Shrink Cube
            if (activeCell[1] > 0) {
                pair<float, float> active = make_pair(activeCell[0], activeCell[2]);
                 m_cube_color[active] = current_col;
                changeCube(-1.0f);
            }
            eventHandled = true;
        }
        if (key == GLFW_KEY_UP) {
            cout << "Up key pressed" << endl;
            // limit Up key range due to the ring
            if (activeCell[2] >= 2) {
                pair<float, float> prevActive = make_pair(activeCell[0], activeCell[2]);
                activeCell[2] -= 1;
                updateActiveCell(bShiftKeyPressed, prevActive);
            }
            
            eventHandled = true;
        }
        if (key == GLFW_KEY_DOWN) {
            cout << "Down key pressed" << endl;
            // limit Down key range due to the ring
            if (activeCell[2] <= DIM-1) {
                pair<float, float> prevActive = make_pair(activeCell[0], activeCell[2]);
                activeCell[2] += 1;
                updateActiveCell(bShiftKeyPressed, prevActive);
            }
            eventHandled = true;
        }
        if (key == GLFW_KEY_LEFT) {
            cout << "Left key pressed" << endl;
            // limit Left key range due to the ring
            if (activeCell[0] >= 2) {
                pair<float, float> prevActive = make_pair(activeCell[0], activeCell[2]);
                activeCell[0] -= 1;
                updateActiveCell(bShiftKeyPressed, prevActive);
                
            }
            eventHandled = true;
        }
        if (key == GLFW_KEY_RIGHT) {
            cout << "Right key pressed" << endl;
            // limit Right key range due to the ring
            if (activeCell[0] <= DIM-1) {
                pair<float, float> prevActive = make_pair(activeCell[0], activeCell[2]);
                activeCell[0] += 1;
                updateActiveCell(bShiftKeyPressed, prevActive);
            }
            eventHandled = true;
        }
        if (key == GLFW_KEY_LEFT_SHIFT || key == GLFW_KEY_RIGHT_SHIFT) {
            cout << "Shift key pressed" << endl;
            bShiftKeyPressed = true;
            eventHandled = true;
        }
        if (key == GLFW_KEY_R) {
            cout << "R key pressed" << endl;
            // Reset
            reset();
            eventHandled = true;
        }
        
	}
    
    if ( action == GLFW_RELEASE ) {
        if (key == GLFW_KEY_LEFT_SHIFT || key == GLFW_KEY_RIGHT_SHIFT) {
            cout << "Shift key released" << endl;
            bShiftKeyPressed = false;
            eventHandled = true;
        }
    }

	return eventHandled;
}
