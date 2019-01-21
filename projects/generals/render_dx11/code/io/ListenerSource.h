#pragma once

#include <list>
#include "Listener.h"
#include <Windows.h>
#include <windowsx.h>

using std::list;

class ListenerSource // класс - источник данных
{
	list<Listener*> listeners; // список всех слушателей
	void keypressed(const KeyCode & kc); // скрытые методы - для выбора, какое событие произошло. Аналог виртуальных функций класса наблюдатель
	void keyunpressed(const KeyCode& kc);
	void mousemove(const MouseData& md);
	void mousepressed(const MouseData& md);
	void mouseunpressed(const MouseData& md);
	void mousewheel(const MouseData& md);
public:
	void Add(Listener* l); // добавляем слушателя
	void Run(UINT msg, WPARAM wparam, LPARAM lparam); // метод, принимающий событие из внешнего источника
};