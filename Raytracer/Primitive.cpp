#include "Primitive.hpp"
#include "polyroots.hpp"
#include <vector>
using namespace glm;


Primitive::~Primitive()
{
}

HitInfo Primitive::intersect(glm::vec3 origin, glm::vec3 &rayDir)
{
    HitInfo hitinfo;
    hitinfo.bIsHit = false;
    return hitinfo;
}

vec3 Primitive::getFaceNormal(glm::vec3 intersection)
{
    return vec3(0.0f);
}

vec2 Primitive::computeTextCoord(glm::vec3 intersection, glm::vec3 normal)
{
    return vec2(0.0f);
}

Torus::Torus() {
    m_nt = new NonhierTorus(5.0, 2.0, glm::vec3(0.0));
}

HitInfo Torus::intersect(glm::vec3 origin, glm::vec3 &rayDir)
{
    return m_nt->intersect(origin, rayDir);
}

vec3 Torus::getFaceNormal(glm::vec3 intersection)
{
    return m_nt->getFaceNormal(intersection);
}

vec2 Torus::computeTextCoord(glm::vec3 intersection, glm::vec3 normal)
{
    return m_nt->computeTextCoord(intersection, normal);
}

Torus::~Torus() {
    delete m_nt;
}

HitInfo NonhierTorus::intersect(glm::vec3 origin, glm::vec3 &rayDir)
{
    HitInfo hitInfo;
    hitInfo.bIsHit = false;
    // Reference and based from http://users.wowway.com/~phkahler/torus.pdf
    
    rayDir = glm::normalize(rayDir);
    vec3 originCenter = origin - m_center;
    
    double u = glm::dot(m_axisRev, originCenter);
    double v = glm::dot(m_axisRev, rayDir);
    
    double a = glm::dot(rayDir, rayDir) - v * v;
    double b = 2.0 * (glm::dot(originCenter, rayDir) - u * v);
    double c = glm::dot(originCenter, originCenter) - u * u;
    double d = glm::dot(originCenter, originCenter) + m_R * m_R - m_r * m_r;
    
    // A = 1
    double B = 4.0 * glm::dot(rayDir, originCenter);
    double C = 2.0 * d + 0.25 * B * B - 4.0 * m_R * m_R * a;
    double D = B * d - 4.0 * m_R * m_R * b;
    double E = d * d - 4.0 * m_R * m_R * c;
    
    double roots[4];
    size_t numRoots = quarticRoots(B, C, D, E, roots);
    double root;
    double tFar;
    
    if (numRoots == 0)
        return hitInfo;
    else if (numRoots == 1) {
        if (roots[0] > t_min) {
            root = roots[0];
            hitInfo.bIsHit = true;
            tFar = root;
        } else
            return hitInfo;
    }
    else {
        std::vector<double> afterFilter;
        for (int i = 0; i < numRoots; i++)
        {
            if (roots[i] > t_min)
                afterFilter.push_back(roots[i]);
        }
        
        if (afterFilter.size() == 0)
            return hitInfo;
        
        double min = INFINITY;
        double max = -INFINITY;
        for (int i = 0; i < afterFilter.size(); i++)
        {
            min = std::min(afterFilter[i],min);
            max = std::max(afterFilter[i], max);
        }
        
        root = min;
        tFar = max;
    }
    
    // hitInfo setting
    hitInfo.bIsHit = true;
    hitInfo.tNear = root;
    hitInfo.tFar = tFar;
    hitInfo.hitPoint = origin + rayDir * (float) root;
    hitInfo.hitPointFar = origin + rayDir * (float) tFar;
    hitInfo.hitNormal = getFaceNormal(hitInfo.hitPoint);
    hitInfo.hitNormalFar = getFaceNormal(hitInfo.hitPointFar);
    hitInfo.hitUV = computeTextCoord(hitInfo.hitPoint, hitInfo.hitNormal);
    hitInfo.hitUVFar = computeTextCoord(hitInfo.hitPointFar, hitInfo.hitNormalFar);
    
    return hitInfo;
}

