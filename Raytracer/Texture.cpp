//
//  Texture.cpp
//  A4
//
//  Created by Jessica's Mac on 2017-11-19.
//  Copyright © 2017 none. All rights reserved.
//

#include "Texture.hpp"
using namespace std;

Texture::Texture(const std::string& fname,
                 const glm::vec3& ks, double shininess,
                 double reflectivity,
                 double transparencyCoeff, double refractionIdx)
: m_ks(ks)
, m_shininess(shininess)
, m_reflectivity(reflectivity)
, m_transparencyCoeff(transparencyCoeff)
, m_refractionIdx(refractionIdx)
, m_bLoadingSuccess(false)
{
    m_kd = glm::vec3(0.0);
    if (fname == "" || !loadPng(fname, m_imagePixel)) {
        m_bLoadingSuccess = false;
        cerr << "Texture" << " " << fname<< " loading failed." << endl;
    }
    m_bLoadingSuccess = true;
}

Texture::~Texture()
{
    m_imagePixel.clear();
}

glm::vec3 Texture::getKd(glm::vec2 &textCoord)
{
    if (!m_bLoadingSuccess)
        return m_kd;
  
    // bilinear interpolation reference http://www.cs.uu.nl/docs/vakken/gr/2011/Slides/06-texturing.pdf
    
    double u = textCoord.x;
    double v = textCoord.y;

    uint i = int(m_width  * u);
    uint i1 = int(m_width  * u) + 1;
    uint j = int(m_height * v);
    uint j1 = int(m_height * v) + 1;
    
    // clamp
    i = std::min(std::max(i, (uint)0), m_width - 1 );
    i1 = std::min(std::max(i1, (uint)0), m_width -1 );
    j = std::min(std::max(j, (uint)0), m_height-1);
    j1 = std::min(std::max(j1, (uint)0), m_height - 1);
    
    glm::vec3 colorIJ = mapColor(i, j);
    glm::vec3 colorI1J = mapColor(i1, j);
    glm::vec3 colorIJ1 = mapColor(i, j1);
    glm::vec3 colorI1J1 = mapColor(i1, j1);
    
    double udash = u * m_width - i;
    double vdash = v * m_height - j;
    
    glm::vec3 result = (float) (1.0 - udash) * (float)(1.0 - vdash) * colorIJ +
                        (float)udash * (float)(1.0 - vdash) * colorI1J +
                        (float)(1.0 - udash) * (float)vdash * colorIJ1 +
                        (float)udash * (float)vdash * colorI1J1;
    
    return result;
}

glm::vec3 Texture::mapColor(int x, int y)
{
    glm::vec3 result((double) (m_imagePixel[m_colorComponents * (m_width * y + x) + 0] / 255.0),
                     (double) (m_imagePixel[m_colorComponents * (m_width * y + x) + 1] / 255.0),
                     (double) (m_imagePixel[m_colorComponents * (m_width * y + x) + 2] / 255.0));
    
    return result;
}



