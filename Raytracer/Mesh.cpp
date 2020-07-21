#include <iostream>
#include <fstream>

#include <glm/ext.hpp>

// #include "cs488-framework/ObjFileDecoder.hpp"
#include "Mesh.hpp"

/* phong shading flag */
#define phong_shading true

Mesh::Mesh( const std::string& fname )
	: m_vertices()
	, m_faces()
{
	std::string code;
	double vx, vy, vz;
	size_t s1, s2, s3;
    
	std::ifstream ifs( fname.c_str() );
	while( ifs >> code ) {
		if( code == "v" ) {
			ifs >> vx >> vy >> vz;
            
			m_vertices.push_back( glm::vec3( vx, vy, vz ) );
		} else if( code == "f" ) {
			ifs >> s1 >> s2 >> s3;
			m_faces.push_back( Triangle( s1 - 1, s2 - 1, s3 - 1 ) );
		}
	}
    if (m_faces.size() > 0)
        calculateVertexNormals();
}

void Mesh::calculateVertexNormals()
{
    for (glm::vec3 vertex: m_vertices) {
        m_vertexNormals.push_back(glm::vec3(0.0));
    }
    for (Triangle triangle: m_faces)
    {
        glm::vec3 vert0 = m_vertices[triangle.v1];
        glm::vec3 vert1 = m_vertices[triangle.v2];
        glm::vec3 vert2 = m_vertices[triangle.v3];
        
        // Calculate the faceNormal
        glm::vec3 edge01 = vert1 - vert0;
        glm::vec3 edge02 = vert2 - vert0;
        
        glm::vec3 faceNormal = glm::cross(edge01, edge02);
        
        m_vertexNormals[triangle.v1] = m_vertexNormals[triangle.v1] + faceNormal;
        m_vertexNormals[triangle.v2] = m_vertexNormals[triangle.v2] + faceNormal;
        m_vertexNormals[triangle.v3] = m_vertexNormals[triangle.v3] + faceNormal;
    }
}

HitInfo Mesh::intersect(glm::vec3 origin, glm::vec3 &rayDir)
{
    HitInfo hitInfo;
    hitInfo.bIsHit = false;
    hitInfo.tNear = INFINITY;
    
    if (m_faces.size() == 0)
        return hitInfo;
    
    glm::vec3 baryPos;
    
    for (Triangle triangle: m_faces) {
            
        glm::vec3 vert0 = m_vertices[triangle.v1];
        glm::vec3 vert1 = m_vertices[triangle.v2];
        glm::vec3 vert2 = m_vertices[triangle.v3];
        
        
        if(glm::intersectRayTriangle(origin, rayDir, vert0, vert1, vert2, baryPos) &&
           baryPos.z > 0 && baryPos.z < hitInfo.tNear) {
            
            hitInfo.tNear = baryPos.z;
            hitInfo.bIsHit = true;
            
            if (!phong_shading) {
                // old way
                // Calculate the normal
                glm::vec3 edge01 = vert1 - vert0;
                glm::vec3 edge02 = vert2 - vert0;
            
                hitInfo.hitNormal = glm::cross(edge01, edge02);
            }
            else {
                // phong shading: interpolating vertex normal with barycentric point
                // calculate the barycentric coordinates
                // reference from
                https://gamedev.stackexchange.com/questions/23743/whats-the-most-efficient-way-to-find-barycentric-coordinates
            
                hitInfo.hitNormal = (1 - baryPos.x - baryPos.y) * glm::normalize(m_vertexNormals[triangle.v1]) +
                                         baryPos.x * glm::normalize(m_vertexNormals[triangle.v2]) +
                                         baryPos.y * glm::normalize(m_vertexNormals[triangle.v3]);
            }
        }
    }
    
    if (hitInfo.tNear == INFINITY)
        return hitInfo;
    
    // hitInfo setting
    hitInfo.bIsHit = true;
    hitInfo.hitPoint = origin + rayDir * (float) hitInfo.tNear;
    
    hitInfo.hitUV = computeTextCoord(hitInfo.hitPoint, hitInfo.hitNormal);
    
    return hitInfo;
}

glm::vec2 Mesh::computeTextCoord(glm::vec3 intersection, glm::vec3 normal)
{
    return glm::vec2(0, 0);
}

std::ostream& operator<<(std::ostream& out, const Mesh& mesh)
{
  out << "mesh {";
  /*
  
  for( size_t idx = 0; idx < mesh.m_verts.size(); ++idx ) {
  	const MeshVertex& v = mesh.m_verts[idx];
  	out << glm::to_string( v.m_position );
	if( mesh.m_have_norm ) {
  	  out << " / " << glm::to_string( v.m_normal );
	}
	if( mesh.m_have_uv ) {
  	  out << " / " << glm::to_string( v.m_uv );
	}
  }

*/
  out << "}";
  return out;
}


