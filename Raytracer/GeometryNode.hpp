#pragma once

#include "SceneNode.hpp"
#include "Primitive.hpp"
#include "Material.hpp"
#include "Texture.hpp"

class GeometryNode : public SceneNode {
public:
	GeometryNode( const std::string & name, Primitive *prim, 
		Material *mat = nullptr);

	void setMaterial( Material *material );
    virtual HitInfo traceRay(glm::vec3 &origin, glm::vec3 &rayDir);

	Material *m_material;
	Primitive *m_primitive;
    bool isPrimitive = true;
};



class CSGNode: public GeometryNode
{
public:
    CSGNode(GeometryNode *left, GeometryNode *right, std::string opStr);
    virtual HitInfo traceRay(glm::vec3 &origin, glm::vec3 &rayDir);
    std::vector<HitInfo> csgTraceRay(glm::vec3 &origin, glm::vec3 &rayDir);
    std::vector<HitInfo> mergeList(std::vector<HitInfo> leftList, std::vector<HitInfo> rightList,
                                   std::string m_op);
    
    GeometryNode *m_left;
    GeometryNode *m_right;
    std::string m_op;
};
