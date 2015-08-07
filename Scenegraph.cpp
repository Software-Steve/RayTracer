#include "Scenegraph.h"
#include "Group.h"
#include "Object.h"
#include "Material.h"
#include <stack>
#include <sstream>
#include <iostream>
#include <algorithm>
using namespace std;
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/swizzle.hpp>



Scenegraph::Scenegraph()
{
    root = NULL;
}

void Scenegraph::makeScenegraph(Node *r)
{
    if (root!=NULL)
    {
        delete root;
        nodemap.clear();
        root = NULL;
    }
    this->root = r;
    root->makeNodemap(nodemap);
}

Scenegraph::~Scenegraph()
{
    if (root!=NULL)
    {
        delete root;
        nodemap.clear();
        root = NULL;
    }

    for (int i=0;i<textures.size();i++)
    {
        delete textures[i];
    }
}

GLfloat *Scenegraph::raytrace(int width,int height,stack<glm::mat4>& modelview)
{
    GLfloat *buffer;
    int i,j,k;
    float z = (-0.5*height)/tan(30*3.14159/180);
    Ray r;
    glm::vec3 color;
    bool hit;
    int bounce = 0;

    buffer = new GLfloat[3*width*height];

    lights.clear();
    root->getLights(lights,modelview);

    //all red for now
    for (i=0;i<height;i++)
    {
        for (j=0;j<width;j++)
        {

//            if ((i==height/2) && (j==width/2))
//                cout << "here" << endl;
            r.start = glm::vec4(0,0,0,1);
            r.dir = glm::vec4(j-0.5*width,i-0.5*height,z,0);

            raycast(r,modelview,color,hit,bounce);
            for (k=0;k<3;k++)
            {
                buffer[3*(i*width+j)+k] = color[k];
            }
        }
    }
    return buffer;
}

bool Scenegraph::raycast(Ray& r,stack<glm::mat4>& modelview,glm::vec3& color, bool hit, int bounce)
{
//    if(bounce >= 5)
//    {
//        color = glm::vec3(1,1,1);
//        return true;
//    }
    HitRecord h;

    h.time = -1;
    bool result = root->intersect(r,h,modelview);

    if(!result || bounce >= 5)
    {
        color = glm::vec3(0,0,0);
        hit = false;
        return result;
    }
    else {
        hit = true;
        color = shade(h,modelview);

        //if the object is reflective, and the bounce count is below 5,
        //create the reflect ray and cast it
        if(h.mat.getReflection() > 0)
        {
            Ray reflect;

            glm::vec3 temp, tempNormal, tempDir;
            glm::vec3 reflectColor;

            temp = glm::vec3(r.dir.x,r.dir.y,r.dir.z);
            tempNormal = glm::vec3(h.normal.x,h.normal.y,h.normal.z);
            tempDir = glm::vec3(glm::reflect(temp,tempNormal));

            reflect.dir = glm::vec4(tempDir.x, tempDir.y, tempDir.z, 0);
            //reflect.dir = glm::vec4(glm::reflect(r.dir,h.normal));

            reflect.start = glm::vec4(h.point.x +(0.01*reflect.dir.x) ,h.point.y +(0.01*reflect.dir.y) , h.point.z +(0.01*reflect.dir.z) ,1);
            //reflect.start = glm::vec4(h.point.x +(0.01*r.dir.x) ,h.point.y +(0.01*r.dir.y) , h.point.z +(0.01*r.dir.z) ,1);

            raycast(reflect,modelview,reflectColor,hit,bounce+1);

            //blend reflect color and color from shade
            color.r = (color.r * h.mat.getAbsorption()) + (reflectColor.r * h.mat.getReflection());
            color.g = (color.g * h.mat.getAbsorption()) + (reflectColor.g * h.mat.getReflection());
            color.b = (color.b * h.mat.getAbsorption()) + (reflectColor.b * h.mat.getReflection());
        }
    }

    return result;
}

