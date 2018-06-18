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

layout(set = 1, binding = 1) uniform sampler2D albedo[6];

// In data //
layout (location = 0) in vec3 inNormal;
layout (location = 1) in vec2 inUV;

// Out data //
layout (location = 0) out vec4 outColor;

void main() 
{
    //vec3 lightDirection = (perFrameData.viewMatrix * vec4(normalize(vec3(-1, 1, -1)), 0.0)).xyz;
    //vec3 normal = (perDrawData.mvMatrix * vec4(inNormal, 0.0)).xyz;
    vec4 texSample = texture(albedo[0], inUV);
    //vec3 diffuseColour = texSample * dot(normal, lightDirection);

    outColor = texSample;
    //outColor = vec4(inNormal, 1.0);
    //outColor = vec4(inUV, 0.0, 1.0);
}