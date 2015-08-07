#include "Light.h"

Light::Light(void)
{
	init();
}

Light::~Light(void)
{
}

void Light::init()
{
    ambient = diffuse = specular = glm::vec3(0,0,0);
    position = glm::vec4(0,0,0,1);
    spot_direction = glm::vec4(0,0,-1,0);
    spot_cutoff = 180; //by default not a spotlight
}


void Light::setAmbient(float r,float g,float b)
{
    ambient = glm::vec3(r,g,b);
}

void Light::setDiffuse(float r,float g,float b)
{
    diffuse = glm::vec3(r,g,b);
}

void Light::setSpecular(float r,float g,float b)
{
    specular = glm::vec3(r,g,b);
}


void Light::setAmbient(glm::vec3 color)
{
    ambient = color;
}

void Light::setDiffuse(glm::vec3 color)
{
    diffuse = color;
}

void Light::setSpecular(glm::vec3 color)
{
    specular = color;
}

void Light::setPosition(glm::vec4 pos)
{
    position = pos;
}


void Light::setSpotDirection(glm::vec4 dir)
{
    spot_direction = dir;
}

void Light::setSpotAngle(float angle)
{
	spot_cutoff = angle;
}


glm::vec3 Light::getAmbient()
{
	return ambient;
}

glm::vec3 Light::getDiffuse()
{
	return diffuse;
}

glm::vec3 Light::getSpecular()
{
	return specular;
}

glm::vec4 Light::getPosition()
{
	return position;
}

glm::vec4 Light::getSpotDirection()
{
	return spot_direction;
}

float Light::getSpotAngle()
{
	return spot_cutoff;
}