vec3 NonhierTorus::getFaceNormal(glm::vec3 intersection)
{
     // Reference and based from http://users.wowway.com/~phkahler/torus.pdf
    
    float y = glm::dot((intersection - m_center), m_axisRev);
    vec3 D = (intersection - m_center) - y * m_axisRev;
    vec3 X = D * (1/sqrt(glm::dot(D, D))) * (float)m_R;
    
    vec3 faceNormal = intersection - m_center - X;
    return faceNormal;
}

vec2 NonhierTorus::computeTextCoord(glm::vec3 intersection, glm::vec3 normal)
{
    // Use spherical mapping for torus TODO FIX & TEST
    double theta = acos((m_center.y-intersection.y)/(m_R + m_r));
    double v = (M_PI - theta) / M_PI;
    double phi = acos((m_center.x-intersection.x) / ((m_R + m_r)* sin(M_PI * v)));
    double u = mod(phi, 2.0 * M_PI) / (2.0 * M_PI);
    
    return glm::vec2(u,v);
    
}

NonhierTorus::~NonhierTorus()
{
}

Cylinder::Cylinder()
{
    m_ncy = new NonhierCylinder(1.0, 2.0, glm::vec3(0.0));
}

HitInfo Cylinder::intersect(glm::vec3 origin, glm::vec3 &rayDir)
{
    return m_ncy->intersect(origin, rayDir);
}

vec3 Cylinder::getFaceNormal(glm::vec3 intersection)
{
    return m_ncy->getFaceNormal(intersection);
}

vec2 Cylinder::computeTextCoord(glm::vec3 intersection, glm::vec3 normal)
{
    return m_ncy->computeTextCoord(intersection, normal);
}

Cylinder::~Cylinder()
{
    delete m_ncy;
}

HitInfo NonhierCylinder::intersect(glm::vec3 origin, glm::vec3 &rayDir)
{
    HitInfo hitInfo;
    hitInfo.bIsHit = false;
    
    // x^2 + z^2 = 1
    glm::vec3 originCenter = origin - m_center;
    double a = rayDir.x * rayDir.x + rayDir.z * rayDir.z;
    double b = 2.0 * (rayDir.x * originCenter.x + rayDir.z * originCenter.z);
    double c = originCenter.x * originCenter.x + originCenter.z * originCenter.z - m_radius * m_radius;
    double roots[2];
    size_t numRoots = quadraticRoots(a, b, c, roots);
    double root;
    double tFar;
    
    if (numRoots == 0) {
        return hitInfo;
    }
    else if (numRoots == 1) {
        double t = roots[0];
        if (t < t_min)
            return hitInfo;
        else {
            // y-test
            double y = origin.y + t * rayDir.y;
            if (y >= y_min && y <= y_max) {
                // hit the cylinder side
                // one intersection can only hit the cylinder bit
                root = t;
                double x = origin.x + t * rayDir.x;
                double z = origin.z + t * rayDir.z;
                m_normal = vec3(x, 0.0, z);
                tFar = t;
                m_normalFar = vec3(x, 0.0, z);
                hitInfo.bIsHit = true;
            } else {
                return hitInfo;
            }
        }
    } else {
        // numRoots == 2
        double t0 = roots[0];
        double t1 = roots[1];
        
        if (t1 < t0)
            std::swap(t0, t1);
        
        double y0 = origin.y + t0 * rayDir.y;
        double y1 = origin.y + t1 * rayDir.y;
        
        if (y0 < y_min)
        {
            if (y1 < y_min)
                return hitInfo;
            else
            {
                // hit the bottom cap
                double t = t0 + (t1 - t0) * (y0 - y_min) / (y0 - y1);
                if (t < t_min) return hitInfo;
                
                root = t;
                m_normal = vec3(0.0, -1, 0.0);
                tFar = t;
                m_normalFar = vec3(0.0, -1, 0.0);
                hitInfo.bIsHit = true;
            }
        }
        else if (y0 >= y_min && y0 <= y_max)
        {
            // hit the cylinder side
            if (t0 < t_min) return hitInfo;
            
            root = t0;
            double x = origin.x + t0 * rayDir.x;
            double z = origin.z + t0 * rayDir.z;
            m_normal = vec3(x, 0.0f, z);
            
            tFar = t1;
            x = origin.x + t1 * rayDir.x;
            z = origin.z + t1 * rayDir.z;
            m_normalFar = vec3(x, 0.0f, z);
            hitInfo.bIsHit = true;
        }
        else if (y0 > y_max)
        {
            if (y1 > y_max)
                return hitInfo;
            else
            {
                // hit the top cap
                float t = t0 + (t1 - t0) * (y0 - y_max) / (y0 - y1);
                if (t < t_min) return hitInfo;
                
                root = t;
                m_normal = vec3(0.0, 1, 0.0);
                
                tFar = t;
                m_normalFar = vec3(0.0, 1, 0.0);
                hitInfo.bIsHit = true;
            }
        }
    }
    
    
    // hitInfo setting
    hitInfo.bIsHit = true;
    hitInfo.tNear = root;
    hitInfo.tFar = tFar;
    hitInfo.hitPoint = origin + rayDir * (float) root;
    hitInfo.hitPointFar = origin + rayDir * (float) tFar;
    hitInfo.hitNormal = getFaceNormal(hitInfo.hitPoint);
    hitInfo.hitNormalFar = m_normalFar;
    hitInfo.hitUV = computeTextCoord(hitInfo.hitPoint, hitInfo.hitNormal);
    hitInfo.hitUVFar = computeTextCoord(hitInfo.hitPointFar, hitInfo.hitNormalFar);
    
    return hitInfo;
    
}

