struct VS_OUT
{
	float4 Position : SV_Position;
	float2 TexCoord : TEXCOORD0;
};

cbuffer VSPerFrame : register(b0)
{
	float4x4 g_View;
	float4x4 g_Proj;
};

VS_OUT VSMain(float3 position : POSITION)
{
	float4x4 camera = mul(g_View, g_Proj);

	VS_OUT output;
	output.Position = mul(camera, float4(position, 1.0f));
	output.TexCoord = float2(position.x + 0.5f, position.y + 0.5f);
	return output;
}

cbuffer PSPerObject : register(b0)
{
	float4 g_Color;
};

Texture2D g_Texture : register(t0);
SamplerState g_Sampler : register(s0);

float4 PSMain(VS_OUT input) : SV_Target0
{
	return g_Texture.Sample(g_Sampler, input.TexCoord) * g_Color;
}