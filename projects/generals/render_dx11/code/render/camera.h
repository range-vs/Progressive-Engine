#pragma once

#include "..\utilites\types.h"
#include "../io/Listener.h"

using namespace gmath;

class Camera
{
	// ������ ������
	Matrix view;
	Vector position;
	Vector direction;
	Vector height;

	// ��������� ��������� ������
	float theta;
	float phi;
	float radius;
	POINT lastMousePos;

public:
	Camera() {};
	void Init(const float& t, const float& p, const float& r); // �������������� ������
	void setMauseLastPosition(const POINT& p) { this->lastMousePos = p; } // ������������� ������� ������� �����
	Vector getViewPosition()const; // �������� ������� �������
	Vector getViewDirection()const; // �������� ������� �����������
	Vector getViewHeight() const; // �������� ������� ������
	void updatePosition(); // ��������� ������
	void rePosition(const MouseData& md); // �������� ������� ������(���������� �� MouseMove)
	void rePositionWheel(const MouseData& md); // �������� ������� ������(���������� �� MouseWheel)
	Matrix getView()const { return view; }
};

// TODO: ��������� ����������� ��� ����. ��������� ������(���� ��� 25.000)