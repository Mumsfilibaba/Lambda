#version 450

layout(location = 0) in     vec3 a_Position;
layout(location = 1) in     vec2 a_TexCoord;

layout(location = 0) out    vec2 g_TexCoord;

void main()
{
    vec3 position = a_Position;
    //position.y = -position.y;
    
    gl_Position = vec4(position, 1.0);
    g_TexCoord = a_TexCoord;
}
