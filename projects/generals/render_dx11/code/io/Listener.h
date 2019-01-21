#pragma once

// перечисления для слушателя клавиатуры и мыши
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

// структуры, хранящие данные клавиатуры и мыши

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


class Listener // класс - слушатель(наблюдатель)
{
public:
	// пока не используем чистые виртуальные функции(не требует обязательной реализации
	// методов)
	virtual void KeyPressed(const KeyCode& kc) {} // нажата кнопка
	virtual void KeyUnPressed(const KeyCode& kc) {} // отпущена кнопка
	virtual void MouseMove(const MouseData& kc) {} // мыше передвинута
	virtual void MousePressed(const MouseData& kc) {} // нажата клавиша мыши
	virtual void MouseUnPressed(const MouseData& kc) {} // отпущена клавиша мыши
	virtual void MouseWheel(const MouseData& kc) {} // колесико мыши прокручено
};
