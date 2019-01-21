#pragma once

// ������������ ��� ��������� ���������� � ����
enum KeyCodes
{
	vkEsc = 0x1B,
	vkHome = 0x24,
	vkLeft = 0x25,
	vkRight = 0x27,
	vkShift = 0x10,
	vkControl = 0x11
};

enum CharCodes
{

};

enum MouseCodes
{
	vkLMouse = 0x01,
	vkRMouse = 0x02,
	vkMMouse = 0x0010
};

// ���������, �������� ������ ���������� � ����

struct KeyCode
{
	unsigned virtualKey;
	wchar_t charKey;
	KeyCode() :virtualKey(-1), charKey(-1) {  }
};

struct MouseData
{
	int x;
	int y;
	int CountWheel;
	unsigned button;
	MouseData() :x(0), y(0), CountWheel(0), button(0) {  }
};


class Listener // ����� - ���������(�����������)
{
public:
	// ���� �� ���������� ������ ����������� �������(�� ������� ������������ ����������
	// �������)
	virtual void KeyPressed(const KeyCode& kc) {} // ������ ������
	virtual void KeyUnPressed(const KeyCode& kc) {} // �������� ������
	virtual void MouseMove(const MouseData& kc) {} // ���� �����������
	virtual void MousePressed(const MouseData& kc) {} // ������ ������� ����
	virtual void MouseUnPressed(const MouseData& kc) {} // �������� ������� ����
	virtual void MouseWheel(const MouseData& kc) {} // �������� ���� ����������
};
