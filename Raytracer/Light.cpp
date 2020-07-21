#include <iostream>

#include <glm/ext.hpp>

#include "Light.hpp"

Light::Light()
  : colour(0.0, 0.0, 0.0),
    position(0.0, 0.0, 0.0)
{
  falloff[0] = 1.0;
  falloff[1] = 0.0;
  falloff[2] = 0.0;
}


AreaLight::AreaLight(const glm::vec3& pos, double width, double height, const glm::vec3& col,
                     double fof[3])
: m_width(width)
, m_height(height)
{
    position = glm::vec3(pos.x, pos.y, pos.z);
    colour = glm::vec3(col.x, col.y, col.z);
    falloff[0] = fof[0];
    falloff[1] = fof[1];
    falloff[2] = fof[2];
}

glm::vec3 AreaLight::getLightPos() const
{
    double max = (double)rand() / RAND_MAX;
    
    double xmin = position.x - m_width/2.0;
    double xmax = position.x + m_width/2.0;
    // pick a random x value between this range
    double xRan = xmin + max * (xmax - xmin);
    
    double zmin = position.z - m_height/2.0;
    double zmax = position.z + m_height/2.0;
    // pick a random z value between this range
    double zRan = zmin + max * (zmax - zmin);
    
    return glm::vec3(xRan, position.y, zRan);
}

std::ostream& operator<<(std::ostream& out, const Light& l)
{
  out << "L[" << glm::to_string(l.colour) 
  	  << ", " << glm::to_string(l.position) << ", ";
  for (int i = 0; i < 3; i++) {
    if (i > 0) out << ", ";
    out << l.falloff[i];
  } 
  out << "; width: " << l.getWidth() << ", height: " << l.getHeight();
  out << "]";
  return out;
}
