#include "GeometryNode.hpp"

//---------------------------------------------------------------------------------------
GeometryNode::GeometryNode(
	const std::string & name, Primitive *prim, Material *mat)
	: SceneNode( name )
	, m_material( mat )
	, m_primitive( prim )
{
	m_nodeType = NodeType::GeometryNode;
}

void GeometryNode::setMaterial( Material *mat )
{
	// Obviously, there's a potential memory leak here.  A good solution
	// would be to use some kind of reference counting, as in the 
	// C++ shared_ptr.  But I'm going to punt on that problem here.
	// Why?  Two reasons:
	// (a) In practice we expect the scene to be constructed exactly
	//     once.  There's no reason to believe that materials will be
	//     repeatedly overwritten in a GeometryNode.
	// (b) A ray tracer is a program in which you compute once, and 
	//     throw away all your data.  A memory leak won't build up and
	//     crash the program.

	m_material = mat;
}

HitInfo GeometryNode::traceRay(glm::vec3 &origin, glm::vec3 &rayDir)
{
    HitInfo hitInfo;
    hitInfo.bIsHit = false;
    double tNear = INFINITY;
    
    // Transfrom ray
    glm::vec4 originV4(origin.x, origin.y, origin.z, 1.0f);
    originV4 = invtrans * originV4;
    glm::vec3 transOrigin(originV4.x, originV4.y, originV4.z);
    
    glm::vec4 rayDirV4(rayDir.x, rayDir.y, rayDir.z, 0.0f);
    rayDirV4 = invtrans * rayDirV4;
    glm::vec3 transRayDir(rayDirV4.x, rayDirV4.y, rayDirV4.z);
    
    hitInfo = m_primitive->intersect(transOrigin, transRayDir);
    for (SceneNode * child : children) {
        
        HitInfo childHit = child->traceRay(transOrigin, transRayDir);
        // if hit and closest
        if (childHit.bIsHit && childHit.tNear <= tNear)
        {
            hitInfo = childHit;
            tNear = childHit.tNear;
        }
    }
    
    if (hitInfo.bIsHit) {
        // set material || texture
        hitInfo.hitMaterial = m_material;
        hitInfo.hitMaterialFar = m_material;
    }
        
    // Transform back the point & normal
    glm::vec4 hitPointV4(hitInfo.hitPoint.x, hitInfo.hitPoint.y, hitInfo.hitPoint.z, 1.0f);
    hitPointV4 = trans * hitPointV4;
    hitInfo.hitPoint = glm::vec3(hitPointV4.x, hitPointV4.y, hitPointV4.z);
    
    glm::vec4 normalV4(hitInfo.hitNormal.x, hitInfo.hitNormal.y,
                        hitInfo.hitNormal.z, 0.0);
    normalV4 = glm::transpose(invtrans) * normalV4;
    hitInfo.hitNormal = glm::vec3(normalV4.x, normalV4.y, normalV4.z);
        
    return hitInfo;
}

/* -------------------------------------------------------------------*/

CSGNode::CSGNode(GeometryNode *left, GeometryNode *right, std::string opStr)
: GeometryNode("", nullptr, nullptr)
{
    m_left = left;
    m_right = right;
    isPrimitive = false;
    m_op = opStr;
}

HitInfo CSGNode::traceRay(glm::vec3 &origin, glm::vec3 &rayDir)
{
    HitInfo hitInfo;
    hitInfo.bIsHit = false;
    
    // Transfrom ray
    glm::vec4 originV4(origin.x, origin.y, origin.z, 1.0f);
    originV4 = invtrans * originV4;
    glm::vec3 transOrigin(originV4.x, originV4.y, originV4.z);
    
    glm::vec4 rayDirV4(rayDir.x, rayDir.y, rayDir.z, 0.0f);
    rayDirV4 = invtrans * rayDirV4;
    glm::vec3 transRayDir(rayDirV4.x, rayDirV4.y, rayDirV4.z);
    
    
    std::vector<HitInfo> segList = csgTraceRay(transOrigin, transRayDir);
    if (segList.size() > 0) {
        hitInfo.tNear = INFINITY;
        for (int i = 0; i < segList.size(); i++) {
            if (segList[i].bIsHit && segList[i].tNear < hitInfo.tNear)
                hitInfo = segList[i];
        }
    }
    
    // Transform back the point & normal
    glm::vec4 hitPointV4(hitInfo.hitPoint.x, hitInfo.hitPoint.y, hitInfo.hitPoint.z, 1.0f);
    hitPointV4 = trans * hitPointV4;
    hitInfo.hitPoint = glm::vec3(hitPointV4.x, hitPointV4.y, hitPointV4.z);
    
    glm::vec4 normalV4(hitInfo.hitNormal.x, hitInfo.hitNormal.y,
                       hitInfo.hitNormal.z, 0.0);
    normalV4 = glm::transpose(invtrans) * normalV4;
    hitInfo.hitNormal = glm::vec3(normalV4.x, normalV4.y, normalV4.z);
    
    return hitInfo;
}

