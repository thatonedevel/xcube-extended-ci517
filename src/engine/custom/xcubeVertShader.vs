#version 150 core

// vertex attributes
in vec3 worldSpacePosition;

uniform mat4 modelMat;
uniform mat4 projMat;
uniform mat4 viewMat;

void main()
{
    gl_Position = projMat * viewMat * modelMat * vec4(worldSpacePosition, 1);
}