vec3 NonhierCylinder::getFaceNormal(glm::vec3 intersection)
{
    return m_normal;
}

vec2 NonhierCylinder::computeTextCoord(glm::vec3 intersection, glm::vec3 normal)
{
    // based on http://www.irisa.fr/prive/kadi/Cours_LR2V/RayTracing_Texturing.pdf
    
    double theta = acos((m_center.x-intersection.x)/m_radius);
    double u = mod(theta, 2.0 * M_PI) / (2.0 * M_PI);
    double v = (y_max - intersection.y) / (y_max - y_min);
    
    return glm::vec2(u,v);
}

NonhierCylinder::~NonhierCylinder()
{
    
}

Cone::Cone()
{
    m_nc = new NonhierCone(1.0, 2.0, glm::vec3(0.0));
}

HitInfo Cone::intersect(glm::vec3 origin, glm::vec3 &rayDir)
{
    return m_nc->intersect(origin, rayDir);
}

vec3 Cone::getFaceNormal(glm::vec3 intersection)
{
    return m_nc->getFaceNormal(intersection);
}

vec2 Cone::computeTextCoord(glm::vec3 intersection, glm::vec3 normal)
{
    return m_nc->computeTextCoord(intersection, normal);
}


Cone::~Cone()
{
    delete m_nc;
}

