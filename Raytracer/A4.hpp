#pragma once

#include <glm/glm.hpp>
#include <stack>

#include "SceneNode.hpp"
#include "Light.hpp"
#include "Image.hpp"
#include "Primitive.hpp"
#include "GeometryNode.hpp"

static int softShadowNum = 1;
static int maxRecur = 100;

void A4_Render(
		// What to render
		SceneNode * root,

		// Image to write to, set to a given width and height
		Image & image,

		// Viewing parameters
		const glm::vec3 & eye,
		const glm::vec3 & view,
		const glm::vec3 & up,
		double fovy,

		// Lighting parameters
		const glm::vec3 & ambient,
		const std::list<Light *> & lights,
               
        int maxRecursion,
        bool supersampling,
        int softShadowRay,
               
        bool depthOf,
        double focalLength,
        double apertureRadius
);

glm::vec3 background(uint x, uint y, size_t w, size_t h);

glm::vec3 castRay(SceneNode &node, glm::vec3 origin, glm::vec3 &rayDir, glm::vec3 ambient,
                   const std::list<Light *> & lights, uint x, uint y, size_t w, size_t h,
                   int recursionDepth);

glm::vec3 reflectRayDir(glm::vec3 view, glm::vec3 faceNormal);
glm::vec3 refractRayDir(glm::vec3 view, glm::vec3 faceNormal,
                        double transparencyCoeff, double refractionIdx);
