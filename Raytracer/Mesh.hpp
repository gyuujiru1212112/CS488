#pragma once

#include <vector>
#include <map>
#include <iosfwd>
#include <string>

#include <glm/glm.hpp>

#include "Primitive.hpp"

struct Triangle
{
	size_t v1;
	size_t v2;
	size_t v3;
    glm::vec3 normal;

	Triangle( size_t pv1, size_t pv2, size_t pv3 )
		: v1( pv1 )
		, v2( pv2 )
		, v3( pv3 )
	{}
};

// A polygonal mesh.
class Mesh : public Primitive {
public:
  Mesh( const std::string& fname );
  virtual HitInfo intersect(glm::vec3 origin, glm::vec3 &rayDir);
  virtual glm::vec2 computeTextCoord(glm::vec3 intersection, glm::vec3 normal);
  virtual bool isMesh() { return true; };
  void calculateVertexNormals();
	
  std::vector<glm::vec3> m_vertices;
  std::vector<Triangle> m_faces;
  std::vector<glm::vec3> m_vertexNormals;
    
  friend std::ostream& operator<<(std::ostream& out, const Mesh& mesh);
};
