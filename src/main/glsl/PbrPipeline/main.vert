#version 460 core

#extension GL_EXT_shader_explicit_arithmetic_types : require
#extension GL_EXT_scalar_block_layout : require
#extension GL_EXT_nonuniform_qualifier : require

layout(location = 0) in vec3 position;
layout(location = 1) in vec2 uvs;

layout(location = 0) out vec2 ReadyUvs;


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
    ReadyUvs = uvs;
    gl_Position = vec4(position, 1.0);
}