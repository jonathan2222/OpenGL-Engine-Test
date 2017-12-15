#version 440

layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec2 vertexUvs;

out vec2 fragUvs;

void main()
{
    fragUvs = vertexUvs;
    gl_Position = vec4(vertexPosition, 1.0);
}