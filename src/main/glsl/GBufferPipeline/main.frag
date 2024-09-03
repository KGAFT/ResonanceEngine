#version 460 core
#extension GL_EXT_scalar_block_layout : require

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normals;
layout(location = 2) in vec2 textureCoordinates;
layout(location = 3) flat in uint materialIndex;

layout(location = 0) out vec4 readyPos;
layout(location = 1) out vec4 readyAlbedo;
layout(location = 2) out vec4 readyNormal;
layout(location = 3) out vec4 readyMetallicRoughnessAoEmissive;


layout(std430, binding = 2, set = 0) uniform WorldTransformData{
    mat4 viewMatrix;
    mat4 worldMatrix;
    vec3 cameraPosition;
} worldTransformData;

#include "ShaderBase.glsl"

vec4 parseAlbedo();
vec2 parseMetallicRoughness();
vec3 parseNormals();
vec2 parseAoEmissive(vec3 albedo);

void main(){
    readyPos = vec4(position, 1.0f);
  //  readyAlbedo = parseAlbedo();
  //  readyNormal = vec4(parseNormals(), 1.0f);
 //   vec2 metallicRoughness = parseMetallicRoughness();
 //   vec2 aoEmissive = parseAoEmissive(readyAlbedo.rgb);
  //  readyMetallicRoughnessAoEmissive = vec4(metallicRoughness, aoEmissive.r, aoEmissive.g);
}

vec4 parseAlbedo(){
    vec4 res = vec4(1,1,1,1);
    Material material = materialBuffer.materials[materialIndex];
    if(material.albedoIndex!=-1){
        res = texture(textures[material.albedoIndex], textureCoordinates);
    }
    if(material.opacityMapIndex!=-1){
        res.a = texture(textures[material.opacityMapIndex], textureCoordinates).r;
    }
    return res;
}

vec2 parseMetallicRoughness(){
    vec2 res = vec2(0);
    Material material = materialBuffer.materials[materialIndex];
    if(material.metallicMapIndex!=-1 || material.roughnessMapIndex!=-1){
        vec2 parsedMetallicRoughness = vec2(0);
        if(material.metallicMapIndex==material.roughnessMapIndex){
            parsedMetallicRoughness = texture(textures[material.metallicMapIndex], textureCoordinates).rg;
        }
        else{
            parsedMetallicRoughness = vec2(texture(textures[material.metallicMapIndex], textureCoordinates).r, texture(textures[material.roughnessMapIndex], textureCoordinates).r);
        }
        res.r+=parsedMetallicRoughness.r;
        res.g+=parsedMetallicRoughness.g;
    }
    return res;
}

vec3 getNormalFromMap(vec2 uvsCoords, vec3 normals, vec3 fragmentPosition)
{
    int normalIndex = materialBuffer.materials[materialIndex].normalMapIndex;
    if(normalIndex!=-1){
        vec4 baseNormal = texture(textures[normalIndex], uvsCoords);
        vec3 tangentNormal = baseNormal.xyz * 2.0 - 1.0;

        vec3 Q1  = dFdx(fragmentPosition);
        vec3 Q2  = dFdy(fragmentPosition);
        vec2 st1 = dFdx(uvsCoords);
        vec2 st2 = dFdy(uvsCoords);
        vec3 N   = normalize(normals);
        vec3 T  = normalize(Q1*st2.t - Q2*st1.t);
        vec3 B  = -normalize(cross(N, T));
        mat3 TBN = mat3(T, B, N);
        return normalize(TBN * tangentNormal);
    }
    return normalize(normals);
}

vec3 parseNormals(){
    return getNormalFromMap(textureCoordinates, normals, position);
}

float getEmissivePower(int emissiveIndex, vec2 UvsCoords, vec3 albedoColor){
    vec4 emissive = texture(textures[emissiveIndex], UvsCoords);
    if(emissive.r+emissive.g+emissive.b>=0.1f){
        return (emissive.r+emissive.g+emissive.b)/(albedoColor.r+albedoColor.g+albedoColor.b);
    }
    return 0;
}

vec2 parseAoEmissive(vec3 albedo){
    vec2 res = vec2(0, 1);
    int aoMapIndex = materialBuffer.materials[materialIndex].aoMapIndex;
    if(aoMapIndex!=-1){
        res.r = texture(textures[aoMapIndex], textureCoordinates).r;
    }
    int emissiveIndex = materialBuffer.materials[materialIndex].emissiveMapIndex;
    if(emissiveIndex!=-1){
        res.g+=getEmissivePower(emissiveIndex, textureCoordinates, albedo);
    }
    return res;
}