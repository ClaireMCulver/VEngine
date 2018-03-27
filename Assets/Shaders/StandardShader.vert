#version 450
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

// Uniforms //
layout(set = 0, binding = 0) uniform PerFrameData
{
    mat4 viewMatrix;
    mat4 projectionMatrix;
    mat4 viewProjectionMatrix;
} perFrameData;

layout(set = 0, binding = 1) uniform PerPassData
{
    int data;
} perPassData;

layout (set = 1, binding = 0) uniform PerDrawData
{
    mat4 modelMatrix;
    mat4 mvMatrix;
    mat4 mvpMatrix;
} perDrawData;

layout(set = 1, binding = 1) uniform sampler2D albedo;

// Per Vertex Data //
layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 inNormal;
layout (location = 2) in vec2 inUV;

// Per Instance Data
layout (location = 3) in vec3 localPosition;
layout (location = 4) in vec4 localRotation;

// Out data //
layout (location = 0) out vec3 outNormal;
layout (location = 1) out vec2 outUV;
out gl_PerVertex 
{
    vec4 gl_Position;
};

void main() {
    outNormal = inNormal;
    outUV = inUV;
    vec3 rotatedPosition = pos + 2.0 * cross(localRotation.xyz, cross(localRotation.xyz, pos) + localRotation.w * pos);
    gl_Position = perFrameData.viewProjectionMatrix * vec4(rotatedPosition + localPosition, 1.0);
}