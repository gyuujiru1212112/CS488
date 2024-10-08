#pragma once

#include "cs488-framework/CS488Window.hpp"
#include "cs488-framework/OpenGLImport.hpp"
#include "cs488-framework/ShaderProgram.hpp"
#include "cs488-framework/MeshConsolidator.hpp"

#include "SceneNode.hpp"
#include "JointNode.hpp"
#include "ChangeJointAngleCommand.hpp"

#include <glm/glm.hpp>
#include <memory>
#include <stack>
#include <map>

struct LightSource {
	glm::vec3 position;
	glm::vec3 rgbIntensity;
};


class A3 : public CS488Window {
public:
	A3(const std::string & luaSceneFile);
	virtual ~A3();

protected:
	virtual void init() override;
	virtual void appLogic() override;
	virtual void guiLogic() override;
	virtual void draw() override;
	virtual void cleanup() override;

	//-- Virtual callback methods
	virtual bool cursorEnterWindowEvent(int entered) override;
	virtual bool mouseMoveEvent(double xPos, double yPos) override;
	virtual bool mouseButtonInputEvent(int button, int actions, int mods) override;
	virtual bool mouseScrollEvent(double xOffSet, double yOffSet) override;
	virtual bool windowResizeEvent(int width, int height) override;
	virtual bool keyInputEvent(int key, int action, int mods) override;

	//-- One time initialization methods:
	void processLuaSceneFile(const std::string & filename);
	void createShaderProgram();
	void enableVertexShaderInputSlots();
	void uploadVertexDataToVbos(const MeshConsolidator & meshConsolidator);
	void mapVboDataToVertexShaderInputLocations();
	void initViewMatrix();
	void initLightSources();

	void initPerspectiveMatrix();
	void uploadCommonSceneUniforms();
	void renderGeometryNode(const SceneNode &node);
	void renderArcCircle();
    void traverseRenderGeometry(const SceneNode &node); // Render Geomerty Node recursively
    void traverseRenderJoint(const SceneNode &node); // Render Joint Node recursively
    void setOption(const std::string &option);
    void resetPosition();
    void resetOrientation();
    void resetJoints();
    void resetAll();
    void undo();
    void redo();
    /* trackball function from the demo */
    void vCalcRotVec(float fNewX, float fNewY,
                     float fOldX, float fOldY,
                     float fDiameter,
                     float *fVecX, float *fVecY, float *fVecZ);
    glm::mat4 vAxisRotMatrix(float fVecX, float fVecY, float fVecZ);
    void initSelectableNodeMap(const SceneNode &node);

	glm::mat4 m_perpsective;
	glm::mat4 m_view;
    
    glm::mat4 m_translation;
    glm::mat4 m_orientation;

	LightSource m_light;

	//-- GL resources for mesh geometry data:
	GLuint m_vao_meshData;
	GLuint m_vbo_vertexPositions;
	GLuint m_vbo_vertexNormals;
	GLint m_positionAttribLocation;
	GLint m_normalAttribLocation;
	ShaderProgram m_shader;

	//-- GL resources for trackball circle geometry:
	GLuint m_vbo_arcCircle;
	GLuint m_vao_arcCircle;
	GLint m_arc_positionAttribLocation;
	ShaderProgram m_shader_arcCircle;

	// BatchInfoMap is an associative container that maps a unique MeshId to a BatchInfo
	// object. Each BatchInfo object contains an index offset and the number of indices
	// required to render the mesh with identifier MeshId.
	BatchInfoMap m_batchInfoMap;

	std::string m_luaSceneFile;

	std::shared_ptr<SceneNode> m_rootNode;
    glm::mat4 m_rootNodeDefaultTrans;
    
    // Matrix transform stack
    std::stack <glm::mat4> m_matrixStack;
    
    
    // Mode related fields
    int m_radioButtonChoice;
    std::string m_currentMode;
    
    /* Options related fields */
    // current option enable/disable
    bool m_bCircle;
    bool m_bZbuffer;
    bool m_bBackCull;
    bool m_bFrontCull;
    
    // previous option enable/disable
    bool m_bPrevCircle;
    bool m_bPrevZbuffer;
    bool m_bPrevBackCull;
    bool m_bPrevFrontCull;
    
    // Mouse event related fields
    bool bMouseFirstClick;
    bool bMousePressed;
    double prevMouseX;
    double prevMouseY;

    // Picking related fields
    bool m_bPicking;
    std::map<unsigned int, JointNode *> m_mapChildJoint;
    std::vector<JointNode *> m_jointNodes;
    
    // Undo/Redo related fields
    std::map<JointNode *, ChangeJointAngleCommand *> m_mapJointCmd;
};
