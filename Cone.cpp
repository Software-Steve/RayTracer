#include "Cone.h"
#include <cmath>

Cone::Cone(Scenegraph *graph,string name)
    :Object(graph,name)
{
	// TODO Auto-generated constructor stub
	float theta;
    int STACKS=5;
	int i,j;
    int SLICES = 50;
    float PI = 3.14159f,y;
	float costheta,sintheta;
	float radius;

    /*
     *A cone is very similar to a cylinder, except the top face is a point.
     *STACKS measures the divisions along its axis, and SLICES measures the number of slices on its
     *circular face
     */

    VertexAttribs v;

    //prepare the vertex data

	for (i=0;i<=STACKS;i++)
	{
		y = (float)i/STACKS;
        radius = 1.0f - y;
		for (j=0;j<=SLICES;j++)
		{
			theta = 2*j*PI/SLICES;
			costheta = cos(theta);
			sintheta = sin(theta);

            v.position[0] = radius*costheta;
            v.position[1] = y;
            v.position[2] = radius*sintheta;
            v.position[3] = 1;

            v.normal[0] = costheta/sqrt(2.0f);
            v.normal[1] = 1.0f/sqrt(2.0f);
            v.normal[2] = sintheta/sqrt(2.0f);
            v.normal[3] = 0;

            v.texcoords[0] = theta/(2*PI);
            v.texcoords[1] = y;
            vertexData.push_back(v);
		}
	}

    //the bottom circular face
    for (j=0;j<=SLICES;j++)
    {
        theta = 2*j*PI/SLICES;
        costheta = cos(theta);
        sintheta = sin(theta);

        v.position[0] = costheta;
        v.position[1] = 0;
        v.position[2] = sintheta;
        v.position[3] = 1;

        v.normal[0] = 0;
        v.normal[1] = -1;
        v.normal[2] = 0;
        v.normal[3] = 0;

        v.texcoords[0] = theta/(2*PI);
        v.texcoords[1] = 0;

        vertexData.push_back(v);
    }

    //the center of the circular face, useful to draw that face
    v.position[0] = 0;
    v.position[1] = 0;
    v.position[2] = 0;
    v.position[3] = 1;

    v.normal[0] = 0;
    v.normal[1] = -1;
    v.normal[2] = 0;
    v.normal[3] = 0;

    v.texcoords[0] = 0;
    v.texcoords[1] = 1;

    vertexData.push_back(v);

    //now prepare the triangle index list
    //this is simple enough. Just imagine drawing each quad in the sphere as two triangles
    //triangle 1: (i,j), (i,j+1) and (i+1,j+1)
    //triangle 2: (i,j), (i+1,j+1) and (i+1,j)
    //It is a good habit to specify all triangles in counter-clockwise order as OpenGL uses by default the order
    //to determine front-facing vs. back-facing if culling is enabled

    //the conical surface
    for (i=0;i<STACKS;i++)
    {
        for (j=0;j<SLICES;j++)
        {
            triangleIndices.push_back(i*(SLICES+1)+j);
            triangleIndices.push_back(i*(SLICES+1)+j+1);
            triangleIndices.push_back((i+1)*(SLICES+1)+j+1);

            triangleIndices.push_back(i*(SLICES+1)+j);
            triangleIndices.push_back((i+1)*(SLICES+1)+j+1);
            triangleIndices.push_back((i+1)*(SLICES+1)+j);

        }
    }

    //the bottom circular face
    for (j=0;j<SLICES;j++)
    {
        triangleIndices.push_back(vertexData.size()-1);
        triangleIndices.push_back((STACKS+1)*(SLICES+1) + j);
        triangleIndices.push_back((STACKS+1)*(SLICES+1) + j+1);
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



Cone::~Cone() {
	// TODO Auto-generated destructor stub
}
