//
//  Texture.hpp
//  A4
//
//  Created by Jessica's Mac on 2017-11-19.
//  Copyright © 2017 none. All rights reserved.
//

#pragma once

#include <glm/glm.hpp>
#include <stdio.h>
#include <iostream>
#include <vector>
#include "Image.hpp"
#include "Material.hpp"

class Texture: public Image, public Material {
public:
    Texture(const std::string &fname,
            const glm::vec3& ks, double shininess, double reflectivity,
            double transparencyCoeff, double refractionIdx);
    
    virtual ~Texture();
    virtual glm::vec3 getKd(glm::vec2 &textCoord);
    virtual glm::vec3 getKs() { return m_ks; }
    virtual double getShininess() { return m_shininess; }
    virtual double getReflectivity() { return m_reflectivity; }
    virtual double getTransparencyCoeff() { return m_transparencyCoeff; }
    virtual double getRefractionIdx() { return m_refractionIdx; }
    virtual bool isTexture() { return m_bLoadingSuccess; }
    
    bool m_bLoadingSuccess;
    
private:
    glm::vec3 mapColor(int x, int y);
    
    glm::vec3 m_kd;
    glm::vec3 m_ks;
    double m_shininess;
    double m_reflectivity;
    double m_transparencyCoeff;
    double m_refractionIdx;
    std::vector<unsigned char> m_imagePixel;
    bool m_bIsCubeText;
};
