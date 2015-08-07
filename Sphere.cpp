#include "Sphere.h"
#include <GL/glew.h>
#include <GL/gl.h>
#include <cmath>
#include <glm/gtc/swizzle.hpp>


Sphere::Sphere(Scenegraph *graph,string name)
    :Object(graph,name)
{
	float theta,phi;
    int STACKS=100;
	int i,j;
    int SLICES = 100;
    float PI = 3.14159f;
	float cosphi,sinphi,costheta,sintheta;
    VertexAttribs v;

    /*
     *A sphere is drawn using vertices in polar coordinates. Polar coordinates are in terms of the radiu
     *, latitude and longitude. STACKS controls how many latitudes, and SLICES controls how many longitudes
     * are being used to draw this sphere.
     */

    //prepare the vertex data
	for (i=0;i<=STACKS;i++)
	{
		phi = -PI/2 + i*PI/STACKS;
		cosphi = cos(phi);
		sinphi = sin(phi);
		for (j=0;j<=SLICES;j++)
		{
			theta = 2*j*PI/SLICES;
			costheta = cos(theta);
			sintheta = sin(theta);

            v.position[0] = cosphi*costheta;
            v.position[1] = sinphi;
            v.position[2] = -cosphi*sintheta;
            v.position[3] = 1;

            v.normal[0] = cosphi*costheta;
            v.normal[1] = sinphi;
            v.normal[2] = -cosphi*sintheta;
            v.normal[3] = 0;

            v.texcoords[0] = theta/(2*PI);
            v.texcoords[1] = (phi+0.5f*PI)/PI;

            vertexData.push_back(v);
		}
	}

    //now prepare the triangle index list
    //this is simple enough. Just imagine drawing each quad in the sphere as two triangles
    //triangle 1: (i,j), (i,j+1) and (i+1,j+1)
    //triangle 2: (i,j), (i+1,j+1) and (i+1,j)
    //It is a good habit to specify all triangles in counter-clockwise order as OpenGL uses by default the order
    //to determine front-facing vs. back-facing if culling is enabled


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

bool Sphere::intersect(Ray& r,HitRecord& h,stack<glm::mat4>& modelview)
{
    bool result;
    glm::mat4 objToView,viewToObj;

    result = false;
    modelview.push(modelview.top());
    modelview.top() = modelview.top() * animation * transform;

    objToView = modelview.top();
    viewToObj = glm::inverse(objToView);

    Ray objectRay;

    objectRay.start = viewToObj * r.start;
    objectRay.dir = viewToObj * r.dir;

    float phi, theta;
    float texS = 0;
    float texT = 0;
    double a,b,c;
    double pi = 3.141592;



    a = glm::dot(objectRay.dir,objectRay.dir);
    b = 2*glm::dot(objectRay.dir,objectRay.start);
    c = glm::dot(glm::vec3(glm::swizzle<glm::X,glm::Y,glm::Z>(objectRay.start)),glm::vec3(glm::swizzle<glm::X,glm::Y,glm::Z>(objectRay.start))) - 1;

    double disc = b*b - 4*a*c;

    if (disc<0)
    {
        result = false;
    }
    else
    {
        float t1,t2,t;

        t1 = (-b+sqrt(disc))/(2*a);
        t2 = (-b-sqrt(disc))/(2*a);

        if (t1>t2)
        {
            float temp = t1;
            t1 = t2;
            t2 = temp;
        }

        //now always t1<t2

        if (t2<0) //both of them are negative
        {
            result = false;
        }
        else
        {
            result = true;
            if (t1<0)
            {
                t = t2;
            }
            else
            {
                t = t1;
            }

            if ((h.time<0) || (h.time>t))
            {
                glm::vec4 ptIntersect;

                ptIntersect = objectRay.start + objectRay.dir*t;

                h.time = t;
                h.point = objToView * ptIntersect;
                h.normal = glm::transpose(viewToObj) * glm::vec4(ptIntersect.x,ptIntersect.y,ptIntersect.z,0);
                h.normal = glm::normalize(h.normal);
                h.mat = material;

                h.tex = this->getTexture();

                //bound x and y to be bewtween 1 and -1
                if(ptIntersect.x > 1)
                {
                    ptIntersect.x = 1;
                }
                else if(ptIntersect.x < -1)
                {
                    ptIntersect.x = -1;
                }

                if(ptIntersect.y > 1)
                {
                    ptIntersect.y = 1;
                }
                else if(ptIntersect.y < -1)
                {
                    ptIntersect.y = -1;
                }

                //y = sin(theta), use to find theta
                theta = asin(fabs(ptIntersect.y));

                //other side of sphere

                if(ptIntersect.y < 0 )
                {
                    theta = -theta;
                }


                phi = acos(fabs(ptIntersect.x) / cos(theta));

                if(ptIntersect.z < 0)
                {
                    if(ptIntersect.x < 0)
                    {
                        phi = pi - phi;
                    }
                }
                else if(ptIntersect.x < 0)
                {
                    phi = pi + phi;
                }
                else
                    phi = 2*pi - phi;

                texS = phi / (2*pi);
                texT = 1-((theta + (pi/2)) / pi);

                h.textCoords = glm::vec4(texS,texT,0,1);
            }
        }
    }

    modelview.pop();
    return result;
}

Sphere::~Sphere(void)
{
}

