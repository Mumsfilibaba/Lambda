#version 450
#extension GL_ARB_separate_shader_objects : enable

     layout(location = 0) in     vec2 g_TexCoord;
     layout(location = 1) in     vec3 g_Normal;
     layout(location = 2) in     vec3 g_WorldPosition;
flat layout(location = 3) in    vec3 g_ViewPosition;

layout(location = 0) out vec4 g_OutColor;

//Materialbuffer
layout(set = 0, binding = 2) uniform ColorBuffer
{
    vec4 Color;
} u_Material;
//Buffer for light
layout(set = 0, binding = 3) uniform LightBuffer
{
    vec4 Color;
    vec3 Position;
} u_PointLight;
//Diffuse texture
layout(set = 0, binding = 4) uniform texture2D u_Texture;
layout(set = 0, binding = 5) uniform sampler u_Sampler;

const vec3  g_LightDir          = vec3(0.0f, 1.0f, -1.0f);
const float g_SpecularStrength  = 0.5f;
void main()
{
    vec4 tex = texture(sampler2D(u_Texture, u_Sampler), g_TexCoord);
    //Calculate lightning
    float   distance        = length(u_PointLight.Position - g_WorldPosition);
    float   attenuation     = 1.0f / (distance * distance);
    vec3    lightDir        = normalize(u_PointLight.Position - g_WorldPosition);
    float   lightStrength   = max(dot(lightDir, g_Normal), 0.0f);

    vec3    viewDir         = normalize(g_ViewPosition - g_WorldPosition);
    vec3    reflectedDir    = reflect(-lightDir, g_Normal);

    float   specularPow     = pow(max(dot(viewDir, reflectedDir), 0.0f), 128.0f);
    vec3    specularColor   = g_SpecularStrength * specularPow * u_PointLight.Color.rgb * attenuation;
    //Set output color
    vec3 lightColor     = u_PointLight.Color.rgb * lightStrength * attenuation;
    vec3 ambientLight   = vec3(0.15f);
    vec3 objectColor    = tex.rgb * u_Material.Color.rgb;
    vec3 resultColor    = objectColor * (ambientLight + lightColor + specularColor);
    g_OutColor = min(vec4(1.0f), vec4(resultColor, 1.0f));
}
