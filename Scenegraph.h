#ifndef SCENEGRAPH_H
#define SCENEGRAPH_H

#include "Node.h"
#include "Texture.h"
#include <map>
#include <vector>
using namespace std;
#include <glm/glm.hpp>
#include <GL/glew.h>
#include <GL/gl.h>
#include <stack>
#include <map>
using namespace std;

class Scenegraph
{

    friend class Object;

    typedef struct
    {
        GLint positionLocation;
        GLint ambientLocation;
        GLint diffuseLocation;
        GLint specularLocation;
        GLint spotDirectionLocation;
        GLint spotCutoffLocation;
    } LightLocation;


    typedef struct
    {
        GLint ambientLocation;
        GLint diffuseLocation;
        GLint specularLocation;
        GLint shininessLocation;
    } MaterialLocation;

public:
    Scenegraph();
    ~Scenegraph();
    void makeScenegraph(Node *root);
    void initShaderProgram(GLint shaderProgram);
    void draw(stack<glm::mat4>& modelView);
    /*void draw(glm::mat4 worldToView);*/
    void animate(float t);
    void enableLights(stack<glm::mat4>& modelView);
    void addTexture(Texture *texture);

    //ray tracer functions
    GLfloat *raytrace(int width,int height,stack<glm::mat4>& modelview);
    bool raycast(Ray& r,stack<glm::mat4>& modelview,glm::vec3& color, bool hit, int bounce);
    glm::vec3 shade(HitRecord& h, stack<glm::mat4> & modelview);

private:
    Node *root;
    map<string,Node *> nodemap;
    vector<Light> lights;
    GLint modelviewLocation,normalmatrixLocation,textureMatrixLocation;
    MaterialLocation materialLocations;
    LightLocation lightLocation[10];
    GLint numLightsLocation;
    GLint textureLocation;

    GLuint materialBO;
    vector<Texture *> textures;
};

#endif // SCENEGRAPH_H
