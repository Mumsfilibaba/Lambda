struct VS_OUT
{
	float4 Position : SV_Position;
};

VS_OUT VSMain(float3 position : POSITION)
{
	VS_OUT output;
	output.Position = float4(position, 1.0f);
	return output;
}

cbuffer PerObject : register(b0)
{
	float4 g_Color;
};

float4 PSMain(VS_OUT input) : SV_Target0
{
	return g_Color;
}