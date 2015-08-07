#version 400 core

uniform sampler2D image;

in vec4 fTexCoord;
in vec4 fNormal;
out vec4 fColor;

void main()
{
    fColor = texture2D(image,fTexCoord.st);
}
