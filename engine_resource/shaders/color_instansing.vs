    // инстансинг 
    struct LogicBufferPosition
    {
        matrix Pos;
		matrix Dir;
		matrix Sca;
    };

    struct LogicBufferBool
    {
        bool flag;
    };

    StructuredBuffer<LogicBufferPosition> arrayPositions: register(t0);
    StructuredBuffer<LogicBufferBool> arrayBools: register(t1);

    cbuffer ConstantBufferVS: register(b0)
    { 
        matrix View; 
        matrix Perspective; 
		float3 center; // центр модели
    };
    
    ////////////////////////////

    struct VS_INPUT 
    { 
        float3 Pos: POSITION; 
		float4 Color: COLOR;
    }; 

    struct GS_INPUT
    { 
        float4 Pos: SV_POSITION; 
		float4 Color: COLOR0;
		bool visible: VISIBLE_FLAG; // отрисовывать ли примитив?
    }; 

    GS_INPUT main(VS_INPUT input, uint id: SV_InstanceID) 
    { 
		GS_INPUT _out = (GS_INPUT)0; 
		if(arrayBools[id].flag == false)
		{
			_out.visible = arrayBools[id].flag;
			return _out;
		}
		
		/*
		float4x4 translateCenter = 
		{
		1.f,0.f,0.f,0.f,
		0.f,1.f,0.f,0.f, 
		0.f,0.f,1.f,0.f,
		-center.x,-center.y,-center.z,1.f
		};
		
		float4x4 translateCenterInverse = 
		{
		1.f,0.f,0.f,0.f,
		0.f,1.f,0.f,0.f,
		0.f,0.f,1.f,0.f,
		center.x,center.y,center.z,1.f
		};
		*/
		
		_out.Pos = float4(input.Pos, 1);  
		//_out.Pos = mul(_out.Pos, translateCenter);
		_out.Pos = mul(_out.Pos, arrayPositions[id].Dir);
		_out.Pos = mul(_out.Pos, arrayPositions[id].Sca);
		//_out.Pos = mul(_out.Pos, translateCenterInverse);
		_out.Pos = mul(_out.Pos, arrayPositions[id].Pos);
		
		_out.Pos = mul(_out.Pos, View); 
		_out.Pos = mul(_out.Pos, Perspective); 
		_out.Color = input.Color;
		_out.visible = arrayBools[id].flag;
		return _out; 
    } 