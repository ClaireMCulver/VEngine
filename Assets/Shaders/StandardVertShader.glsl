#version 450
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 inNormal;
layout (location = 2) in vec2 inUV;

layout (location = 0) out vec3 outNormal;
layout (location = 1) out vec2 outUV;

layout (set = 0, binding = 0) uniform Matrix_ModelViewProjection 
{
    mat4 data;
} mvp_matrix;

out gl_PerVertex 
{
    vec4 gl_Position;
};

void main() {
    outNormal = inNormal;
    outUV = inUV;
    gl_Position = mvp_matrix.data * vec4(pos, 1.0);
}