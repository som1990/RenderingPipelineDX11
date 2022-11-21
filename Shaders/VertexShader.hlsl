struct VSOut
{
	float3 color : COLOR;
	float4 pos: SV_POSITION;
};

VSOut main( float4 pos : POSITION, float3 color: COLOR )
{
	VSOut vsOut;
	vsOut.pos = pos;
	vsOut.color = color;
	return vsOut;
}