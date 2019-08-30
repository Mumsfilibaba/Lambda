#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in     vec2 g_TexCoord;
layout(location = 1) in     vec3 g_Normal;

layout(location = 0) out vec4 g_OutColor;

layout(set = 4, binding = 0) uniform ColorBuffer
{
    vec4 Color;
} u_Material;

layout(set = 4, binding = 8) uniform texture2D u_Texture;
layout(set = 4, binding = 16) uniform sampler u_Sampler;


const vec3 g_LightDir = vec3(0.0f, 1.0f, -1.0f);

void main()
{
    vec4 tex = texture(sampler2D(u_Texture, u_Sampler), g_TexCoord);
    
    vec3  lightDir = normalize(g_LightDir);
    float lightStrength = dot(lightDir, g_Normal);
    const float ambientLight = 0.15f;
    
    vec4 col = tex * u_Material.Color;
    g_OutColor = min(vec4(1.0f), (ambientLight + lightStrength) * col);
}
