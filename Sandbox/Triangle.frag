#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 g_Color;
layout(location = 0) out vec4 g_OutColor;

void main()
{
    g_OutColor = vec4(1.0, 0.0, 0.0, 1.0);
}
