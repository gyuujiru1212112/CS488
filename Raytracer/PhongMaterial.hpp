#pragma once

#include <glm/glm.hpp>

#include "Material.hpp"

class PhongMaterial : public Material {
public:
  PhongMaterial(const glm::vec3& kd, const glm::vec3& ks, double shininess, double reflectivity,
                double transparencyCoeff, double refractionIdx);
  virtual ~PhongMaterial();
  virtual glm::vec3 getKd(glm::vec2 &textCoord) { return m_kd; };
  virtual glm::vec3 getKs() { return m_ks; }
  virtual double getShininess() { return m_shininess; }
  virtual double getReflectivity() { return m_reflectivity; }
  virtual double getTransparencyCoeff() { return m_transparencyCoeff; }
  virtual double getRefractionIdx() { return m_refractionIdx; }
  virtual bool isTexture() { return false; }

private:
  glm::vec3 m_kd;
  glm::vec3 m_ks;

  double m_shininess;
  double m_reflectivity;
  double m_transparencyCoeff;
  double m_refractionIdx;
};
