#version 400 core

layout(location=0) in vec4 vPosition;
layout(location=1) in vec4 vNormal;
layout(location=2) in vec4 texCoord;

uniform mat4 projection;
uniform mat4 modelview;

out vec4 fTexCoord;
out vec4 fNormal;

void main()
{
    gl_Position = projection * modelview * vec4(vPosition.xyzw);
    fNormal = vNormal; //bogus
    fTexCoord = texCoord;
}
