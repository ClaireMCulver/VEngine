#version 450
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable
layout (std140, set = 0, binding = 0) uniform bufferVals 
{
    mat4 mvp;
} myBufferVals;
layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 inUV;
layout (location = 0) out vec2 outUV;
out gl_PerVertex 
{
    vec4 gl_Position;
};
void main() {
   outUV = inUV;
   gl_Position = myBufferVals.mvp * vec4(pos, 1.0);
}