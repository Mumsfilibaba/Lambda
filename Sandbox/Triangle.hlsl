struct VS_OUT
{
	float4 Position : SV_Position;
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
	return output;
}

cbuffer PSPerObject : register(b0)
{
	float4 g_Color;
};

float4 PSMain(VS_OUT input) : SV_Target0
{
	return g_Color;
}