glm::vec3 Scenegraph::shade(HitRecord& h, stack<glm::mat4> & modelview)
{
    glm::vec3 color = glm::vec3(0,0,0);
    glm::vec3 texColor = glm::vec3(0,0,0);
    glm::vec3 ambient,diffuse,specular;
    glm::vec3 constantAmbient = glm::vec3(0.15,0.15,0.15);
    int i;
    glm::vec3 lightVec,reflectVec,viewVec,normalView;
    float nDotL,rDotV;
    Ray shadow;
    bool hit;
    int bounce = 0;

    //glm::normalize(h.)

    normalView = glm::vec3(glm::swizzle<glm::X,glm::Y,glm::Z>(h.normal));
    normalView = glm::normalize(normalView);

    viewVec = glm::vec3(-h.point.x,-h.point.y,-h.point.z);
    viewVec = glm::normalize(viewVec);

    //lookup the color of the point with the texture
    h.tex->lookup(h.textCoords.s, h.textCoords.t,texColor.r,texColor.g,texColor.b);


    for (i=0;i<lights.size();i++)
    {
        if (lights[i].getPosition().w!=0)
            lightVec = glm::vec3(glm::swizzle<glm::X,glm::Y,glm::Z>(lights[i].getPosition() - h.point));
        else
            lightVec = glm::vec3(glm::swizzle<glm::X,glm::Y,glm::Z>(-lights[i].getPosition()));



        glm::vec3 spotDirection = glm::vec3(glm::swizzle<glm::X,glm::Y,glm::Z>(lights[i].getSpotDirection()));

        spotDirection = glm::normalize(spotDirection);

        //Direction of light vector
        lightVec = glm::normalize(lightVec);

        //shadows
        shadow.start = glm::vec4(h.point.x + (0.01*lightVec.x), h.point.y + (0.01*lightVec.y),h.point.z + (0.01*lightVec.z),1);
        //shadow.dir = glm::vec4(lights[i].getPosition().x,lights[i].getPosition().y,lights[i].getPosition().z,0);
        shadow.dir = glm::vec4(lightVec.x,lightVec.y,lightVec.z,0);
        hit = root->intersect(shadow,h,modelview);

        //if the ray has hit something ignore it
        if(hit)
        {

            //keep the ambient lighting of the material, and the ambient lighting for the
            //entire scene to try to make the shadow looks more realisitic
            ambient = h.mat.getAmbient()*constantAmbient;
            color += ambient;

        }
         else if (glm::dot(-lightVec,spotDirection)>cos(lights[i].getSpotAngle()*3.14159/180))
        {


            nDotL = glm::dot(normalView,lightVec);

            reflectVec = glm::reflect(-lightVec,normalView);
            reflectVec = glm::normalize(reflectVec);

            rDotV = std::max<float>(glm::dot(reflectVec,viewVec),0.0);

            /*outColor = vZColor; */
            ambient = h.mat.getAmbient() * lights[i].getAmbient();
            diffuse = h.mat.getDiffuse() * lights[i].getDiffuse() * std::max<float>(nDotL,0);
            if (nDotL>0)
                specular = h.mat.getSpecular() * lights[i].getSpecular() * pow(rDotV,h.mat.getShininess());
            else
                specular = glm::vec3(0,0,0);
            color = color + ambient + diffuse + specular;


        }
    }
    color *= texColor;
    return color;
}

void Scenegraph::initShaderProgram(GLint shaderProgram)
{
   modelviewLocation = glGetUniformLocation(shaderProgram,"modelview");
   normalmatrixLocation = glGetUniformLocation(shaderProgram,"normalmatrix");
   textureMatrixLocation = glGetUniformLocation(shaderProgram,"texturematrix");
   materialLocations.ambientLocation = glGetUniformLocation(shaderProgram,"material.ambient");
   materialLocations.diffuseLocation = glGetUniformLocation(shaderProgram,"material.diffuse");
   materialLocations.specularLocation = glGetUniformLocation(shaderProgram,"material.specular");
   materialLocations.shininessLocation = glGetUniformLocation(shaderProgram,"material.shininess");

   numLightsLocation = glGetUniformLocation(shaderProgram,"numLights");
   for (int i=0;i<10;i++)
   {
       stringstream name;

       name << "light[" << i << "].ambient";

       lightLocation[i].ambientLocation = glGetUniformLocation(shaderProgram,name.str().c_str());

       name.clear();//clear any bits set
       name.str(std::string());

       name << "light[" << i << "].diffuse";

       lightLocation[i].diffuseLocation = glGetUniformLocation(shaderProgram,name.str().c_str());

       name.clear();//clear any bits set
       name.str(std::string());

       name << "light[" << i << "].specular";

       lightLocation[i].specularLocation = glGetUniformLocation(shaderProgram,name.str().c_str());

       name.clear();//clear any bits set
       name.str(std::string());

       name << "light[" << i << "].position";

       lightLocation[i].positionLocation = glGetUniformLocation(shaderProgram,name.str().c_str());

       name.clear();//clear any bits set
       name.str(std::string());

       name << "light[" << i << "].spot_direction";

       lightLocation[i].spotDirectionLocation = glGetUniformLocation(shaderProgram,name.str().c_str());

       name.clear();//clear any bits set
       name.str(std::string());

       name << "light[" << i << "].spot_cutoff";

       lightLocation[i].spotCutoffLocation = glGetUniformLocation(shaderProgram,name.str().c_str());

       name.clear();//clear any bits set
       name.str(std::string());

   }
   textureLocation = glGetUniformLocation(shaderProgram,"image");


}

