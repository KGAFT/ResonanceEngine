#version 460 core

layout(location = 0) in vec3 position;
layout(location = 0) out vec3 localPos;

layout(push_constant) uniform WorldTransformData{
    mat4 viewProjectionMatrix;
} worldTransformData;

void main(){
    gl_Position = worldTransformData.viewProjectionMatrix*vec4(position, 1);
    localPos = position;

}