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

//predefined fxaa vals
float FXAA_SPAN_MAX = 8.0; //max checking span
float FXAA_REDCUE_MIN = 1.0/128.0; //1.f/128.f used for biasing the sample
float FXAA_REDUCE_MUL = 1.0/8.0; //1.f/8.f, used for biasing the sample

vec2 invVP = vec2(1.0/800.0, 1.0/600.0); //I really just don't want to go through the trouble of creating a new uniform for this in vulkan.

void main() 
{
   vec3 l = vec3(0.299, 0.587, 0.114); //luminosity vector dot(l,colour.rgb) = luma value per pixel
	float lTL = dot(l, texture(albedo[0], inUV + (vec2(-1.0, -1.0) * invVP)).rgb);
	float lTR = dot(l, texture(albedo[0], inUV + (vec2(1.0, -1.0) * invVP)).rgb);
	float lBL = dot(l, texture(albedo[0], inUV + (vec2(-1.0, 1.0) * invVP)).rgb);
	float lBR = dot(l, texture(albedo[0], inUV + (vec2(1.0, 1.0) * invVP)).rgb);
	float lM  = dot(l, texture(albedo[0], inUV + (vec2(0.0, 0.0) * invVP)).rgb);

	vec2 blurDir; //if near 0,0 then the pixel is not blured
	blurDir.x = -((lTL + lTR) - (lBL + lBR));
	blurDir.y = ((lTL + lBL) - (lTR + lBR));
	
    //scale blurDir so smallest val = 1 texel
	float dirReduce = max((lTL + lTR + lBL + lBR) * (0.25 * FXAA_REDUCE_MUL), FXAA_REDCUE_MIN);
	float blurScale = 1.0 / (min(abs(blurDir.x), abs(blurDir.y)) + dirReduce);

    //cap off the blurDir so it is between 8,8 and -8,-8, also transform into texel space
	blurDir = min(vec2(FXAA_SPAN_MAX, FXAA_SPAN_MAX),
              max(vec2(-FXAA_SPAN_MAX, -FXAA_SPAN_MAX),
              blurDir * blurScale)) * invVP;

	//now we actually blur
	vec3 res1 = (0.5 * (texture(albedo[0], inUV + (blurDir * (1.0/3.0 - 0.5))).xyz + texture(albedo[0], inUV + (blurDir * (2.0/3.0 - 0.5))).xyz));
	vec3 res2 = (texture(albedo[0], inUV + (blurDir * (0.0/3.0 - 0.5))).xyz + texture(albedo[0], inUV + (blurDir * (3.0/3.0 - 0.5))).xyz);
	vec3 result = res1 * 0.5 + res2 * 0.25;

	//if the new luma is too far off, then we use res1
	float luma = dot(l, result);
	float lMin = min(lM, min(min(lTL, lTR), min(lBL, lBR)));
	float lMax = max(lM, max(max(lTL, lTR), max(lBL, lBR)));
	if (luma < lMin || luma > lMax)
		outColor = vec4(res1.rgb, 1.0); //outside of range, use the acutal value
	else
		outColor = vec4(result.rgb, 1.0);//result
}