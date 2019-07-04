#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 g_Color;
layout(location = 0) out vec4 g_OutColor;

layout(set = 4, binding = 0) uniform ColorBuffer
{
    vec4 Color;
} material;

void main()
{
    g_OutColor = material.Color;
}
