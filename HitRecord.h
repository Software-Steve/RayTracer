#ifndef HITRECORD_H
#define HITRECORD_H

#include <glm/glm.hpp>
#include "Material.h"
#include "Texture.h"

class HitRecord
{
public:
    float time;
    glm::vec4 point;
    glm::vec4 normal;
    Material mat;
    Texture * tex;
    glm::vec4 textCoords;//(s,t,0,1)
};

#endif // HITRECORD_H
