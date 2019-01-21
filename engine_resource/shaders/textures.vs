cbuffer ConstantBufferVS: register(b0)
    { 
		matrix Position;
		matrix Rotation;
		matrix Scale;
    	matrix View; 
    	matrix Perspective; 
    }    

    struct VS_INPUT 
    { 
        float3 Pos: POSITION; 
	float3 TexCoord: TEXCOORD;
	float3 Normal: NORMAL;
    }; 

    struct VS_OUTPUT 
    { 
        float4 Pos: SV_POSITION; 
	float3 TexCoord: TEXCOORD0;
    	float3 Normal: TEXCOORD1; 
    }; 

    VS_OUTPUT main(VS_INPUT input) 
    { 
        VS_OUTPUT _out = (VS_OUTPUT)0;
		_out.Pos = mul(float4(input.Pos, 1), Rotation);	
		_out.Pos = mul(_out.Pos, Scale);
		_out.Pos = mul(_out.Pos, Position);
        _out.Pos = mul(_out.Pos, View); 
        _out.Pos = mul(_out.Pos, Perspective); 
		_out.Normal = input.Normal;
        _out.TexCoord = input.TexCoord;
        return _out; 
    }