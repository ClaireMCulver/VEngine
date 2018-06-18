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

layout (location = 0) in vec2 inUV;

// Out data //
layout (location = 0) out vec4 outColor;

int blurAmount = 5;

vec2 invVP = vec2(1.0/800.0, 1.0/600.0); //I really just don't want to go through the trouble of creating a new uniform for this in vulkan.

void main() 
{
	float weight[5];
	weight[0] = 0.227027;
	weight[1] = 0.1945946;
	weight[2] = 0.1216216;
	weight[3] = 0.054054;
	weight[4] = 0.016216;

	outColor = texture(albedo[0], inUV + (vec2( 0.0,  0.0) * invVP)) * weight[0];

	for (int i = 1; i < 5; i++)
	{
		outColor += texture(albedo[0], inUV + (vec2( i, 0) * invVP)) * weight[i];
		outColor += texture(albedo[0], inUV + (vec2(-i, 0) * invVP)) * weight[i];
	}

	
}