HitInfo NonhierCone::intersect(glm::vec3 origin, glm::vec3 &rayDir)
{
    HitInfo hitInfo;
    hitInfo.bIsHit = false;
    
    // x^2 + z^2 = y^2
    glm::vec3 originPos = origin - m_center;
    double ratio = m_radius/m_height;
    double ydiff = m_height - origin.y + m_center.y;
    
    double a = rayDir.x * rayDir.x + rayDir.z * rayDir.z - ratio * ratio * rayDir.y * rayDir.y;
    double b = 2.0 * (rayDir.x * originPos.x + rayDir.z * originPos.z + ratio * ratio * rayDir.y * ydiff);
    double c = originPos.x * originPos.x + originPos.z * originPos.z - ratio * ratio * ydiff * ydiff;
    double roots[2];
    size_t numRoots = quadraticRoots(a, b, c, roots);
    double root;
    double tFar;
    
    if (numRoots == 0)
        return hitInfo;
    else if (numRoots == 1) {
        double t = roots[0];
        if (t < t_min)
            return hitInfo;
        else {
            // y test
            double pointY = origin.y + t * rayDir.y;
            if (pointY > m_center.y && pointY < m_center.y + m_height) {
                root = t;
                tFar = t;
                hitInfo.bIsHit = true;
            } else
                return hitInfo;
        }
    }
    else {
        // numRoots == 2
        
        double t0 = roots[0];
        double t1 = roots[1];
        
        if (t0 < t_min && t1 < t_min) {
            return hitInfo;
        } else {
            if (t0 > t1) std::swap(t0, t1);
            tFar = t1;
            
            if (t0 < t_min) {
                t0 = t1;
                if (t0 < t_min)
                    return hitInfo;
            }
            
            double pointY = origin.y + t1 * rayDir.y;
            if (pointY > m_center.y && pointY < m_center.y + m_height) {
                tFar = t1;
            } else
                tFar = t0;
            
            pointY = origin.y + t0 * rayDir.y;
            if (pointY > m_center.y && pointY < m_center.y + m_height) {
                root = t0;
                hitInfo.bIsHit = true;
            } else
                return hitInfo;
        }
    }
    
    // hitInfo setting
    hitInfo.bIsHit = true;
    hitInfo.tNear = root;
    hitInfo.tFar = tFar;
    hitInfo.hitPoint = origin + rayDir * (float) root;
    hitInfo.hitPointFar = origin + rayDir * (float) tFar;
    hitInfo.hitNormal = getFaceNormal(hitInfo.hitPoint);
    hitInfo.hitNormalFar = getFaceNormal(hitInfo.hitPointFar);
    hitInfo.hitUV = computeTextCoord(hitInfo.hitPoint, hitInfo.hitNormal);
    hitInfo.hitUVFar = computeTextCoord(hitInfo.hitPointFar, hitInfo.hitNormalFar);
    
    return hitInfo;
}

vec3 NonhierCone::getFaceNormal(glm::vec3 intersection)
{
    double ratio = sqrt((intersection.x-m_center.x) *
                    (intersection.x-m_center.x) +
                    (intersection.z-m_center.z) *
                    (intersection.z-m_center.z));
    glm::vec3 normal(intersection.x-m_center.x, ratio * (m_radius/m_height), intersection.z-m_center.z);
    return normal;
}

vec2 NonhierCone::computeTextCoord(glm::vec3 intersection, glm::vec3 normal)
{
    double theta = acos((m_center.x - intersection.x)/(m_radius));
    double u = mod(theta, 2.0 * M_PI) / (2.0 * M_PI);
    double v = (m_center.y + m_height - intersection.y) / m_height;
    
    return glm::vec2(u, v);
}

NonhierCone::~NonhierCone()
{
    
}

Sphere::Sphere()
{
    m_ns = new NonhierSphere(vec3(0.0), 1.0);
}

Sphere::~Sphere()
{
    delete m_ns;
}

HitInfo Sphere::intersect(glm::vec3 origin, glm::vec3 &rayDir)
{
    return m_ns->intersect(origin, rayDir);
}

vec3 Sphere::getFaceNormal(glm::vec3 intersection)
{
    return m_ns->getFaceNormal(intersection);
}

vec2 Sphere::computeTextCoord(glm::vec3 intersection, glm::vec3 normal)
{
    return m_ns->computeTextCoord(intersection, normal);
}

Cube::Cube()
{
    m_nb = new NonhierBox(vec3(0.0f), 1.0);
}



Cube::~Cube()
{
    delete m_nb;
}

HitInfo Cube::intersect(glm::vec3 origin, glm::vec3 &rayDir)
{
    return m_nb->intersect(origin, rayDir);
}

