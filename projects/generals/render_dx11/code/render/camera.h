#pragma once

#include "..\utilites\types.h"
#include "../io/Listener.h"

using namespace gmath;

class Camera
{
	// данные камеры
	Matrix view;
	Vector position;
	Vector direction;
	Vector height;

	// настройки положения камеры
	float theta;
	float phi;
	float radius;
	POINT lastMousePos;

public:
	Camera() {};
	void Init(const float& t, const float& p, const float& r); // инициализируем камеру
	void setMauseLastPosition(const POINT& p) { this->lastMousePos = p; } // устанавливаем позицию нажатия мышки
	Vector getViewPosition()const; // получаем текущую позицию
	Vector getViewDirection()const; // получаем текущее направление
	Vector getViewHeight() const; // получаем текущую высоту
	void updatePosition(); // обновляем камеру
	void rePosition(const MouseData& md); // изменяем позицию камеры(вызывается из MouseMove)
	void rePositionWheel(const MouseData& md); // изменяем позицию камеры(вызывается из MouseWheel)
	Matrix getView()const { return view; }
};

// TODO: придумать ограничение для макс. прокрутки камеры(пока что 25.000)