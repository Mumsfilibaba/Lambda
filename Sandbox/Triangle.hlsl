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

float4 PSMain(VS_OUT input) : SV_Target0
{
	return float4(1.0f, 0.0f, 0.0f, 1.0f);
}