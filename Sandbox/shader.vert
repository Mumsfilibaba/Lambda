#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in     vec3 a_Position;
layout(location = 1) in     vec3 a_Normal;
layout(location = 2) in     vec3 a_Tangent;
layout(location = 3) in     vec2 a_TexCoord;

layout(location = 0) out    vec2 g_TexCoord;
layout(location = 1) out    vec3 g_Normal;
layout(location = 2) out    vec3 g_Tangent;
layout(location = 3) out    vec3 g_BiTangent;
layout(location = 4) out    vec3 g_WorldPosition;
layout(location = 5) out    vec3 g_ViewPosition;

//Camerabuffer
layout(set = 0, binding = 0) uniform CameraBuffer
{
    mat4 View;
    mat4 Projection;
    vec3 Position;
} u_Camera;
//Model transformbuffer
layout(set = 0, binding = 1) uniform TransformBuffer
{
    mat4 Model;
} u_Transform;

void main()
{
    vec3 position = a_Position;
    //Camera position
    g_ViewPosition = u_Camera.Position;
    //Vertexposition
    vec4 worldPosition  = u_Transform.Model * vec4(position, 1.0);
    g_WorldPosition     = worldPosition.xyz;
    gl_Position         = u_Camera.Projection * u_Camera.View * worldPosition;
    //UV
    g_TexCoord  = a_TexCoord;
    //Normal
    g_Normal    = normalize(u_Transform.Model * vec4(a_Normal, 0.0f)).xyz;
    //Tangent
    g_Tangent   = normalize(u_Transform.Model * vec4(a_Tangent, 0.0f)).xyz;
    //Bitangent
    g_BiTangent = cross(g_Normal, g_Tangent);
}
