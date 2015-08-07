#ifndef CYLINDER_H_
#define CYLINDER_H_

#include "Object.h"

//represents a cylinder of unit radius and inner face of supplied radius centered at the origin standing with its lower face on the X-Z plane and axis along the +Y axis

class Cylinder: public Object {
public:
    Cylinder(Scenegraph *graph,string name="",float inner_radius=0.0f);
    ~Cylinder(void);
};

#endif /* CYLINDER_H_ */
