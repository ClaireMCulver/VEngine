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

// Geometry settings //
layout (points) in;
layout (triangle_strip) out;
layout (max_vertices = 4) out;

// Per Vertex Data //
layout (location = 0) in vec3 inNormal[];
layout (location = 1) in vec2 inUV[];

layout (location = 0) out vec3 outNormal;
layout (location = 1) out vec2 outUV;

void main() 
{
    vec3 right = vec3(perDrawData.mvMatrix[0][0], perDrawData.mvMatrix[1][0], perDrawData.mvMatrix[2][0]);

    vec3 up = vec3(perDrawData.mvMatrix[0][1], perDrawData.mvMatrix[1][1], perDrawData.mvMatrix[2][1]);    

    vec3 P = gl_in[0].gl_Position.xyz;

    vec3 BottomLeft = P - (right + up);
    gl_Position = perFrameData.viewProjectionMatrix * vec4(BottomLeft, 1.0);
    outNormal = inNormal[0];
    outUV = vec2(0, 0);
    EmitVertex();
    
    vec3 TopLeft = P - (right - up);
    gl_Position = perFrameData.viewProjectionMatrix * vec4(TopLeft, 1.0);
    outNormal = inNormal[0];
    outUV = vec2(0, 1);
    EmitVertex();
    
    vec3 rightBottom = P + (right - up);
    gl_Position = perFrameData.viewProjectionMatrix * vec4(rightBottom, 1.0);
    outNormal = inNormal[0];
    outUV = vec2(1, 0);
    EmitVertex();
    
    vec3 rightTop = P + (right + up);
    gl_Position = perFrameData.viewProjectionMatrix * vec4(rightTop, 1.0);
    outNormal = inNormal[0];
    outUV = vec2(1, 1);
    EmitVertex();
    
    EndPrimitive();
}