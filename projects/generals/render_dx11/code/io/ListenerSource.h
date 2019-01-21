#pragma once

#include <list>
#include "Listener.h"
#include <Windows.h>
#include <windowsx.h>

using std::list;

class ListenerSource // ����� - �������� ������
{
	list<Listener*> listeners; // ������ ���� ����������
	void keypressed(const KeyCode & kc); // ������� ������ - ��� ������, ����� ������� ���������. ������ ����������� ������� ������ �����������
	void keyunpressed(const KeyCode& kc);
	void mousemove(const MouseData& md);
	void mousepressed(const MouseData& md);
	void mouseunpressed(const MouseData& md);
	void mousewheel(const MouseData& md);
public:
	void Add(Listener* l); // ��������� ���������
	void Run(UINT msg, WPARAM wparam, LPARAM lparam); // �����, ����������� ������� �� �������� ���������
};