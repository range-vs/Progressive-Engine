#include "Camera.h"
#include <string>

void Camera::Init(const float& t, const float& p, const float& r)
{
	// Камера - добавить переменную-ограничитель по прокрутке камеры(z - коорината вектора позиции)
	theta = t; // поворот вокруг оси y
	phi = p; // поворот вокруг оси x и z
	radius = r; // масштаб
	lastMousePos = { 0, 0 };
	direction = 0;
	height = Vector(0, 1, 0, 0);
	updatePosition();
}

Vector Camera::getViewPosition()const
{
	return this->position;
}

Vector Camera::getViewDirection() const
{
	return this->direction;
}

Vector Camera::getViewHeight() const
{
	return height;
}

void Camera::rePosition(const MouseData & md)
{
	if (md.button == vkLMouse) // определяем, какая кнопка нажата
	{
		auto dx = GeneralMath::degreesToRadians(0.25f * (md.x - lastMousePos.x));
		auto dy = GeneralMath::degreesToRadians(0.25f * (md.y - lastMousePos.y));

		theta += dx;
		phi += dy;

		phi = GeneralMath::clamp(phi, 0.1f, GeneralMath::PI - 0.1f);
	}
	else if (md.button == vkRMouse)
	{
		auto dx = 0.5f * (md.x - lastMousePos.x);
		auto dy = 0.5f * (md.y - lastMousePos.y);
		radius += dx - dy;

		radius = GeneralMath::clamp(radius, 5, 25000.f); // TODO - границы масштаба
	}
	else if (md.button == vkMMouse)
	{
		//direction = { direction[x] + 5, 0, 0 };
		position[x]+=5;
	}
	lastMousePos = { md.x, md.y };
}

void Camera::rePositionWheel(const MouseData& md)
{
	lastMousePos = { md.x, md.y };
	float dx = 0;
	float dy = 0;
	if (md.CountWheel < 0)
	{
		dx = 0.005f * 5000;
		dy = 0.005f * 1000;
	}
	else
	{
		dx = 0.005f * 1000;
		dy = 0.005f * 5000;
	}
	radius += dx - dy;

	radius = GeneralMath::clamp(radius, 5.f, 25000.f); // TODO - границы масштаба
	this->updatePosition();
}

void Camera::updatePosition()
{
	// обновляем координаты относительно нажатия мышки
	float x = radius * (float)sin(phi) * (float)cos(theta);
	float z = radius * (float)sin(phi) * (float)sin(theta);
	float y = radius * (float)cos(phi);
	// Создаем матрицу вида 
	position = Vector(x, y, z, 0);
	view = Matrix::CreateLookAtLHMatrix(position, direction, height);
}