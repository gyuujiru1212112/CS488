#pragma once

#include <vector>
#include <map>
#include <glm/glm.hpp>

#include "cs488-framework/CS488Window.hpp"
#include "cs488-framework/OpenGLImport.hpp"
#include "cs488-framework/ShaderProgram.hpp"

#include "grid.hpp"

class A1 : public CS488Window {
public:
	A1();
	virtual ~A1();

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

private:
	void initGrid();
    void initActiveCell(float startX, float startY, float startZ);
    void changeCube(float growNum); // 1 for growing cubes; -1 for shrinking cubes; other numbers for growinglevel
    void updateActiveCell(bool bShiftKeyPressed, std::pair<float, float> prevActive);
    void reset();
    
	// Fields related to the shader and uniforms.
	ShaderProgram m_shader;
	GLint P_uni; // Uniform location for Projection matrix.
	GLint V_uni; // Uniform location for View matrix.
	GLint M_uni; // Uniform location for Model matrix.
	GLint col_uni;   // Uniform location for cube colour.

	// Fields related to grid geometry.
	GLuint m_grid_vao; // Vertex Array Object
	GLuint m_grid_vbo; // Vertex Buffer Object
    
    // Fields related to cube geometry.
    std::vector<std::pair<float, float>> m_cube_ordered;
    std::map<std::pair<float, float>, GLuint> m_cube_vaos;
    std::map<std::pair<float, float>, GLuint> m_cube_vbos;
    std::map<std::pair<float, float>, GLuint> m_cube_ebos;
    std::map<std::pair<float, float>, float> m_cube_height;
    std::map<std::pair<float, float>, int> m_cube_color;

	// Matrices controlling the camera and projection.
	glm::mat4 proj;
	glm::mat4 view;

	float colour[3];
    float colour1[3];
    float colour2[3];
    float colour3[3];
    float colour4[3];
    float colour5[3];
    float colour6[3];
    float colour7[3];
    
	int current_col;
    
    // Active cell
    float activeCell[24];
    GLuint m_activeCell_vao;
    GLuint m_activeCell_vbo;
    glm::vec3 m_activeCell_color;
    
    // Field related to key event
    bool bShiftKeyPressed;
    
    // Field related to mouse event
    bool bMouseFirstClick;
    bool bMousePressed;
    double prevMouseX;
    
    // Shape rotation angle in radians.
    float m_shape_rotation;
    
    // Positive float range for size of shape.
    // Clamped to be within [0.0, 10.0].
    // Shape starts at size 1.0
    float m_shape_size;
};
