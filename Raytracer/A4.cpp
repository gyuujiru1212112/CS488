#include <glm/ext.hpp>

#include "A4.hpp"
#include "GeometryNode.hpp"
#include "Mesh.hpp"



/* -------------------------------- */

#define epsilon 0.0001

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
            
               
        int maxRecursion, // recursion level for reflection/refraction
        bool supersampling, // supersampling or not
        int softShadowRay, // number of soft shadow rays
               
        bool depthOf, // depth of field or not
        double focalLength, // focal length for dof
        double apertureRadius // aperture radius for dof
) {

  // Fill in raytracing code here...

  std::cout << "Calling A4_Render(\n" <<
		  "\t" << *root <<
          "\t" << "Image(width:" << image.width() << ", height:" << image.height() << ")\n"
          "\t" << "eye:  " << glm::to_string(eye) << std::endl <<
		  "\t" << "view: " << glm::to_string(view) << std::endl <<
		  "\t" << "up:   " << glm::to_string(up) << std::endl <<
		  "\t" << "fovy: " << fovy << std::endl <<
          "\t" << "ambient: " << glm::to_string(ambient) << std::endl <<
		  "\t" << "lights{" << std::endl;

	for(const Light * light : lights) {
		std::cout << "\t\t" <<  *light << std::endl;
	}
	std::cout << "\t}" << std::endl;
	std:: cout <<")" << std::endl;
    
    std::cout <<"\t" << "Max recursion level: " << maxRecursion << std::endl;
    if (supersampling) {
        std:: cout <<  "\t" << "Supersampling is turned on" << std::endl;
    } else {
        std:: cout <<  "\t" << "Supersampling is off" << std::endl;
    }
    std::cout << "\t" << "Soft shadow Ray number: " << softShadowRay << std::endl;
    if (depthOf) {
        std::cout<< "\t" << "Depth of field is turned on" << std::endl
        << "\t" << "Focal Length: " << focalLength << std::endl
        << "\t" << "Aperture Radius: " << apertureRadius << std::endl;
    } else {
        std::cout<< "\t" << "Depth of field is turned off" << std::endl;
    }
    
    maxRecur = maxRecursion;
    softShadowNum = softShadowRay;
    

    // Image height & width
	size_t h = image.height();
	size_t w = image.width();
    
    double scale = tan(glm::radians(fovy * 0.5));
    double aspectRatio = (double)w / (double)h;
    
    // World-to-view Matrix
    glm::mat4 viewMatrix = glm::lookAt(eye, view, up);
    
    // View-to-world Matrix
    glm::mat4 invViewMatrix = glm::inverse(viewMatrix);
    
	for (uint y = 0; y < h; ++y) {
		for (uint x = 0; x < w; ++x) {
            glm::vec3 pixColor(0.0);
            // Reference from http://www.cs.cornell.edu/courses/cs4620/2011fa/lectures/25sampling-AAWeb.pdf
            if (supersampling) {
                for (int dx = -1; dx <= 1; ++dx) {
                    for (int dy = -1; dy <= 1; ++dy) {
                        double ptx = x + dx / 3.0;
                        double pty = y + dy / 3.0;
 
                        double px = (2 * ((ptx + 0.5) / (double) w) - 1) * scale * aspectRatio;
                        double py = (1 - 2 * ((pty + 0.5) /(double) h)) * scale;
                        
                        glm::vec4 rayPWorldV4 = invViewMatrix * glm::vec4(px, py, -1.0, 1.0);
                        glm::vec3 rayPoint = glm::vec3(rayPWorldV4.x, rayPWorldV4.y, rayPWorldV4.z);
                        
                        glm::vec3 rayDir = glm::normalize(rayPoint - eye);
                        glm::vec3 color;
                        
                        if (depthOf) {
                            glm::vec3 focalPoint = eye + focalLength * rayDir;
                            float x_aperture = apertureRadius * (float)rand() / (float)RAND_MAX;
                            float y_aperture = apertureRadius * (float)rand() / (float)RAND_MAX;
                            
                            glm::vec3 newEye = eye + glm::vec3(x_aperture, y_aperture, 0.0);
                            
                            // new direction of ray
                            glm::vec3 newDirection = glm::normalize(focalPoint - newEye);
                            
                            color = castRay(*root, newEye, newDirection, ambient, std::move(lights),
                                               x, y, w, h, 0);
                        } else {
                            color = castRay(*root, eye, rayDir, ambient, std::move(lights),
                                        x, y, w, h, 0);
                        }
                        pixColor += color;
                    }
                }
                pixColor /= 9.0;
            }
            else {
                // pixel x,y in view space
                double px = (2 * ((x + 0.5) / (double) w) - 1) * tan(glm::radians(fovy / 2)) * aspectRatio;
                double py = (1 - 2 * ((y + 0.5) /(double) h)) * tan(glm::radians(fovy / 2));
                
                glm::vec4 rayPWorldV4 = invViewMatrix * glm::vec4(px, py, -1.0, 1.0);
                glm::vec3 rayPoint = glm::vec3(rayPWorldV4.x, rayPWorldV4.y, rayPWorldV4.z);
                glm::vec3 rayDir = glm::normalize(rayPoint - eye);
                
                if (depthOf) {
                    glm::vec3 focalPoint = eye + focalLength * rayDir;
                    float x_aperture = apertureRadius * (float)rand() / (float)RAND_MAX;
                    float y_aperture = apertureRadius * (float)rand() / (float)RAND_MAX;
                        
                    glm::vec3 newEye = eye + glm::vec3(x_aperture, y_aperture, 0.0);
                    
                    // new direction of ray
                    glm::vec3 newDirection = glm::normalize(focalPoint - newEye);

                    pixColor = castRay(*root, newEye, newDirection, ambient, std::move(lights),
                                    x, y, w, h, 0);
                    
                } else {
                    pixColor = castRay(*root, eye, rayDir, ambient, std::move(lights),
                                       x, y, w, h, 0);
                }
            }
            
            image(x, y, 0) = pixColor.x;
            image(x, y, 1) = pixColor.y;
            image(x, y, 2) = pixColor.z;
        }
	}
}

