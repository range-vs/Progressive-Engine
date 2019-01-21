cbuffer ConstantBufferVS: register(b0)
{
	matrix Position;
	matrix Rotation;
	matrix Scale;
	matrix View;
	matrix Perspective;
}

struct VS_OUTPUT
{
	float4 Pos: SV_POSITION;
	float4 Color: COLOR0;
};

struct VS_INPUT
{
	float3 Pos: POSITION;
	float4 Color: COLOR;
};

VS_OUTPUT main(VS_INPUT input)
{
	VS_OUTPUT _out = (VS_OUTPUT)0;
	_out.Pos = mul(float4(input.Pos, 1), Rotation);	
	_out.Pos = mul(_out.Pos, Scale);
	_out.Pos = mul(_out.Pos, Position);
	_out.Pos = mul(_out.Pos, View);
	_out.Pos = mul(_out.Pos, Perspective);
	_out.Color = input.Color;
	return _out;
}