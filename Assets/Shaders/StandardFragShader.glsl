#version 450
#extension GL_ARB_separate_shader_objects : enable
#extension GL_ARB_shading_language_420pack : enable

// In data //
layout (location = 0) in vec3 inNormal;
layout (location = 1) in vec2 inUV;

// Out data //
layout (location = 0) out vec4 outColor;

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

layout (set = 0, binding = 2) uniform PerDrawData
{
    mat4 modelMatrix;
    mat4 MVMatrix;
    mat4 MVPMatrix;
} perDrawData;

layout(set = 0, binding = 3) uniform sampler2D albedo;

void main() 
{
    vec3 normal = (perDrawData.MVMatrix * vec4(inNormal, 0.0)).xyz;
    //outColor = vec4(normal, 1.0);
    outColor = texture(albedo, inUV);
}