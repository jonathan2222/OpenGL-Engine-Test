#version 440
in vec2 fragUvs;
in vec3 fragNormal;
in vec4 fragPos;

uniform vec3 camPos;

layout(std140) uniform MaterialData
{
	vec4 ka;
	vec4 kd;
    vec4 ks;
} materialData;

struct LightData
{
    vec4 positionRadius;
    vec4 colorIntensity;
    vec4 specularColor;
};

#define MAX_LIGHTS 10
layout(std140) uniform Lights
{
    LightData lightsData[MAX_LIGHTS];
    int numLights;
    int padding[3];
} lights;

out vec4 finalColor;

void main () 
{
    vec3 normal = normalize(fragNormal);
	vec3 materialColor = min(vec3(0.5, 0.5, 0.5) + vec3(fragUvs.x, fragUvs.y, 0.0f), vec3(1.0, 1.0, 1.0));

    // Ambient part
    vec3 ambientFactor = materialData.ka.xyz;
    vec3 ambient = ambientFactor*materialColor;

    vec3 color = ambient;
    for(int i = 0; i < lights.numLights; i++)
    {
        // Diffuse part
        vec3 fragToLight = normalize(lights.lightsData[i].positionRadius.xyz-fragPos.xyz);
        float diffuseFactor = max(dot(normal, fragToLight), 0.0);
        vec3 diffuse = diffuseFactor*materialColor*materialData.kd.xyz*lights.lightsData[i].colorIntensity.xyz*lights.lightsData[i].colorIntensity.w/(smoothstep(0.0, lights.lightsData[i].positionRadius.w, length(fragToLight))*lights.lightsData[i].positionRadius.w);

        // Specular part
        vec3 specularColor = materialData.ks.xyz;
        float s = materialData.ks.w;
        vec3 r = reflect(normal, fragToLight);
        vec3 v = normalize(camPos - fragPos.xyz);
        float specularFactor = pow(max(dot(r, v), 0.0), s);
        vec3 specular = specularFactor*specularColor*lights.lightsData[i].specularColor.xyz;

        color += diffuse + specular; 
    }

	finalColor = vec4(min(color, vec3(1.0, 1.0, 1.0)), 1.0);
}