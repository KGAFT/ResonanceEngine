#version 460 core

#extension GL_EXT_scalar_block_layout : require

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normals;
layout(location = 2) in vec2 textureCoordinates;
layout(location = 3) in uint materialIndex;

layout(location = 0) out vec3 readyPosition;
layout(location = 1) out vec3 readyNormals;
layout(location = 2) out vec2 readyTextureCoordinates;
layout(location = 3) out uint rMaterialIndex;

layout(std430, binding = 2, set = 0) uniform WorldTransformData{
    mat4 viewMatrix;
    mat4 worldMatrix;
    vec3 cameraPosition;
} worldTransformData;

vec2 fixVectorPositioning(vec2 base){
    base.y*=-1;
    return base;
}

vec3 fixVectorPositioning(vec3 base){
    base.y*=-1;
    return base;
}

vec4 fixVectorPositioning(vec4 base){
    base.y*=-1;
    return base;
}

void main(){
    readyPosition = (worldTransformData.worldMatrix*vec4(position, 1.0)).rgb;
    readyNormals = (vec4(normals, 0.0f)*worldTransformData.worldMatrix).xyz;
    readyTextureCoordinates = textureCoordinates;
    rMaterialIndex = materialIndex;
    gl_Position = fixVectorPositioning(worldTransformData.viewMatrix*worldTransformData.worldMatrix*vec4(position, 1.0));
}