#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in     vec3 a_Position;
layout(location = 1) in     vec3 a_Normal;
layout(location = 2) in     vec2 a_TexCoord;

layout(location = 0) out    vec2 g_TexCoord;

layout(set = 0, binding = 0) uniform CameraBuffer
{
    mat4 View;
    mat4 Proj;
} u_Camera;

layout(set = 0, binding = 1) uniform TransformBuffer
{
    mat4 Model;
} u_Transform;


void main()
{
    vec3 position = a_Position;
    //position.y = -position.y;
    
    gl_Position = u_Camera.Proj * u_Camera.View * u_Transform.Model * vec4(position, 1.0);
    g_TexCoord = a_TexCoord;
}
