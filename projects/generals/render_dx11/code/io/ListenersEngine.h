#pragma once

#include "ListenerSource.h"

class MKListener : public Listener
{
public:
	void MousePressed(const MouseData& kc); // ������ ������� ����
	void MouseUnPressed(const MouseData& kc); // �������� ������� ����
	void MouseMove(const MouseData& kc); // ���� �����������
	void MouseWheel(const MouseData& md); // ��������� ��������� �������
};

class SelectObjectListener : public Listener
{
public:
	void KeyPressed(const KeyCode& kc); // ��������� ����� ������� �������
	void KeyUnPressed(const KeyCode& kc); // ��������� ����� ������� �������
	void MousePressed(const MouseData& kc); // ��������� ����� ������� ������
};