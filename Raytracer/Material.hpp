#pragma once
#include <glm/glm.hpp>

class Material {
public:
  virtual ~Material();
    virtual glm::vec3 getKd(glm::vec2 &textCoord) { return glm::vec3(0.0f); }
    virtual glm::vec3 getKs() { return glm::vec3(0.0f); }
    virtual double getShininess() { return 0.0; }
    virtual double getReflectivity() { return 0.0; }
    virtual double getTransparencyCoeff() { return 0.0; }
    virtual double getRefractionIdx() { return 0.0; }
    virtual bool isTexture() { return false; }

protected:
  Material();
};
