#include "Box.h"
#include <GL/glew.h>
#include <GL/gl.h>
#include <cmath>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>

#define max(A,B) A>B?A:B

Box::Box(Scenegraph *graph,string name)
    :Object(graph,name)
{
    int i,j,k,l;
    int DIM;
    glm::mat4 faceTransforms[6];
    glm::mat4 identity = glm::mat4(1.0f);
    VertexAttribs v;
    glm::mat4 transform,inverseTranspose;

    //set the 6 face transforms
    faceTransforms[0] = glm::translate(identity,glm::vec3(-0.5f,0.0f,0.0f)) * glm::rotate(identity,90.0f,glm::vec3(1,0,0)) * glm::rotate(identity,90.0f,glm::vec3(0,0,1));
    faceTransforms[1] = glm::translate(identity,glm::vec3(0.5f,0.0f,0.0f)) * glm::rotate(identity,90.0f,glm::vec3(1,0,0)) * glm::rotate(identity,-90.0f,glm::vec3(0,0,1));
    faceTransforms[2] = glm::translate(identity,glm::vec3(0.0f,0.0f,-0.5f)) * glm::rotate(identity,-90.0f,glm::vec3(1,0,0));
    faceTransforms[3] = glm::translate(identity,glm::vec3(0.0f,0.0f,0.5f)) * glm::rotate(identity,90.0f,glm::vec3(1,0,0));
    faceTransforms[4] = glm::translate(identity,glm::vec3(0.0f,-0.5f,0.0f)) * glm::rotate(identity,180.0f,glm::vec3(0,0,1));
    faceTransforms[5] = glm::translate(identity,glm::vec3(0.0f,0.5f,0.0f));

    /*
     *A plane can be drawn using two triangles. It can also be divided into grids and then drawn
     *DIM controls the size of the grid
     */
    DIM = 1;

    for (k=0;k<6;k++)
    {
        transform = faceTransforms[k];
        inverseTranspose = glm::inverseTranspose(transform);
        //prepare the vertex data
        for (i=0;i<DIM+1;i++)
        {
            for (j=0;j<DIM+1;j++)
            {
                glm::vec4 position,normal;

                //the position such that x and z are (-0.5,0.5) and y=0
                position = glm::vec4(-0.5f+(1.0f*j/DIM),0.0f,-0.5f + (1.0f*i/DIM),1.0f);
                normal = glm::vec4(0.0f,1.0f,0.0f,0.0f);

                //transform by the kth face transform
                position = transform * position;
                normal = glm::normalize(inverseTranspose * normal);

                //put in v
                for (l=0;l<4;l++)
                {
                    v.position[l] = position[l];
                    v.normal[l] = normal[l];
                }

                //texture coordinates go from 0 to 1

                v.texcoords[0] = (float)i/DIM;
                v.texcoords[1] = (float)j/DIM;

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
                triangleIndices.push_back(k*(DIM+1)*(DIM+1) + i*(DIM+1)+j);
                triangleIndices.push_back(k*(DIM+1)*(DIM+1) + i*(DIM+1)+j+1);
                triangleIndices.push_back(k*(DIM+1)*(DIM+1) + (i+1)*(DIM+1)+j+1);

                triangleIndices.push_back(k*(DIM+1)*(DIM+1) + i*(DIM+1)+j);
                triangleIndices.push_back(k*(DIM+1)*(DIM+1) + (i+1)*(DIM+1)+j+1);
                triangleIndices.push_back(k*(DIM+1)*(DIM+1) + (i+1)*(DIM+1)+j);
            }
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

Box::~Box(void)
{
}