vec3 Cube::getFaceNormal(glm::vec3 intersection)
{
    return m_nb->getFaceNormal(intersection);
}

vec2 Cube::computeTextCoord(glm::vec3 intersection, glm::vec3 normal)
{
    return m_nb->computeTextCoord(intersection, normal);
}

NonhierSphere::~NonhierSphere()
{
}

HitInfo NonhierSphere::intersect(glm::vec3 origin, glm::vec3 &rayDir)
{
    HitInfo hitInfo;
    hitInfo.bIsHit = false;
    
    // ray = eye + t * dir
    // sphere using (ray-center)^2 - radius^2 = 0;
    vec3 originCenter = origin - m_pos;
    double a = glm::dot(rayDir, rayDir);
    double b = 2.0 * glm::dot(rayDir, originCenter);
    double c = glm::dot(originCenter, originCenter) - m_radius * m_radius;
    double roots[2];
    double root;
    double tFar;
    
    size_t numRoots = quadraticRoots(a, b, c, roots);
    
    if (numRoots == 0)
        return hitInfo;
    else if (numRoots == 1) {
        if (roots[0] > t_min) {
            root = roots[0];
            tFar = root;
            hitInfo.bIsHit = true;
        } else
            return hitInfo;
    }
    else {
        double root0 = roots[0];
        double root1 = roots[1];
        if (root0 < t_min && root1 < t_min) {
            return hitInfo;
        } else {
            if (root0 > root1) std::swap(root0, root1);
            tFar = root1;
            
            if (root0 < t_min) {
                root0 = root1;
                if (root0 < t_min)
                    return hitInfo;
            }
            root = root0;
            
            hitInfo.bIsHit = true;
        }
    }
    
    // hitInfo setting
    hitInfo.bIsHit = true;
    hitInfo.tNear = root;
    hitInfo.tFar = tFar;
    hitInfo.hitPoint = origin + rayDir * (float) root;
    hitInfo.hitPointFar = origin + rayDir * (float) tFar;
    hitInfo.hitNormal = getFaceNormal(hitInfo.hitPoint);
    hitInfo.hitNormalFar = getFaceNormal(hitInfo.hitPointFar);
    hitInfo.hitUV = computeTextCoord(hitInfo.hitPoint, hitInfo.hitNormal);
    hitInfo.hitUVFar = computeTextCoord(hitInfo.hitPointFar, hitInfo.hitNormalFar);
    
    return hitInfo;
}

vec3 NonhierSphere::getFaceNormal(glm::vec3 intersection)
{
    return intersection - m_pos;
}

vec2 NonhierSphere::computeTextCoord(glm::vec3 intersection, glm::vec3 normal)
{
   // Based on spherical mapping from http://cs-people.bu.edu/sbargal/Fall%202016/lecture_notes/Dec_5_Advanced_Texture_Mapping_And_Ray_Tracing.pdf &
     // http://www.irisa.fr/prive/kadi/Cours_LR2V/RayTracing_Texturing.pdf
    
    double theta = acos((m_pos.y - intersection.y)/m_radius);
    double v = (M_PI - theta) / M_PI;
    double phi = acos((m_pos.x - intersection.x) / (m_radius * sin(M_PI * v)));
    double u = mod(phi, 2.0 * M_PI) / (2.0 * M_PI);
    
    
    return glm::vec2(u,v);
}

NonhierBox::~NonhierBox()
{
}

