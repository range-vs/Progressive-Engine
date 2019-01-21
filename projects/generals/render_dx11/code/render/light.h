#pragma once

#include "..\utilites\types.h"

class Light
{
protected:
	Color color; // r,g,b,a
	//Model3D pos;
	bool enabled; // включен/выключен свет
public:
	virtual void Enable(bool) = 0;
	virtual bool isEnable() = 0;
	virtual ConstantBufferDirectionalLightPS GenerateLightData() = 0;
	//virtual void setModel(const Model3D& m) = 0;
	//virtual ~Light() = 0;
};

class DirectionLight : public Light
{
	Point direction; // направление света
public:
	DirectionLight(const Point& d, const Color& c);
	void Enable(bool en) { enabled = en; }
	ConstantBufferDirectionalLightPS GenerateLightData();
	bool isEnable() { return enabled; }
	//void setModel(const Model3D& m) { this->pos = m; }
	//~DirectionLight();
};