void Scenegraph::enableLights(stack<glm::mat4>& modelView)
{

    if (root!=NULL)
    {
        lights.clear();
        root->getLights(lights,modelView);

        glUniform1i(numLightsLocation,lights.size());

        //light properties
        for (int i=0;((i<lights.size()) && (i<10));i++)
        {
            glUniform3fv(lightLocation[i].ambientLocation,1,glm::value_ptr(lights[i].getAmbient()));
            glUniform3fv(lightLocation[i].diffuseLocation,1,glm::value_ptr(lights[i].getDiffuse()));
            glUniform3fv(lightLocation[i].specularLocation,1,glm::value_ptr(lights[i].getSpecular()));
            glUniform4fv(lightLocation[i].positionLocation,1,glm::value_ptr(lights[i].getPosition()));
            glUniform4fv(lightLocation[i].spotDirectionLocation,1,glm::value_ptr(lights[i].getSpotDirection()));
            glUniform1f(lightLocation[i].spotCutoffLocation,3.14159*lights[i].getSpotAngle()/180);
        }
    }

}

void Scenegraph::addTexture(Texture *texture)
{
    textures.push_back(texture);
}

void Scenegraph::draw(stack<glm::mat4>& modelView)
{
    if (root!=NULL)
    {
     //   glEnable(GL_TEXTURE_2D);
        glActiveTexture(GL_TEXTURE0);
        glUniform1i(textureLocation,0); //bind GL_TEXTURE0 to sampler2D (whatever is bound to GL_TEXTURE0)
        root->draw(modelView);
    }
}

/*
void Scenegraph::draw(glm::mat4 worldToView)
{
    stack<Node *> stackNodes; //the stack of nodes
    stack<glm::mat4> stackModelview; //the corresponding parent to world transformation for each node in stackNodes
    Node *curr;
    glm::mat4 parentToWorld,currentToWorld;

    //put the root on the stack
    if (root==NULL)
        return;

    stackNodes.push(root);
    stackModelview.push(worldToView);

    while (!stackNodes.empty())
    {
        //pop one node and its transform out
        curr = stackNodes.top();
        parentToWorld = stackModelview.top();

        stackNodes.pop();
        stackModelview.pop();

        currentToWorld = parentToWorld * curr->getAnimationTransform() * curr->getTransform();
        //check if the current node is a group or an actual object
        if (dynamic_cast<Group *>(curr)!=NULL)
        {
            Group *group = dynamic_cast<Group *>(curr);

            vector<Node *> children = group->getChildren();

            for (unsigned int i=0;i<children.size();i++)
            {
                stackNodes.push(children[i]);
                stackModelview.push(currentToWorld);
            }
        }
        else
        {
            Object *object = dynamic_cast<Object *>(curr);

            glUniformMatrix4fv(modelviewLocation,1,GL_FALSE,glm::value_ptr(currentToWorld));
            glUniformMatrix4fv(normalmatrixLocation,1,GL_FALSE,glm::value_ptr(glm::inverseTranspose(currentToWorld)));
            //get the material
            Material material = object->getMaterial();

            //put the material in the material properties
            glUniform3fv(materialLocations.mAmbientLocation,1,material.getAmbient());
            glUniform3fv(materialLocations.mDiffuseLocation,1,material.getDiffuse());
            glUniform3fv(materialLocations.mSpecularLocation,1,material.getSpecular());
            glUniform3fv(materialLocations.mShininessLocation,1,material.getShininess());

            object->draw();
        }
    }

}
*/

void Scenegraph::animate(float t)
{
    glm::mat4 mat;
    float speed = 50;


    Node *wagonnode = nodemap["wagon"];
    if (wagonnode!=NULL)
    {
        mat = glm::rotate(glm::mat4(1.0f),speed*t,glm::vec3(0.0f,1.0f,0.0f)) * glm::translate(glm::mat4(1.0f),glm::vec3(0.0f,0.0f,400.0f));
        wagonnode->setAnimationTransform(mat);
    }

    Node *propellor = nodemap["propellor"];
    if (propellor!=NULL)
    {
        mat = glm::rotate(glm::mat4(1.0f),speed*25*t,glm::vec3(0.0f,0.0f,1.0f));
        propellor->setAnimationTransform(mat);
    }

    mat = glm::rotate(glm::mat4(1.0f),-speed*40*t/3,glm::vec3(0.0f,0.0f,1.0f));
    Node *wheel = nodemap["front-left-wheel"];
    if (wheel!=NULL)
    {
        wheel->setAnimationTransform(mat);
    }

    wheel = nodemap["front-right-wheel"];
    if (wheel!=NULL)
    {
        wheel->setAnimationTransform(mat);
    }
    mat = glm::rotate(glm::mat4(1.0f),-speed*40*t/3,glm::vec3(0.0f,0.0f,1.0f));
    wheel = nodemap["rear-left-wheel"];
    if (wheel!=NULL)
    {
        wheel->setAnimationTransform(mat);
    }

    wheel = nodemap["rear-right-wheel"];
    if (wheel!=NULL)
    {
        wheel->setAnimationTransform(mat);
    }
}
