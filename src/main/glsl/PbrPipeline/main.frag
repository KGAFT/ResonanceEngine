#version 460 core

#define PI 3.141592653589793238462643383279502884197

layout(location = 0) in vec2 uvs;
layout(location = 0) out vec4 fragColor;
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

layout(std140, binding = 6) uniform LightConfiguration{
    int enabledDirects;
    int enabledPoints;

    float emissiveIntensity;
    float emissiveShininess;
    float gammaCorrect;
    float ambientIntensity;
    vec3 cameraPosition;
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

vec3 processPointLight(in PointLight light, vec3 normals, vec3 fragmentPosition, vec3 worldViewVector,
                         vec3 startFresnelSchlick, float roughness, float metallic, vec3 albedo)
{
    vec3 processedLightPos = normalize(light.position - fragmentPosition);
    float distance = length(light.position - fragmentPosition);
    vec3 halfWay = normalize((worldViewVector + processedLightPos)/2.0f);
    
    float attenuation = clamp(1.0 / (distance * distance), 0.01, 1.0f);
    vec3 radiance = light.color * light.intensity *attenuation;

    float halfWayGGX = distributeGGX(normals, halfWay, roughness);
    float geometrySmith = smithGeometry(normals, worldViewVector, processedLightPos, roughness);
    
    
    vec3 fresnelSchlick = fresnelFunctionMetal(albedo, startFresnelSchlick, halfWay, worldViewVector, metallic);

    vec3 numerator = vec3(0);
    float denominator = 1;

    numerator = halfWayGGX*geometrySmith * fresnelSchlick;
    denominator = 4.0 * max(dot(normals, worldViewVector), 0.0) * max(dot(normals, processedLightPos), 0.0) +
            0.0001;
   

    vec3 specular = numerator / denominator;
   
       
    vec3 finalFresnelSchlick = vec3(1) - fresnelSchlick;
    finalFresnelSchlick *= 1.0 - metallic+0.001;

    float NdotL = max(dot(normals, processedLightPos), 0.0);
    return (finalFresnelSchlick * albedo / PI + specular) * radiance * NdotL;
}

vec3 processDirectLight(in DirectLight light, vec3 normals, vec3 fragmentPosition, vec3 worldViewVector,
                         vec3 startFresnelSchlick, float roughness, float metallic, vec3 albedo)
{
    vec3 processedLightPos  = normalize(-light.direction);
    vec3 halfWay = normalize((worldViewVector + processedLightPos)/2.0f);
    vec3 radiance = light.color * light.intensity;
    float halfWayGGX = distributeGGX(normals, halfWay, roughness);
    float geometrySmith = smithGeometry(normals, worldViewVector, processedLightPos, roughness);
    vec3 fresnelSchlick = fresnelFunctionMetal(albedo, startFresnelSchlick, halfWay, worldViewVector, metallic);
    vec3 numerator = vec3(0);
    float denominator = 1;
    numerator = halfWayGGX*geometrySmith * fresnelSchlick;
    denominator = 4.0 * max(dot(normals, worldViewVector), 0.0) * max(dot(normals, processedLightPos), 0.0) +
            0.0001;
    vec3 specular = numerator / denominator;
    vec3 finalFresnelSchlick = vec3(1) - fresnelSchlick;
    finalFresnelSchlick *= 1.0 - metallic+0.001;

    float NdotL = max(dot(normals, processedLightPos), 0.0);
    return (finalFresnelSchlick * albedo / PI + specular) * radiance * NdotL;
}


vec3 postProcessColor(vec3 color){
    color = color / (color + vec3(1.0));
    color = pow(color, vec3(lightUbo.gammaCorrect));
    return color;
}

void main(){
    vec3 fragmentPosition = texture(positions, uvs).xyz;
    vec3 processedNormals = normalize(texture(normal, uvs).xyz);
    vec4 albedoSource = texture(albedo, uvs);
    vec3 albedo = pow(albedoSource.rgb, vec3(2.2));
    vec4 pbrData = texture(metallicRoughnessAoEmissive, uvs);
    float metallic = pbrData.r;
    float roughness = pbrData.g;
    vec3 emissive = albedo*pbrData.a;
    float ao = pbrData.b;
    float opacity = albedoSource.a;

    vec3 startFresnelSchlick = vec3(0.04);
    startFresnelSchlick = mix(startFresnelSchlick, albedo, metallic);
    vec3 worldViewVector = normalize(lightUbo.cameraPosition - fragmentPosition);
    vec3 Lo = vec3(0.0);

    for (int c = 0; c<lightUbo.enabledDirects; c++){
        Lo+=processDirectLight(directLightsBuffer.lights[c], processedNormals, fragmentPosition, worldViewVector,startFresnelSchlick, roughness, metallic, albedo);
    }
    for (int c = 0; c<lightUbo.enabledPoints; c++){
        Lo+=processPointLight(pointLightsBuffer.lights[c], processedNormals, fragmentPosition, worldViewVector,startFresnelSchlick, roughness, metallic, albedo);
    }
    vec3 ambient = (vec3(lightUbo.ambientIntensity) * albedo * ao);
    vec3 color = ambient + Lo;

    color+=(emissive*pow(1, lightUbo.emissiveShininess)*lightUbo.emissiveIntensity);
    color = postProcessColor(color);

    fragColor = vec4(color, opacity);
}