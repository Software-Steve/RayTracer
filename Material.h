#ifndef _MATERIAL_H_
#define _MATERIAL_H_

#include <glm/glm.hpp>

class Material
{
private:
    glm::vec3 ambient,diffuse,specular;
	float shininess;
    float absorption,reflection,transparency,refractive_index;

public:
	Material(void);
	~Material(void);

	void setAmbient(float r,float g,float b);
	void setDiffuse(float r,float g,float b);
	void setSpecular(float r,float g,float b);
	void setShininess(float r);
    void setAbsorption(float a);
    void setReflection(float r);
    void setTransparency(float t);
    void setRefractiveIndex(float r);

    void init();
	
    glm::vec3 getAmbient();
    glm::vec3 getDiffuse();
    glm::vec3 getSpecular();
    float getShininess();

    float getAbsorption();
    float getReflection();
    float getTransparency();
    float getRefractiveIndex();
};

#endif