//----------------------------------------------------------------------------------------

glm::vec3 reflectRayDir(glm::vec3 view, glm::vec3 faceNormal)
{
    return view - 2.0 * glm::dot(view, faceNormal) * faceNormal;
}

glm::vec3 refractRayDir(glm::vec3 view, glm::vec3 faceNormal,
                        double transparencyCoeff, double refractionIdx)
{
    double costheta = glm::dot(view, faceNormal);
    costheta = glm::clamp(costheta, -1.0, 1.0);
    glm::vec3 normalTemp = faceNormal;
    if (costheta < 0) {
        // outside the surface
        costheta = -costheta;
        
    } else {
        // inside the surface
        std::swap(transparencyCoeff, refractionIdx);
        normalTemp = -faceNormal;
    }
    
    double coeff = transparencyCoeff / refractionIdx;
    double k = 1.0 - coeff * coeff * (1.0 - costheta * costheta);
    if (k >= 0) {
        return coeff * view + (coeff * costheta - sqrt(k)) * normalTemp;
    } else {
        return glm::vec3(0.0);
    }
}


//----------------------------------------------------------------------------------------

glm::vec3 background(uint x, uint y, size_t w, size_t h)
{
    return glm::vec3((float)x / w, 0.2f, (float)y / h); // background color settings without green
}

//----------------------------------------------------------------------------------------

