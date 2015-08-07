#ifndef NODE_H
#define NODE_H

#include <string>
#include <map>
#include <stack>
#include <vector>
using namespace std;
#include <glm/glm.hpp>
#include "Light.h"
#include "HitRecord.h"
#include "Ray.h"

class Scenegraph;

class Node
{
protected:
    string name;
    glm::mat4 transform,animation;
    Scenegraph *scenegraph;
    vector<Light> lights;

public:
    Node(Scenegraph *graph,string name="")
    {
        scenegraph = graph;
        transform = glm::mat4(1.0);
        animation = glm::mat4(1.0);
        setName(name);
    }

    virtual ~Node()
    {     
        lights.clear();
    }

    virtual void draw(stack<glm::mat4>& modelView)=0;

    virtual bool intersect(Ray& r,HitRecord& h,stack<glm::mat4>& modelview)
    {
        return false;
    }

    virtual void makeNodemap(map<string,Node *>& nodemap)
    {
        if (name != "")
        {
            nodemap[name] = this;
        }
    }

    virtual void getLights(vector<Light>& lights,stack<glm::mat4>& modelView)=0;



    void setTransform(glm::mat4 obj)
    {
        transform = obj;
    }

    void setAnimationTransform(glm::mat4 mat)
    {
        animation = mat;
    }

    glm::mat4 getTransform()
    {
        return transform;
    }

    glm::mat4 getAnimationTransform()
    {
        return animation;
    }

    void setName(const string& name)
    {
        this->name = name;
    }

    void addLight(Light& l)
    {
        lights.push_back(l);
    }
};

#endif // NODE_H
