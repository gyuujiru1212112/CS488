#pragma once

#include <glm/glm.hpp>
#include <map>
#include <iostream>
#include "HitInfo.h"

class Primitive {
public:
  virtual ~Primitive();
    virtual HitInfo intersect(glm::vec3 origin, glm::vec3 &rayDir);
    virtual glm::vec3 getFaceNormal(glm::vec3 intersection);
    virtual glm::vec2 computeTextCoord(glm::vec3 intersection, glm::vec3 normal);
    virtual bool isMesh() { return false; };
protected:
    double t_min = 1e-2; // TODO t-range
    glm::vec3 m_normal;
};

class Torus: public Primitive {
public:
    Torus();
    virtual ~Torus();
    virtual HitInfo intersect(glm::vec3 origin, glm::vec3 &rayDir);
    virtual glm::vec3 getFaceNormal(glm::vec3 intersection);
    virtual glm::vec2 computeTextCoord(glm::vec3 intersection, glm::vec3 normal);
    virtual bool isMesh() { return false; };
private:
    Primitive *m_nt;
};

class NonhierTorus : public Primitive {
public:
    NonhierTorus(double R, double r, const glm::vec3& pos)
    : m_R(R)
    , m_r(r)
    , m_center(pos)
    {
        m_axisRev = glm::vec3(0,0,1);
    }
    virtual ~NonhierTorus();
    virtual HitInfo intersect(glm::vec3 origin, glm::vec3 &rayDir);
    virtual glm::vec3 getFaceNormal(glm::vec3 intersection);
    virtual glm::vec2 computeTextCoord(glm::vec3 intersection, glm::vec3 normal);
    virtual bool isMesh() { return false; };
private:
    double m_R; // radius of circle
    double m_r; // radius of tube
    glm::vec3 m_axisRev; // revolution axis
    glm::vec3 m_center; // center
};


class Cylinder: public Primitive {
public:
    Cylinder();
    virtual ~Cylinder();
    virtual HitInfo intersect(glm::vec3 origin, glm::vec3 &rayDir);
    virtual glm::vec3 getFaceNormal(glm::vec3 intersection);
    virtual glm::vec2 computeTextCoord(glm::vec3 intersection, glm::vec3 normal);
    virtual bool isMesh() { return false; };
private:
    Primitive *m_ncy;
};

class NonhierCylinder: public Primitive {
public:
    NonhierCylinder(double radius, double height, const glm::vec3& pos)
    : y_min (pos.y - height/2.0)
    , y_max (pos.y + height/2.0)
    , m_radius (radius)
    , m_height (height)
    , m_center(pos)
    {
        m_normal = glm::vec3(0.0f);
        m_normalFar = glm::vec3(0.0f);
    }
    virtual ~NonhierCylinder();
    virtual HitInfo intersect(glm::vec3 origin, glm::vec3 &rayDir);
    virtual glm::vec3 getFaceNormal(glm::vec3 intersection);
    virtual glm::vec2 computeTextCoord(glm::vec3 intersection, glm::vec3 normal);
    virtual bool isMesh() { return false; };
private:
    double y_min, y_max, m_radius, m_height;
    glm::vec3 m_center;
    glm::vec3 m_normal;
    glm::vec3 m_normalFar;
};


class Cone: public Primitive {
public:
    Cone();
    virtual ~Cone();
    virtual HitInfo intersect(glm::vec3 origin, glm::vec3 &rayDir);
    virtual glm::vec3 getFaceNormal(glm::vec3 intersection);
    virtual glm::vec2 computeTextCoord(glm::vec3 intersection, glm::vec3 normal);
    virtual bool isMesh() { return false; };
private:
    Primitive *m_nc;
};

class NonhierCone: public Primitive {
public:
    NonhierCone(double radius, double height, const glm::vec3& pos)
    : m_radius (radius)
    , m_height (height) 
    , m_center(pos)
    {
    }
    virtual ~NonhierCone();
    virtual HitInfo intersect(glm::vec3 origin, glm::vec3 &rayDir);
    virtual glm::vec3 getFaceNormal(glm::vec3 intersection);
    virtual glm::vec2 computeTextCoord(glm::vec3 intersection, glm::vec3 normal);
    virtual bool isMesh() { return false; };
private:
    double m_height, m_radius;
    glm::vec3 m_center;
    glm::vec3 m_normalFar;
};

class Sphere : public Primitive {
public:
  Sphere();
  virtual ~Sphere();
  virtual HitInfo intersect(glm::vec3 origin, glm::vec3 &rayDir);
  virtual glm::vec3 getFaceNormal(glm::vec3 intersection);
  virtual glm::vec2 computeTextCoord(glm::vec3 intersection, glm::vec3 normal);
  virtual bool isMesh() { return false; };
private:
    Primitive *m_ns;
};

class Cube : public Primitive {
public:
  Cube();
  virtual ~Cube();
  virtual HitInfo intersect(glm::vec3 origin, glm::vec3 &rayDir);
  virtual glm::vec3 getFaceNormal(glm::vec3 intersection);
  virtual glm::vec2 computeTextCoord(glm::vec3 intersection, glm::vec3 normal);
  virtual bool isMesh() { return false; };
private:
    Primitive *m_nb;
};

class NonhierSphere : public Primitive {
public:
  NonhierSphere(const glm::vec3& pos, double radius)
    : m_pos(pos), m_radius(radius)
  {
  }
  virtual ~NonhierSphere();
  virtual HitInfo intersect(glm::vec3 origin, glm::vec3 &rayDir);
  virtual glm::vec3 getFaceNormal(glm::vec3 intersection);
  virtual glm::vec2 computeTextCoord(glm::vec3 intersection, glm::vec3 normal);
  virtual bool isMesh() { return false; };

private:
  glm::vec3 m_pos;
  double m_radius;
};

class NonhierBox : public Primitive {
public:
  NonhierBox(const glm::vec3& pos, double size)
    : m_pos(pos), m_size(size)
  {
  }
  
  virtual ~NonhierBox();
  virtual HitInfo intersect(glm::vec3 origin, glm::vec3 &rayDir);
  virtual glm::vec3 getFaceNormal(glm::vec3 intersection);
  virtual glm::vec2 computeTextCoord(glm::vec3 intersection, glm::vec3 normal);
  virtual bool isMesh() { return false; };

private:
  glm::vec3 m_pos;
  double m_size;
};