glm::vec3 castRay(SceneNode &node, glm::vec3 origin, glm::vec3 &rayDir, glm::vec3 ambient,
                   const std::list<Light *> & lights, uint x, uint y, size_t w, size_t h,
                   int recursionDepth)
{
    glm::vec3 color = glm::vec3(0.0f);

    HitInfo hitInfo = node.traceRay(origin, rayDir);
    
    // Primary ray hit
    if (hitInfo.bIsHit){
        // hit something
        
        glm::vec3 hitPoint(hitInfo.hitPoint.x, hitInfo.hitPoint.y, hitInfo.hitPoint.z);
        glm::vec3 faceNormal = glm::normalize(hitInfo.hitNormal);
        
        glm::vec3 hitColorKd(1.0);
        glm::vec3 hitColorKs(0.0);
        double hitShininess = 0.0;
        double hitReflectivity = 0.0;
        double hitTransparencyCoeff = 0.0;
        double hitRefractionIdx = 0.0;
        
        if (hitInfo.hitMaterial != nullptr) {
            glm::vec2 textCoord = hitInfo.hitUV;
            hitColorKd = hitInfo.hitMaterial->getKd(textCoord);
            hitColorKs = hitInfo.hitMaterial->getKs();
            hitShininess = hitInfo.hitMaterial->getShininess();
            hitReflectivity = hitInfo.hitMaterial->getReflectivity();
            hitTransparencyCoeff = hitInfo.hitMaterial->getTransparencyCoeff();
            hitRefractionIdx = hitInfo.hitMaterial->getRefractionIdx();
        }
        
        color += hitColorKd * ambient; // Ambient
        
        // illumination
        {
            
            for(const Light * light : lights) {
                
                // check whether area light or not
                // TODO testing soft shadow
                int shadowSample = 1;
                if (light->isAreaLight()) {
                    shadowSample = softShadowNum;
                }
                
                glm::vec3 illumiColor(0.0);
                for (int i = 0; i < shadowSample; i++) {
                
                glm::vec3 lightPos = light->getLightPos();
                glm::vec3 lightDir = lightPos - hitPoint;
                
                lightDir = glm::normalize(lightDir);
                glm::vec3 shadowOrigin = hitPoint + faceNormal * epsilon;
                
                // shadow ray
                HitInfo hitShadow = node.traceRay(shadowOrigin, lightDir);
                
                if (!hitShadow.bIsHit) {
                    
                    // Diffuse
                    double diffuseRatio = std::max(0.0, (double)glm::dot(faceNormal, lightDir));
                    double r = glm::distance(lightPos, hitPoint);
                    illumiColor += hitColorKd * diffuseRatio * light->colour /
                    (light->falloff[0] + light->falloff[1] * r + light->falloff[2] * r * r); // attenuation
                    
                    // Specular
                    if (glm::length(hitColorKs) > 0) {
                        double reflect = 2.0 * (glm::dot(lightDir, faceNormal));
                        glm::vec3 phongDir = reflect * faceNormal - lightDir;
                        double phongCoeff = std::max(0.0f, glm::dot(phongDir, -rayDir));
                        phongCoeff = pow(phongCoeff, hitShininess);
                        illumiColor += phongCoeff * hitColorKs * light->colour /
                        (light->falloff[0] + light->falloff[1] * r + light->falloff[2] * r * r); // attenuation
                    }
                }
                }
                color += illumiColor / shadowSample;
            }
        }
        
        
        
        // TODO FIX reflectivity > 0
        
        // reflection
        glm::vec3 reflectionColor(0.0);
        if (hitReflectivity > 0 && recursionDepth <= maxRecur) {
            //std::cout << "recursionDepth: " << recursionDepth << std::endl;
            // reflection ray
            glm::vec3 reflectedDir = reflectRayDir(rayDir, faceNormal);
            reflectedDir = glm::normalize(reflectedDir);
            
            reflectionColor = castRay(node, hitPoint, reflectedDir, ambient, lights, x, y, w, h, recursionDepth+1);
        }
        
        glm::vec3 refractionColor(0.0);
        double fr = 1.0;
        // reflection & refraction
        if (hitTransparencyCoeff > 0 &&
            hitRefractionIdx > 0 && recursionDepth <= maxRecur)
        {
          //  std::cout << "recursionDepth: " << recursionDepth << std::endl;
          //  std::cout << hitInfo.hitTransparencyCoeff << " " << hitInfo.hitRefractionIdx << std::endl;
            
            // based on fresnel equation & reference from
            // https://www.scratchapixel.com/lessons/3d-basic-rendering/introduction-to-shading/reflection-refraction-fresnel
            double costheta = glm::dot(rayDir, faceNormal);
            costheta = glm::clamp(costheta, -1.0, 1.0);
            double transparencyCoeff = hitTransparencyCoeff;
            double refractionIdx = hitRefractionIdx;
            glm::vec3 normalTemp(faceNormal.x, faceNormal.y, faceNormal.z);
            
            if (costheta > 0) {
                // inside the surface
                std::swap(transparencyCoeff, refractionIdx);
            }
            
            double coeff = transparencyCoeff / refractionIdx;
            double cost = 1.0 - (coeff * coeff) * (1.0 - (costheta * costheta));
            // Total internal reflection
            if (cost < 0) {
                fr = 1.0;
            }
            else {
                cost = sqrt(cost);
                if (costheta < 0) {
                    // outside the surface
                    costheta = -costheta;
                }
                double Rs = ((refractionIdx * costheta) - (transparencyCoeff * cost)) / ((refractionIdx * costheta) + (transparencyCoeff * cost));
                double Rp = ((transparencyCoeff * costheta) - (refractionIdx * cost)) / ((transparencyCoeff * costheta) + (refractionIdx * cost));
                //std::cout << " Rs " << Rs << std::endl;
                //std::cout << " Rp " << Rp << std::endl;
                fr = (Rs * Rs + Rp * Rp) / 2.0;
            }
            
            if (fr < 1.0) {
                glm::vec3 refractionDir = refractRayDir(rayDir, faceNormal,
                                                        hitTransparencyCoeff, hitRefractionIdx);
                refractionDir = glm::normalize(refractionDir);
                
                refractionColor = castRay(node, hitPoint, refractionDir, ambient, lights, x, y, w, h, recursionDepth+1);
            }
        }
        
       // std::cout << "fr: " << fr << std::endl;
        color +=  hitReflectivity * reflectionColor * fr + refractionColor * (1.0 - fr);
        
        
    } else {
        color = background(x, y, w, h);
    }
    return color;
}
