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
	StructuredBuffer<float3> arrayCenter: register(t3);
	
	// запись
	RWStructuredBuffer<int> outputBuffer: register(u0);
	
	// константы
	static const int PLANES_SIZE = 6;
	static const int NEARP = 5;
	
	// константные буферы
	
	struct PlanesFrustum
	{
		float4 pl;
	};

	cbuffer FustrumData: register(b0)
    { 
		PlanesFrustum planes[PLANES_SIZE];
		float4 centerNear;
		int countModels;
    };
	
	
	// функции
	float shortDistanseFromPointToPlane(float4 p, float3 p0) // функция определения кратчайшего расстояния от точки до плоскости
	{
		return p.x*p0.x + p.y*p0.y + p.z*p0.z + p.w; // если расстрояние до плоскости < 0 - точка сзади(вектор смотрит от точки), иначе точка спереди(векто смотрит на точку),
		// == 0 - точка на плоскости
	}
	
	int testBBoxInFrustum(float3 p0) // тест ббокс на попадание
	{
		int isFrustum = 0;
		for(int l = 0; l<PLANES_SIZE;l++) // перебираем все плоскости фрустума
		{
			float4 n = planes[l].pl;
			float value = shortDistanseFromPointToPlane(n, p0);
			if(value > 0.f) // точка попала за плоскость // >=
				++isFrustum;
		}
		return isFrustum;
	}
	
	int testBSphereInFrustum(float3 p0, float3 cen) // тест сферы на попадание
	{
		// длина радиуса должна быть больше расстояния от центра до передней плоскости
		float radius = length(p0 - cen);
		float sCenterToPlaneNearCenter = length((float3)centerNear - cen); // расстояние от центра до плоскости | shortDistanseFromPointToPlane(n, cen) | sPointForPlane((float3)n, cen, n.w)
		int res = sCenterToPlaneNearCenter <= radius ? PLANES_SIZE : 0;
		return res;
	}
	
	float3 getCurrentPoint(float3 p0, int indexMatrix) // получаем точку из массива
	{
		p0 = (float3)mul(float4(p0, 1), arrayMatrix[indexMatrix].sca);
		p0 = (float3)mul(float4(p0, 1), arrayMatrix[indexMatrix].rot);
		p0 = (float3)mul(float4(p0, 1), arrayMatrix[indexMatrix].pos);
		return p0;
	}
	
	
	// стартовая функция
	[numthreads(1, 1, 1)]
	void main(uint3 groupThreadID : SV_GroupThreadID)
	{
		int indexMatrix = 0;
		float3 p0;
		for(int i = 0, offsetModel = 0;i < countModels;i++) // перебираем все модели
		{
			for(int j = 0;j<arrayOffset[i];j++) // берем все матрицы у модели
			{
				int isFrustum = 0; // количество плоскостей, в которые точка попадает(за какое количество плоскостей)
				for(int k = 0;k<8;k++) // берём все восемь вершин у модели
				{
					p0 = getCurrentPoint(arrayPoints[offsetModel * 8 + k], indexMatrix);
					// теперь надо проверить эту вершину со всеми 6-ю плоскостями
					isFrustum = testBBoxInFrustum(p0); // тестим точку на попадание
					if (isFrustum == PLANES_SIZE)// точка попала в фрустум полностью
						break;
					isFrustum = 0; // обнуляем счётчик попаданий
				}
				if(isFrustum != PLANES_SIZE)
				{
					float3 c = getCurrentPoint(arrayCenter[i], indexMatrix); // получаем центр
					isFrustum = testBSphereInFrustum(p0, c); // сфера - тест
				}
				outputBuffer[indexMatrix] = isFrustum == PLANES_SIZE ? 1 : 0; // если 0 - то вне фрустума, иначе 1 - внутри фруструма
				++indexMatrix;
			}
			if(arrayOffset[i] != 0)
				offsetModel++;
		}
	}
	
	
	