#ifndef BOX_H
#define BOX_H

#include "Object.h"

//a class that represents a box centered at origin with side 1

class Box : public Object
{
public:
    Box(Scenegraph *graph,string name="");
    virtual ~Box();
};

#endif // BOX_H
