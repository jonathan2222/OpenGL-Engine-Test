#version 440

in vec2 fragUvs;

out vec3 color;

uniform sampler2D diffuseTexture;
uniform sampler2D normalTexture;
uniform sampler2D positionTexture;
uniform sampler2D depthTexture;

float inRange(vec4 bound)
{
    if(fragUvs.x < bound.x ||
    fragUvs.x > bound.z ||
    fragUvs.y < bound.y ||
    fragUvs.y > bound.w)
        return 0.0;
    else
        return 1.0;
}

vec2 map(vec2 v, vec2 inMin, vec2 inMax, vec2 outMin, vec2 outMax)
{
    v = clamp(v, inMin, inMax);
    return outMin + (outMax-outMin) * ((v-inMin)/(inMax-inMin));
}

void main()
{
    vec3 diffuse = texture(diffuseTexture,      map(fragUvs, vec2(0.0, 0.0), vec2(0.5, 0.5), vec2(0.0, 0.0), vec2(1.0,1.0))).xyz;
    vec3 normal = texture(normalTexture,        map(fragUvs, vec2(0.5, 0.0), vec2(1.0, 0.5), vec2(0.0, 0.0), vec2(1.0,1.0))).xyz;
    vec3 position = texture(positionTexture,    map(fragUvs, vec2(0.0, 0.5), vec2(0.5, 1.0), vec2(0.0, 0.0), vec2(1.0,1.0))).xyz;
    float depth = texture(depthTexture,         map(fragUvs, vec2(0.5, 0.5), vec2(1.0, 1.0), vec2(0.0, 0.0), vec2(1.0,1.0))).x;
    
    vec3 normal2 = texture(normalTexture,       map(fragUvs, vec2(0.0, 0.0), vec2(0.5, 0.5), vec2(0.0, 0.0), vec2(1.0,1.0))).xyz; 
    vec3 position2 = texture(positionTexture,   map(fragUvs, vec2(0.0, 0.0), vec2(0.5, 0.5), vec2(0.0, 0.0), vec2(1.0,1.0))).xyz; 

    diffuse = diffuse*(vec3(0.1,0.1,0.1)+vec3(0.5,0.5,0.5)*max(0.0, dot(normalize((normal2-0.5)*2.0), normalize(vec3(0.0, 8.0, 8.0)-position2))));
    color = clamp(diffuse*inRange(vec4(0.0, 0.0, 0.5, 0.5))+
                normal*inRange(vec4(0.5, 0.0, 1.0, 0.5))+
                position*inRange(vec4(0.0, 0.5, 0.5, 1.0))+
                vec3(depth, depth, depth)*inRange(vec4(0.5, 0.5, 1.0, 1.0))
                , 0.0, 1.0);
}