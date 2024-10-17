#version 460 core
layout(location = 0) in vec3 position;
layout(location = 0) out vec4 color;

layout(set = 0, binding = 0) uniform sampler2D hdrMap;
#define PI 3.1415926535897932384626433832795

void main(){
    vec3 pos = normalize(position);
    float u = 1 - atan(pos.z , pos.x) / (2 * PI);
    float v = -atan(pos.y, sqrt(pos.x * pos.x + pos.z * pos.z)) / PI + 0.5f;
    color = texture(hdrMap, vec2(u, v));
}