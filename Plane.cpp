#include "Plane.h"
#include <GL/glew.h>
#include <GL/gl.h>
#include <cmath>
#include <glm/gtc/matrix_inverse.hpp>

#define max(A,B) A>B?A:B

Plane::Plane(Scenegraph *graph,string name)
    :Object(graph,name)
{
    int i,j;
	int DIM;
    VertexAttribs v;

    /*
     *A plane can be drawn using two triangles. It can also be divided into grids and then drawn
     *DIM controls the size of the grid
     */
    DIM = 1;

    //prepare the vertex data
    for (i=0;i<DIM+1;i++)
    {
        for (j=0;j<DIM+1;j++)
        {
            //the position such that x and z are (-0.5,0.5) and y=0
            v.position[0] = -0.5f+(1.0f*j/DIM);
            v.position[1] = 0.0f;
            v.position[2] = 0.5f - (1.0f*i/DIM);
            v.position[3] = 1.0f; //always 1

            //the normal of such a plane is always the +y axis, since this plane is on the X-Z plane
            v.normal[0] = 0.0f;
            v.normal[1] = 1.0f;
            v.normal[2] = 0.0f;
            v.normal[3] = 0.0f;

            //texture coordinates go from 0 to 1

            v.texcoords[0] = (float)j/DIM;
            v.texcoords[1] = (float)i/DIM;

            vertexData.push_back(v);

        }
    }

    //now prepare the triangle index list
    //this is simple enough. Just imagine drawing each quad in the grid as two triangles
    //triangle 1: (i,j), (i,j+1) and (i+1,j+1)
    //triangle 2: (i,j), (i+1,j+1) and (i+1,j)
    //It is a good habit to specify all triangles in counter-clockwise order as OpenGL uses by default the order
    //to determine front-facing vs. back-facing if culling is enabled
    for (i=0;i<DIM;i++)
    {
        for (j=0;j<DIM;j++)
        {
            triangleIndices.push_back(i*(DIM+1)+j);
            triangleIndices.push_back(i*(DIM+1)+j+1);
            triangleIndices.push_back((i+1)*(DIM+1)+j+1);

            triangleIndices.push_back(i*(DIM+1)+j);
            triangleIndices.push_back((i+1)*(DIM+1)+j+1);
            triangleIndices.push_back((i+1)*(DIM+1)+j);
        }
    }


    /*
     *Bind the VAO as the current VAO, so that all subsequent commands affect it
     */
    glBindVertexArray(VAO);

    /*
     *Allocate the VBO for vertex data and send it to the GPU
     */
    glBindBuffer(GL_ARRAY_BUFFER,buffers[VertexBuffer]);
    glBufferData(GL_ARRAY_BUFFER,sizeof(VertexAttribs)*vertexData.size(),&vertexData[0],GL_STATIC_DRAW);

    /*
     *Allocate the VBO for triangle indices and send it to GPU
     */
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffers[IndexBuffer]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint)*triangleIndices.size(), &triangleIndices[0], GL_STATIC_DRAW);


    /*
     *Specify all the vertex attribute pointers, i.e. tell OpenGL how to organize data according to attributes rather than vertices
     */

    //first enable the correct VBO, since the "current" is the VBO for triangle indices
    glBindBuffer(GL_ARRAY_BUFFER,buffers[VertexBuffer]);

    //VertexData starts with position, so starting byte is 0
    glVertexAttribPointer(vPosition,4,GL_FLOAT,GL_FALSE,sizeof(VertexAttribs),BUFFER_OFFSET(0));
    //In VertexData, normal follows the position (4 floats), so start reading normals from 4*sizeof(float)
    glVertexAttribPointer(vNormal,4,GL_FLOAT,GL_FALSE,sizeof(VertexAttribs),BUFFER_OFFSET(4*sizeof(float)));
    //In VertexData, texture coordinates follow the position and normal (8 floats), so start reading texture coordinates from 8*sizeof(float)
    glVertexAttribPointer(vTexCoord,2,GL_FLOAT,GL_FALSE,sizeof(VertexAttribs),BUFFER_OFFSET(8*sizeof(float)));

    //enable the vertex attribute arrays
    glEnableVertexAttribArray(vPosition);
    glEnableVertexAttribArray(vNormal);
    glEnableVertexAttribArray(vTexCoord);

    /*
     *Unbind the VAO to prevent accidental change to all the settings
     *so at this point, this VAO has two VBOs and two enabled VertexAttribPointers.
     * It is going to remember all of that!
     */
    glBindVertexArray(0);

    numIndices = triangleIndices.size();
    triangleIndices.clear();
    vertexData.clear();
}

bool Plane::intersect(Ray& r,HitRecord& h,stack<glm::mat4>& modelview)
{
    bool result;
    glm::mat4 objToView,viewToObj;
    float texS;
    float texT;



    result = false;
    modelview.push(modelview.top());
    modelview.top() = modelview.top() * animation * transform;

    objToView = modelview.top();
    viewToObj = glm::inverse(objToView);

    Ray objectRay;

    objectRay.start = viewToObj * r.start;
    objectRay.dir = viewToObj * r.dir;

    if (fabs(objectRay.dir.y)<0.0001) //ray parallel to the plane
    {
        result = false;
    }
    else
    {
        float t = -objectRay.start.y/objectRay.dir.y;

        if (t<0)
        {
            result = false;
        }
        else
        {
            glm::vec4 ptIntersect;

            ptIntersect = objectRay.start + objectRay.dir*t;

            if ((ptIntersect.x<-0.5) || (ptIntersect.x>0.5) || (ptIntersect.z<-0.5) || (ptIntersect.z>0.5))
            {
                result = false;
            }
            else
            {
                result = true;
                if ((h.time<0) || (h.time>t))
                {
                    h.time = t;
                    h.point = objToView * ptIntersect;
                    h.normal = glm::transpose(viewToObj) * glm::vec4(0,1,0,0);
                    h.normal = glm::normalize(h.normal);
                    h.mat = material;

                    h.tex = this->getTexture();

                    texS = 0.5 + ptIntersect.x;
                    texT = 1 - (0.5 + ptIntersect.z);

                    h.textCoords = glm::vec4(texS, texT,0,1);

                }
            }
        }
    }
    modelview.pop();
    return result;
}

Plane::~Plane(void)
{
}

