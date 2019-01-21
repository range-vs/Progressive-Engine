#include "Light.h"

DirectionLight::DirectionLight(const Point & d, const Color& c)
{
	direction = d;
	color = c;
}

ConstantBufferDirectionalLightPS DirectionLight::GenerateLightData()
{
	// генерируем данные о направлееном источнике освещения
	ConstantBufferDirectionalLightPS ld;
	ZeroMemory(&ld, sizeof ld);
	ld.Direction = (Point4)direction;
	ld.Color = color;
	// константы для шейдерной обработки
	ld.factors[x] = 0.8f;// 0.11f;
	ld.factors[y] = 0.8f;
	return ld;
}
