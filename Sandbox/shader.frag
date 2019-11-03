#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) out vec4 g_OutColor;

layout(location = 0) in    vec2 g_TexCoord;
layout(location = 1) in    vec3 g_Normal;
layout(location = 2) in    vec3 g_Tangent;
layout(location = 3) in    vec3 g_BiTangent;
layout(location = 4) in    vec3 g_WorldPosition;
layout(location = 5) in    vec3 g_ViewPosition;

//Materialbuffer
layout(set = 0, binding = 2) uniform MaterialBuffer
{
    vec4 Color;
    int HasAlbedoMap;
    int HasNormalMap;
} u_Material;
//Buffer for light
layout(set = 0, binding = 3) uniform LightBuffer
{
    vec4 Color;
    vec3 Position;
} u_PointLight;


//Diffuse texture
layout(set = 0, binding = 4) uniform texture2D  u_Albedo;
layout(set = 0, binding = 5) uniform texture2D  u_Normal;
layout(set = 0, binding = 6) uniform sampler    u_Sampler;


void main()
{
    //Get albedo
    vec4 albedo;
    if (u_Material.HasAlbedoMap == 1)
    {
        albedo = texture(sampler2D(u_Albedo, u_Sampler), g_TexCoord) * u_Material.Color;
    }
    else
    {
        albedo = u_Material.Color;
    }

    //Get normal
    vec3 normal;
    if (u_Material.HasNormalMap == 1)
    {
        vec4 normalMap  = texture(sampler2D(u_Normal, u_Sampler), g_TexCoord);
        normal          = normalize(normalMap.rgb * 2 - 1.0f);
        normal          = normalize(mat3(g_Tangent, g_BiTangent, g_Normal) * normal);
    }
    else
    {
        normal = normalize(g_Normal);
    }

    //Calculate lightning
    float   distance        = length(u_PointLight.Position - g_WorldPosition);
    float   attenuation     = 1.0f / (distance);
    vec3    lightDir        = normalize(u_PointLight.Position - g_WorldPosition);
    float   lightStrength   = max(dot(lightDir, normal), 0.0f);

    vec3    viewDir         = normalize(g_ViewPosition - g_WorldPosition);
    vec3    halfDir         = normalize(lightDir + viewDir);

    float   specularPow     = pow(max(dot(normal, halfDir), 0.0f), 256.0f);
    vec3    specularColor   = specularPow * u_PointLight.Color.rgb * attenuation;

    //Set output color
    vec3 lightColor     = u_PointLight.Color.rgb * lightStrength * attenuation;
    vec3 ambientLight   = vec3(0.1f);
    vec3 objectColor    = albedo.rgb;
    vec3 resultColor    = objectColor * (ambientLight + lightColor + specularColor);
    g_OutColor = min(vec4(1.0f), vec4(resultColor, 1.0f));
}
