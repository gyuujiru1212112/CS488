#include "PhongMaterial.hpp"

PhongMaterial::PhongMaterial(
	const glm::vec3& kd, const glm::vec3& ks, double shininess, double reflectivity,
                             double transparencyCoeff, double refractionIdx)
	: m_kd(kd)
	, m_ks(ks)
	, m_shininess(shininess)
    , m_reflectivity(reflectivity)
    , m_transparencyCoeff(transparencyCoeff)
    , m_refractionIdx(refractionIdx)
{}

PhongMaterial::~PhongMaterial()
{}
