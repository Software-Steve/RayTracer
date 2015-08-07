#ifndef OBJECT_H_
#define OBJECT_H_


/*
 *This class is the base class for all the types of 3D objects that we will draw
 *
 *Individual objects will actually create the vertex attributes and set up the VBOs and VAO.
 *
 *This class just provides them with a VAO object, and provides the drawing code for all objects
 */

#include <GL/glew.h>
#include <GL/gl.h>
#include <vector>
#include <string>
using namespace std;
#include "Node.h"
#include "Scenegraph.h"
#include "Material.h"
#include "Texture.h"
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#define BUFFER_OFFSET(offset) ((void *)(offset))

//a class that stores all the data relevant to a vertex
class VertexAttribs
{
public:
    //position of a vertex, as x,y,z,w with w being 1 always
    float position[4];
    //direction of the normal vector, as x,y,z,w with w being 0 always
    float normal[4];
    //texture coordinates for the vertex as s,t.
    float texcoords[2];
};


class Object: public Node
{
protected:
    /*
     *the different kinds of vertex buffer objects (VBOs). We need two: one for vertex data and one for
     *the triangle indices
     */
    enum Buffer_IDs {VertexBuffer,IndexBuffer,NumBuffers};
    /*
     *As corresponding to the VertexAttribs structure above and the shader input variables, we have
     *three attributes per vertex. The values of the attributes must match the layout specified in the
     *shader.
     */
    enum Attrib_IDs {vPosition=0,vNormal=1,vTexCoord=2};
protected:
    //the VAO corresponding to this object. Consider this as a proxy for rendering this object.
    GLuint VAO;
    //the VBO IDS required for this object
    GLuint buffers[NumBuffers];
    //the vertex data. If you change any of this, you must re-send it to the GPU for it to affect the drawing
    vector<VertexAttribs> vertexData;
    //the triangle indices that index into the above vertexdata array
    vector<GLuint> triangleIndices;

    //each object is assumed to have a material
    Material material;

    //each object is assumed to have a texture
    Texture *texture;

    //texture transformation
    glm::mat4 textureTransform;

    //number of indices
    int numIndices;

    //a unique "name" for this object
    string name;

public:
    Object(Scenegraph *graph,string name="")
        :Node(graph,name)
    {
        textureTransform = glm::mat4(1.0);
        texture = NULL;
        //create the VAO ID for this object
        glGenVertexArrays(1,&VAO);
        //bind the VAO
        glBindVertexArray(VAO);
        //create as many VBO IDs as you need, in this case 2
        glGenBuffers(NumBuffers,buffers);
	}

    /*
     *The destructor is virtual so that any extra memory allocated by subclasses can be freed
     *in their respective destructors
     */
	virtual ~Object(void)
	{
        if (VAO!=0)
        {
            //give back the VBO IDs to OpenGL, so that they can be reused
            glDeleteBuffers(NumBuffers,buffers);
            //give back the VAO ID to OpenGL, so that it can be reused
            glDeleteVertexArrays(1,&VAO);
        }
        texture = NULL;
	}

    virtual void getLights(vector<Light> &lightvector, stack<glm::mat4> &modelView)
    {
        modelView.push(modelView.top());
        modelView.top() = modelView.top() * animation * transform;
        for (int i=0;i<lights.size();i++)
        {
            lightvector.push_back(lights[i]);
            glm::vec4 l = lights[i].getPosition();
            l = modelView.top() * l;
            lightvector[lightvector.size()-1].setPosition(l);

            if (lights[i].getSpotAngle()<180)
            {
                glm::vec4 l = this->lights[i].getSpotDirection();
                l = modelView.top() * l;
                lightvector[lightvector.size()-1].setSpotDirection(l);
            }
        }
        modelView.pop();
    }

    virtual void draw(stack<glm::mat4>& modelView)
    {
      /*  //The total transformation is whatever was passed to it, with its own transformation
        glUniformMatrix4fv(transformLocation,1,GL_FALSE,glm::value_ptr(t * animation * objToWorld));
        //set the color for all vertices to be drawn for this object
        glUniform3fv(colorLocation,1,glm::value_ptr(color));
        //draw the object
*/
        //1. bind its VAO

        modelView.push(modelView.top());
        modelView.top() = modelView.top() * animation * transform;

        if (scenegraph!=NULL)
        {
            glUniformMatrix4fv(scenegraph->modelviewLocation,1,GL_FALSE,glm::value_ptr(modelView.top()));
            glUniformMatrix4fv(scenegraph->normalmatrixLocation,1,GL_FALSE,glm::value_ptr(glm::transpose(glm::inverse(modelView.top()))));
            glUniformMatrix4fv(scenegraph->textureMatrixLocation,1,GL_FALSE,glm::value_ptr(textureTransform));

            //put the material in the material properties
            glUniform3fv(scenegraph->materialLocations.ambientLocation,1,glm::value_ptr(material.getAmbient()));
            glUniform3fv(scenegraph->materialLocations.diffuseLocation,1,glm::value_ptr(material.getDiffuse()));
            glUniform3fv(scenegraph->materialLocations.specularLocation,1,glm::value_ptr(material.getSpecular()));
            glUniform1f(scenegraph->materialLocations.shininessLocation,material.getShininess());
        }

        if (texture!=NULL)
            glBindTexture(GL_TEXTURE_2D,texture->getTextureID());


        glBindVertexArray(VAO);



        //2. execute the "superpower" command
        glDrawElements(GL_TRIANGLES,numIndices,GL_UNSIGNED_INT,BUFFER_OFFSET(0));
        modelView.pop();
    }

    /*
     *Set the color of each vertex in this object
     */
    virtual void setMaterial(Material& m)
    {
        material = m;
    }

    Material getMaterial()
    {
        return material;
    }

    Texture *getTexture()
    {
        return texture;
    }

    void setTexture(Texture *texture)
    {
        this->texture = texture;
    }

    void setTextureTransform(glm::mat4& mat)
    {
        textureTransform = mat;
    }

};

#endif //OBJECT_H_
