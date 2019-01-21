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
	
[maxvertexcount(3)]
void main( triangle GS_INPUT vertex[3], inout TriangleStream<PS_INPUT> triStream )
{
	for(int i = 0;i<3;i++)
	{
		if(vertex[i].visible == false)
			return;
		PS_INPUT v = (PS_INPUT)0;
		v.Pos = vertex[i].Pos;
		v.Color = vertex[i].Color;
		triStream.Append(v);
	}
}