#ifndef GROUP_H
#define GROUP_H

#include <GL/gl.h>
#include <vector>
using namespace std;
#include "Node.h"

class Group: public Node
{
protected:
    vector<Node *> children;

public:
    Group(Scenegraph *graph,string name="")
        :Node(graph,name)
    {

    }

    ~Group()
    {
        for (unsigned int i=0;i<children.size();i++)
            delete children[i];
    }

    virtual void getLights(vector<Light> &lightvector, stack<glm::mat4> &modelView)
    {
        modelView.push(modelView.top());
        modelView.top() = modelView.top() * animation * transform;
        for (int i=0;i<lights.size();i++)
        {
            lightvector.push_back(lights[i]);
            glm::vec4 l = this->lights[i].getPosition();
            l = modelView.top() * l;
            lightvector[lightvector.size()-1].setPosition(l);

            if (lights[i].getSpotAngle()<180)
            {
                glm::vec4 l = this->lights[i].getSpotDirection();
                l = modelView.top() * l;
                lightvector[lightvector.size()-1].setSpotDirection(l);
            }
        }

        for (int i=0;i<children.size();i++)
        {
            children[i]->getLights(lightvector,modelView);
        }
        modelView.pop();
    }

    virtual void draw(stack<glm::mat4> &modelView)
    {
        modelView.push(modelView.top());
        modelView.top() = modelView.top() * animation * transform;
        for (int i=0;i<children.size();i++)
        {
            children[i]->draw(modelView);
        }
        modelView.pop();
    }

    virtual bool intersect(Ray& r,HitRecord& h,stack<glm::mat4>& modelview)
    {
        bool result;

        result = false;
        modelview.push(modelview.top());
        modelview.top() = modelview.top() * animation * transform;
        for (int i=0;i<children.size();i++)
        {
            bool tempresult;
            tempresult = children[i]->intersect(r,h,modelview);
            result = result || tempresult;
        }
        modelview.pop();
        return result;
    }

    void addChild(Node *child)
    {
        children.push_back(child);
    }

    const vector<Node *> getChildren() const
    {
        return children;
    }


    void makeNodemap(map<string,Node *>& nodemap)
    {
        Node::makeNodemap(nodemap);
        for (unsigned int i=0;i<children.size();i++)
            children[i]->makeNodemap(nodemap);
    }
};

#endif // GROUP_H
