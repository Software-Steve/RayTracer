#ifndef SPHERE_H_
#define SPHERE_H_
#include "Object.h"


//represents a sphere of unit radius centered at the origin
class Sphere :public Object
{
public:
    Sphere(Scenegraph *graph,string name="");
    bool intersect(Ray& r,HitRecord& h,stack<glm::mat4>& modelview);
    ~Sphere(void);
};

#endif
