    // структурные буферы
	// чтение
	struct AllTransformations
    {
        matrix pos;
		matrix rot;
		matrix sca;
    };
	
    StructuredBuffer<AllTransformations> arrayMatrix: register(t0);
    StructuredBuffer<int> arrayOffset: register(t1);
	StructuredBuffer<float3> arrayPoints: register(t2);
	
	// запись
	RWStructuredBuffer<int> outputBuffer: register(u0);
	
	// константные буферы
	cbuffer FustrumData: register(b0)
    { 
		//int countMatrix;
		float4 planes[6];
		int countModels;
    };
	
	
	// функции
	float shortDistanseFromPointToPlane(float4 p, float3 p0) // функция определения кратчайшего расстояния от точки до плоскости
	{
		return p.x*p0.x + p.y*p0.y + p.z*p0.z + p.w*1; // если расстрояние до плоскости < 0 - точка сзади(вектор смотрит от точки), иначе точка спереди(векто смотрит на точку),
		// == 0 - точка на плоскости
	}
	
	// стартовая функция
	[numthreads(1, 1, 1)]
	void main(uint3 groupThreadID : SV_GroupThreadID)
	{
		int indexMatrix = 0;
		for(int i = 0, offsetModel = 0;i < countModels;i++) // перебираем все модели
		{
			for(int j = 0;j<arrayOffset[i];j++) // берем все матрицы у модели
			{
				int isFrustum = 0; // количество плоскостей, в которые точка попадает(за какое количество плоскостей)
				for(int k = 0;k<8;k++) // берём все восемь вершин у модели
				{
					float3 p0 = arrayPoints[offsetModel * 8 + k];
					p0 = (float3)mul(float4(p0, 1), arrayMatrix[indexMatrix].sca);
					p0 = (float3)mul(float4(p0, 1), arrayMatrix[indexMatrix].rot);
					p0 = (float3)mul(float4(p0, 1), arrayMatrix[indexMatrix].pos);
					// теперь надо проверить эту вершину со всеми 6-ю плоскостями
					for(int l = 0; l<6;l++) // перебираем все плоскости фрусутма
					{
						float4 p1 = planes[l];
						float value = shortDistanseFromPointToPlane(p1, p0);
						if(value >= 0.f) // точка попала за плоскость
							++isFrustum;
					}
					if (isFrustum == 6)// точка попала во фрустум полностью
						break;
					isFrustum = 0; // обнуляем счётчик попаданий
				}
				outputBuffer[indexMatrix] = isFrustum == 6 ? 1 : 0; // если 0 - то вне фрустума, иначе 1 - внутри фруструма
				++indexMatrix;
			}
			if(arrayOffset[i] != 0)
				offsetModel++;
		}
	}
	
	
	