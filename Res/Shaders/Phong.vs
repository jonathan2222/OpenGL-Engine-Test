#version 440
layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec3 vertexNormal;
layout(location = 2) in vec2 vertexUvs;

uniform mat4 model;
uniform mat4 world;
uniform mat4 camera;

out vec2 fragUvs;
out vec3 fragNormal;
out vec4 fragPos;

void main() 
{
	fragPos = world*model*vec4(vertexPosition, 1.0);
	fragUvs = vertexUvs;
	fragNormal = (world*model*vec4(vertexNormal, 0.0)).xyz;
	gl_Position = camera*fragPos;
}