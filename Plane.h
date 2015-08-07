#ifndef PLANE_H_
#define PLANE_H_

#include "Object.h"

//represents a plane of side 1 lying on the X-Z plane centered at the origin
class Plane : public Object
{
public:
    Plane(Scenegraph *graph,string name="");
    bool intersect(Ray& r,HitRecord& h,stack<glm::mat4>& modelview);
    ~Plane(void);
};

#endif
