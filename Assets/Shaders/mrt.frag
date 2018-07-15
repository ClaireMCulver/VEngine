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
layout (location = 2) in vec3 inTangent;
layout (location = 3) in vec3 inBitangent;

// Out data //
layout (location = 0) out vec4 outAlbedo;
layout (location = 1) out vec4 outNormal;
layout (location = 2) out vec4 outPosition;

void main() 
{
    mat3 TBN = transpose(mat3(inTangent, 
                    inBitangent, 
                    inNormal));
    vec3 lightDirection = TBN * (vec4(normalize(vec3(0, 0, 1)), 0.0)).xyz;
    vec3 normal = texture(albedo[1], inUV).rgb;
    normal = normalize(normal * 2.0 - 1.0);
    vec4 texSample = texture(albedo[0], inUV);
    vec3 diffuseColour = texSample.rgb * dot(normal, lightDirection);

    outAlbedo = texSample;
    outNormal = vec4(inNormal, 1.0);
    outPosition = texSample;
}