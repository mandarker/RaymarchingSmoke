#shader vertex
#version 330 core
layout(location = 0) in vec4 position;
layout(location = 1) in vec3 normal;

uniform mat4 u_LightSpaceMatrix;
uniform mat4 u_Model;

void main()
{
    gl_Position = u_LightSpaceMatrix * u_Model * position;
}