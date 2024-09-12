#version 460 core

layout(location = 0) in vec2 uvs;

layout(set = 0, binding = 0) uniform sampler2D positions;
layout(set = 0, binding = 1) uniform sampler2D albedo;
layout(set = 0, binding = 2) uniform sampler2D normal;
layout(set = 0, binding = 3) uniform sampler2D metallicRoughnessAoEmissive;

struct PointLight{
    vec3 position;
    vec3 color;
    float intensity;
};

struct DirectLight{
    vec3 direction;
    vec3 color;
    float intensity;
};

layout (std430, binding = 4, set = 0)  buffer DirectLightsBuffer {
    DirectLight lights[];
}  directLightsBuffer ;
layout (std430, binding = 5, set = 0)  buffer PointLightsBuffer {
    PointLight lights[];
}  pointLightsBuffer ;

layout(std430, binding = 0) uniform LightConfiguration{
    int enabledDirects;
    int enabledPoints;

    float emissiveIntensity;
    float emissiveShininess;
    float gammaCorrect;
    float ambientIntensity;
} lightUbo;

float distributeGGX(vec3 normals, vec3 halfWayVector, float roughness)
{
    float a = roughness * roughness;
    float NdotH = max(dot(normals, halfWayVector), 0.0);
    float NdotH2 = NdotH * NdotH;

    float nom = a;
    float denom = (NdotH2 * (nom - 1.0) + 1.0);
    denom = PI * denom * denom;

    return nom / denom;
}

float schlickGeometryGGX(float dotWorldViewVector, float roughness)
{
    float roughnessKoef = ((roughness + 1.0) * (roughness + 1.0)) / 8.0;
    float numerator = dotWorldViewVector;
    float denominator = dotWorldViewVector * (1.0 - roughnessKoef) + roughnessKoef;
    return numerator / denominator;
}

float smithGeometry(vec3 processedNormals, vec3 worldViewVector, vec3 lightPosition, float roughness)
{
    float worldViewVectorDot = max(dot(processedNormals, worldViewVector), 0.0);
    float lightDot = max(dot(processedNormals, lightPosition), 0.0);
    float ggx2 = schlickGeometryGGX(worldViewVectorDot, roughness);
    float ggx1 = schlickGeometryGGX(lightDot, roughness);
    return ggx1 * ggx2;
}

vec3 fresnelSchlickBase(float cosTheta, vec3 startFresnelSchlick)
{
    return startFresnelSchlick + (1.0 - startFresnelSchlick) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

vec3 fresnelFunctionMetal(vec3 objColor, vec3 startFresnelSchlick,  vec3 h, in vec3 worldViewVector, float metalllic)
{
    vec3 f = startFresnelSchlick * (1 - metalllic) + objColor * metalllic;
    return f + (vec3(1.0f, 1.0f, 1.0f) - f) * pow(1.0f - max(dot(h, worldViewVector), 0.0f), 5);
}

vec3 F_SchlickR(float cosTheta, vec3 F0, float roughness)
{
    return F0 + (max(vec3(1.0 - roughness, 1.0-roughness, 1.0-roughness), F0) - F0) * pow(1.0 - cosTheta, 5.0);
}

/*
vec3 prefilteredReflection(vec3 R, float roughness)
{
    const float MAX_REFLECTION_LOD = 9.0;
    float lod = roughness * MAX_REFLECTION_LOD;
    float lodf = floor(lod);
    float lodc = ceil(lod);
    vec3 a =  prefilteredTexture.SampleLevel(prefilteredSampler, R, lodf).rgb;
    vec3 b =  prefilteredTexture.SampleLevel(prefilteredSampler, R, lodc).rgb;
    return mix(a, b, lod - lodf);
}
*/



void main(){

}