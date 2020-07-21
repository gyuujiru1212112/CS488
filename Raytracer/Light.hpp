#pragma once

#include <iosfwd>

#include <glm/glm.hpp>

// Represents a simple point light.
class Light {
public:
  Light();
  glm::vec3 colour;
  glm::vec3 position;
  double falloff[3];
  virtual bool isAreaLight() const { return false; }
  virtual glm::vec3 getLightPos() const { return position; }
  virtual double getWidth() const {return 0.0;}
  virtual double getHeight() const {return 0.0;}	
};


class AreaLight: public Light {
public:
    AreaLight(const glm::vec3& pos, double width, double height, const glm::vec3& col,
                  double fof[3]);
    virtual bool isAreaLight() const { return true; }
    virtual glm::vec3 getLightPos() const;
    virtual double getWidth() const {return m_width;}
    virtual double getHeight() const {return m_height;}
private:
    double m_width;
    double m_height;
};

std::ostream& operator<<(std::ostream& out, const Light& l);
