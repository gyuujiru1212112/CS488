#pragma once

#include "cs488-framework/CS488Window.hpp"
#include "cs488-framework/OpenGLImport.hpp"
#include "cs488-framework/ShaderProgram.hpp"

#include <glm/glm.hpp>

#include <vector>
#include <string>

// Set a global maximum number of vertices in order to pre-allocate VBO data
// in one shot, rather than reallocating each frame.
const GLsizei kMaxVertices = 1000;


// Convenience class for storing vertex data in CPU memory.
// Data should be copied over to GPU memory via VBO storage before rendering.
class VertexData {
public:
	VertexData();

	std::vector<glm::vec2> positions;
	std::vector<glm::vec3> colours;
	GLuint index;
	GLsizei numVertices;
};


class A2 : public CS488Window {
public:
	A2();
	virtual ~A2();

protected:
	virtual void init() override;
	virtual void appLogic() override;
	virtual void guiLogic() override;
	virtual void draw() override;
	virtual void cleanup() override;

	virtual bool cursorEnterWindowEvent(int entered) override;
	virtual bool mouseMoveEvent(double xPos, double yPos) override;
	virtual bool mouseButtonInputEvent(int button, int actions, int mods) override;
	virtual bool mouseScrollEvent(double xOffSet, double yOffSet) override;
	virtual bool windowResizeEvent(int width, int height) override;
	virtual bool keyInputEvent(int key, int action, int mods) override;

	void createShaderProgram();
	void enableVertexAttribIndices();
	void generateVertexBuffers();
	void mapVboDataToVertexAttributeLocation();
	void uploadVertexDataToVbos();

	void initLineData();

	void setLineColour(const glm::vec3 & colour);

	void drawLine (
			const glm::vec2 & v0,
			const glm::vec2 & v1
	);
    
    void initCubeData();
    void initGnomonsData();
    void initViewport();
    
    glm::mat4 makeProjMatrix();
    
    
    bool clipping(glm::vec4 & v1,glm::vec4 & v2);
    void drawInsideViewport(const glm::vec4 & v1,
                            const glm::vec4 & v2);
    glm::mat4 rotateMatrix (const glm::vec3 & rotation); // return rotate matrix
    
    glm::mat4 translateMatrix (const glm::vec3 & v); // return translate matrix
    
    glm::mat4 scaleMatrix (const glm::vec3 & v); // return scale matrix
    
    void reset();
    

	ShaderProgram m_shader;
    
    // Field related to radio button
    int m_radioButtonChoice;
    
    // Field related to mouse event
    bool bMouseFirstClick;
    bool bMousePressed;
    double prevMouseX;
    double prevMouseY;
    

	GLuint m_vao;            // Vertex Array Object
	GLuint m_vbo_positions;  // Vertex Buffer Object
	GLuint m_vbo_colours;    // Vertex Buffer Object

	VertexData m_vertexData;
    
    std::vector <glm::vec4> m_cubeVertices; // cube vertices
    
    glm::vec4 m_worldGnomon[6]; // world gnomon vertices
    glm::vec4 m_modelGnomon[6]; // model gnomon vertices
    
    std::string m_currentMode; // current mode
    
    /* Transformation Matrix */
    glm::mat4 m_proj; // view to proj matrix
    glm::mat4 m_view; // world to view matrix
    glm::mat4 m_model; // model to world matrix
    float m_FOV; // Field of view default to 30 range 5 to 160
    float m_near;
    float m_far;
    
    
    /* viewport field */
    glm::vec2 m_viewportVertices[8];
    glm::vec2 m_viewportCenter;
    
    float m_viewportHalfWidth;
    float m_viewportHalfHeight;
   

	glm::vec3 m_currentLineColour;

};
