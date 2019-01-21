#pragma once

#include "ListenerSource.h"

class MKListener : public Listener
{
public:
	void MousePressed(const MouseData& kc); // нажата клавиша мыши
	void MouseUnPressed(const MouseData& kc); // отпущена клавиша мыши
	void MouseMove(const MouseData& kc); // мыше передвинута
	void MouseWheel(const MouseData& md); // реализуем прокрутку колЄсика
};

class SelectObjectListener : public Listener
{
public:
	void KeyPressed(const KeyCode& kc); // реализуем метод Ќј∆ј“»я клавиши
	void KeyUnPressed(const KeyCode& kc); // реализуем метод ќ“∆ј“»я клавиши
	void MousePressed(const MouseData& kc); // реализуем метод Ќј∆ј“»я мышкой
};