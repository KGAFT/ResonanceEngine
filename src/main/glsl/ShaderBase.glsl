#extension GL_EXT_shader_explicit_arithmetic_types : require

struct Material {
    int albedoIndex;
    int normalMapIndex;
    int metallicMapIndex;
    int roughnessMapIndex;
    int aoMapIndex;
    int emissiveMapIndex;
    int opacityMapIndex;
};



layout (binding = 0, set = 0) uniform sampler2D textures[];

layout (std430, binding = 1, set = 0) buffer MaterialBuffer {
    Material materials[];
} materialBuffer;

