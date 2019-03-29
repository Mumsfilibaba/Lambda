struct VS_OUT
{
	float4 Position : SV_Position;
	float2 TexCoord : TEXCOORD0;
};

VS_OUT VSMain(uint VertexID: SV_VertexID)
{
	VS_OUT output;

	output.TexCoord = float2((VertexID << 1) & 2, VertexID & 2);
	output.Position = float4(output.TexCoord * float2(0.5f, -0.5f) + float2(-0.5f, 0.5f), 0.0f, 1.0f);

	return output;
}

float4 PSMain(VS_OUT input) : SV_Target0
{
	return float4(1.0f, 0.0f, 0.0f, 1.0f);
}