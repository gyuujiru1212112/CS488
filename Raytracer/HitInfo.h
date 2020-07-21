//
//  HitInfo.h
//  A4
//
//  Created by Jessica's Mac on 2017-11-24.
//  Copyright © 2017 none. All rights reserved.
//

#pragma once
#include "Material.hpp"

struct HitInfo {
    bool bIsHit = false;
    double tNear = INFINITY;
    double tFar = INFINITY;
    glm::vec3 hitPoint;
    glm::vec3 hitPointFar;
    glm::vec3 hitNormal;
    glm::vec3 hitNormalFar;
    glm::vec2 hitUV;  // texture coordinate
    glm::vec2 hitUVFar;
    
    
    Material *hitMaterial;
    Material *hitMaterialFar;
};
