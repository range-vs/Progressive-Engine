struct GS_INPUT
{ 
    float4 Pos: SV_POSITION; 
	float4 Color: COLOR0;
	bool visible: VISIBLE_FLAG; // отрисовывать ли примитив?
}; 

struct PS_INPUT 
{ 
    float4 Pos: SV_POSITION; 
	float4 Color: COLOR0;
}; 
	
[maxvertexcount(2)]
void main( line GS_INPUT vertex[2], inout LineStream<PS_INPUT> lineStream )
{
	for(int i = 0;i<2;i++)
	{
		if(vertex[i].visible == false)
			return;
		PS_INPUT v = (PS_INPUT)0;
		v.Pos = vertex[i].Pos;
		v.Color = vertex[i].Color;
		lineStream.Append(v);
	}
}