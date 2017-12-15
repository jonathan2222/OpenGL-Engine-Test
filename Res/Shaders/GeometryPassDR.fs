#version 440

in vec2 fragUvs;
in vec3 fragNormal;
in vec3 fragPos;

layout(location = 0) out vec3 diffuseOut;
layout(location = 1) out vec3 normalOut;
layout(location = 2) out vec3 positionOut;

//uniform vec3 camPos;

//uniform sample2D colorMap;

void main()
{
    diffuseOut = vec3(min(fragUvs, 0.0), 0.0)+vec3(1.0, 1.0, 1.0); //texture(colorMap, texcoordsOut).xyz;
    normalOut = fragNormal*0.5 + 0.5;
    positionOut = fragPos;
}