std::vector<HitInfo> CSGNode::csgTraceRay(glm::vec3 &origin, glm::vec3 &rayDir)
{
    std::vector<HitInfo> leftList, rightList;
    if (m_left->isPrimitive) {
        HitInfo hitInfo = m_left->traceRay(origin, rayDir);
        leftList.push_back(hitInfo);
    } else {
        CSGNode *csgLeft = static_cast<CSGNode *>(m_left);
        leftList = csgLeft->csgTraceRay(origin, rayDir);
    }
    
    if (m_right->isPrimitive) {
        HitInfo hitInfo = m_right->traceRay(origin, rayDir);
        rightList.push_back(hitInfo);
    } else {
        CSGNode *csgRight = static_cast<CSGNode *>(m_right);
        rightList = csgRight->csgTraceRay(origin, rayDir);
    }
    
    return mergeList(leftList, rightList, m_op);
    
}

std::vector<HitInfo> CSGNode::mergeList(std::vector<HitInfo> leftList, std::vector<HitInfo> rightList,
                               std::string m_op)
{
    // reference from http://slideplayer.com/slide/680289/
    std::vector<HitInfo> newList;
    for (int i=0; i < leftList.size(); i++) {
        for (int j=0; j < rightList.size(); j++) {
            if (m_op == "union")
            {
                if (leftList[i].bIsHit && !rightList[j].bIsHit) {
                    newList.push_back(leftList[i]);
                    continue;
                }
                
                if (!leftList[i].bIsHit && rightList[j].bIsHit) {
                    newList.push_back(rightList[j]);
                    continue;
                }
                
                
                if (leftList[i].bIsHit && rightList[j].bIsHit) {
                    // no-overlap
                    if (leftList[i].tFar < rightList[j].tNear)
                    {
                        newList.push_back(leftList[i]);
                        newList.push_back(rightList[j]);
                    } else {
                        double tNear = std::min(leftList[i].tNear, rightList[j].tNear);
                        double tFar = std::max(leftList[i].tFar, rightList[j].tFar);
                        HitInfo hitInfo;
                    
                        if (tNear == leftList[i].tNear)
                            hitInfo = leftList[i];
                        else
                            hitInfo = rightList[j];
                    
                        if (tFar == leftList[i].tFar) {
                            hitInfo.tFar = tFar;
                            hitInfo.hitPointFar = leftList[i].hitPointFar;
                            hitInfo.hitNormalFar = leftList[i].hitNormalFar;
                            hitInfo.hitUVFar = leftList[i].hitUVFar;
                        
                            hitInfo.hitMaterialFar = leftList[i].hitMaterialFar;
                        }
                        else {
                            hitInfo.tFar = tFar;
                            hitInfo.hitPointFar = rightList[j].hitPointFar;
                            hitInfo.hitNormalFar = rightList[j].hitNormalFar;
                            hitInfo.hitUVFar = rightList[j].hitUVFar;
                            
                            hitInfo.hitMaterialFar = rightList[j].hitMaterialFar;
                        }
                        newList.push_back(hitInfo);
                    }
                }
            }
            
            else if(m_op == "intersection")
            {
                if (leftList[i].bIsHit && rightList[j].bIsHit) {
                    // no-overlap
                    if (leftList[i].tFar < rightList[j].tNear){}
                    else {
                        double tNear = std::max(leftList[i].tNear, rightList[j].tNear);
                        double tFar = std::min(leftList[i].tFar, rightList[j].tFar);
                        HitInfo hitInfo;
                        
                        if (tNear == leftList[i].tNear)
                            hitInfo = leftList[i];
                        else
                            hitInfo = rightList[j];
                        
                        if (tFar == leftList[i].tFar) {
                            hitInfo.tFar = tFar;
                            hitInfo.hitPointFar = leftList[i].hitPointFar;
                            hitInfo.hitNormalFar = leftList[i].hitNormalFar;
                            hitInfo.hitUVFar = leftList[i].hitUVFar;
                            
                            hitInfo.hitMaterialFar = leftList[i].hitMaterialFar;
                        }
                        else {
                            hitInfo.tFar = tFar;
                            hitInfo.hitPointFar = rightList[j].hitPointFar;
                            hitInfo.hitNormalFar = rightList[j].hitNormalFar;
                            hitInfo.hitUVFar = rightList[j].hitUVFar;
                            
                            hitInfo.hitMaterialFar = rightList[j].hitMaterialFar;
                        }
                        newList.push_back(hitInfo);
                    }
                }
                
                
                
            }
            
            else {
                // difference
                if (leftList[i].bIsHit && !rightList[j].bIsHit) {
                    newList.push_back(leftList[i]);
                    continue;
                }
                
                if (!leftList[i].bIsHit && rightList[j].bIsHit) {
                    continue;
                }
                
                if (leftList[i].bIsHit && rightList[j].bIsHit) {
    
                    // no-overlap
                    if (leftList[i].tFar < rightList[j].tNear){
                        newList.push_back(leftList[i]);
                    } // end no-overlap
                    else if (leftList[i].tNear > rightList[j].tNear && leftList[i].tFar < rightList[j].tFar) {
                        HitInfo hitInfo;
                        hitInfo.bIsHit = true;
                        hitInfo.tNear = 0;
                        hitInfo.tFar = 0;
                        hitInfo.hitPoint = glm::vec3(0.0);
                        hitInfo.hitPointFar = glm::vec3(0.0);
                        hitInfo.hitNormal = glm::vec3(0.0);
                        hitInfo.hitNormalFar = glm::vec3(0.0);
                        hitInfo.hitUV = glm::vec2(0.0);  // texture coordinate
                        hitInfo.hitUVFar = glm::vec2(0.0);
                    }
                    
                    else if (leftList[i].tNear < rightList[j].tNear && leftList[i].tFar > rightList[j].tFar) {
                        HitInfo hitInfo;
                        hitInfo = leftList[i];
                        hitInfo.tFar = rightList[j].tNear;
                        hitInfo.hitPointFar = rightList[j].hitPoint;
                        hitInfo.hitNormalFar = rightList[j].hitNormal;
                        hitInfo.hitUVFar = rightList[j].hitUV;
                        
                        hitInfo.hitMaterialFar = rightList[j].hitMaterial;
                        newList.push_back(hitInfo);
                        
                        HitInfo hitInfo1;
                        hitInfo1.bIsHit = true;
                        hitInfo1.tNear = rightList[j].tFar;
                        hitInfo1.hitPoint = rightList[j].hitPointFar;
                        hitInfo1.hitNormal = rightList[j].hitNormalFar;
                        hitInfo1.hitUV = rightList[j].hitUVFar;
                        
                        hitInfo1.hitMaterial = rightList[j].hitMaterialFar;
                        
                        hitInfo1.tFar = leftList[i].tFar;
                        hitInfo1.hitPointFar = leftList[i].hitPointFar;
                        hitInfo1.hitNormalFar = leftList[i].hitNormalFar;
                        hitInfo1.hitUVFar = leftList[i].hitUVFar;
                        
                        hitInfo1.hitMaterialFar = leftList[i].hitMaterialFar;
                        newList.push_back(hitInfo1);
                    }
                    
                    else if (leftList[i].tNear < rightList[j].tNear &&
                             leftList[i].tFar > rightList[j].tNear &&
                             leftList[i].tFar < rightList[j].tFar) {
                        HitInfo hitInfo;
                        hitInfo = leftList[i];
                        hitInfo.tFar = rightList[j].tNear;
                        hitInfo.hitPointFar = rightList[j].hitPoint;
                        hitInfo.hitNormalFar = rightList[j].hitNormal;
                        hitInfo.hitUVFar = rightList[j].hitUV;
                        
                        hitInfo.hitMaterialFar = rightList[j].hitMaterial;
                        
                        newList.push_back(hitInfo);
                    }
                    
                    else if (leftList[i].tNear > rightList[j].tNear && leftList[i].tNear < rightList[j].tFar
                             && leftList[i].tFar > rightList[j].tFar) {
                        HitInfo hitInfo;
                        hitInfo.bIsHit = true;
                        hitInfo.tNear = rightList[j].tFar;
                        hitInfo.hitPoint = rightList[j].hitPointFar;
                        hitInfo.hitNormal = rightList[j].hitNormalFar;
                        hitInfo.hitUV = rightList[j].hitUVFar;
                        
                        hitInfo.hitMaterial = rightList[j].hitMaterialFar;
                        
                        hitInfo.tFar = leftList[i].tFar;
                        hitInfo.hitPointFar = leftList[i].hitPointFar;
                        hitInfo.hitNormalFar = leftList[i].hitNormalFar;
                        hitInfo.hitUVFar = leftList[i].hitUVFar;
                        
                        hitInfo.hitMaterialFar = leftList[i].hitMaterialFar;
                        newList.push_back(hitInfo);
                    }
                }
                
            }
        }
    }
    return newList;
}