HitInfo NonhierBox::intersect(glm::vec3 origin, glm::vec3 &rayDir)
{
    HitInfo hitInfo;
    hitInfo.bIsHit = false;
    
    double tFar;
    
    // TODO change to more efficient method
    /* Reference & based from bounding box ray intersection algorithm */
    /* "https://www.scratchapixel.com/lessons/3d-basic-rendering/minimal-ray-tracer-rendering-simple-shapes/ray-box-intersection"*/
    
    glm::vec3 min(m_pos.x, m_pos.y, m_pos.z);
    glm::vec3 max(m_pos.x+m_size, m_pos.y+m_size, m_pos.z+m_size);
    glm::vec3 dir(rayDir.x, rayDir.y, rayDir.z);
    
    float tmin = (min.x - origin.x) / dir.x;
    float tmax = (max.x - origin.x) / dir.x;
    
    
    if (tmin > tmax) std::swap(tmin, tmax);
    
    tFar = tmax;
    
    float tymin = (min.y - origin.y) / dir.y;
    float tymax = (max.y - origin.y) / dir.y;
    
    if (tymin > tymax) std::swap(tymin, tymax);
    
    if (tFar < tymax) tFar = tymax;
    
    if ((tmin > tymax) || (tymin > tmax))
        return hitInfo;
    
    if (tymin > tmin)
        tmin = tymin;
    
    if (tymax < tmax)
        tmax = tymax;
    
    float tzmin = (min.z - origin.z) / dir.z;
    float tzmax = (max.z - origin.z) / dir.z;
    
    if (tzmin > tzmax) std::swap(tzmin, tzmax);
    
    if (tFar < tzmax) tFar = tzmax;
    
    if ((tmin > tzmax) || (tzmin > tmax))
        return hitInfo;
    
    if (tzmin > tmin)
        tmin = tzmin;
    
    if (tzmax < tmax)
        tmax = tzmax;
    
    double root = tmin;
    
    if (root < t_min) {
        root = tmax;
        if (root < t_min) return hitInfo;
    }
    
    // hitInfo setting
    hitInfo.bIsHit = true;
    hitInfo.tNear = root;
    hitInfo.tFar = tFar;
    hitInfo.hitPoint = origin + rayDir * (float) root;
    hitInfo.hitPointFar = origin + rayDir * (float) tFar;
    hitInfo.hitNormal = getFaceNormal(hitInfo.hitPoint);
    hitInfo.hitNormalFar = getFaceNormal(hitInfo.hitPointFar);
    hitInfo.hitUV = computeTextCoord(hitInfo.hitPoint, hitInfo.hitNormal);
    hitInfo.hitUVFar = computeTextCoord(hitInfo.hitPointFar, hitInfo.hitNormalFar);
    
    return hitInfo;
}

vec3 NonhierBox::getFaceNormal(glm::vec3 intersection)
{
    float epislon = 0.01;
    vec3 faceNormal(0.0f);
    if (abs(intersection.x - m_pos.x) < epislon)
        faceNormal = vec3(-1.0f,0.0f,0.0f);
    else if(abs(intersection.x - m_pos.x - m_size) < epislon)
        faceNormal = vec3(1.0f,0.0f,0.0f);
    else if(abs(intersection.y - m_pos.y) < epislon)
    {
        faceNormal = vec3(0.0f,-1.0f,0.0f);
    }
    else if(abs(intersection.y - m_pos.y - m_size) < epislon)
        faceNormal = vec3(0.0f,1.0f,0.0f);
    else if (abs(intersection.z - m_pos.z) < epislon)
        faceNormal = vec3(0.0f,0.0f,-1.0f);
    else if(abs(intersection.z - m_pos.z - m_size) < epislon)
        faceNormal = vec3(0.0f,0.0f,1.0f);
 
    return faceNormal;
}

vec2 NonhierBox::computeTextCoord(glm::vec3 intersection, glm::vec3 normal)
{
    // TODO spherical mapping fix & test
    
    double theta = acos((m_pos.y + m_size/2 - intersection.y)/m_size);
    double v = (M_PI - theta) / M_PI;
    double phi = acos((m_pos.x + m_size/2 - intersection.x) / (m_size * sin(M_PI * v)));
    double u = mod(phi, 2.0 * M_PI) / (2.0 * M_PI);
    
    /*
    double u = intersection.x / m_size;
    double v = intersection.y / m_size; */
    return vec2(u, v);
}


