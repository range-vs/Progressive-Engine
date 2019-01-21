#pragma once

#include "..\entry_point\framework.h"

// экспортированные функции из вспомогательной dll для обмена информацией с редактором

// псевдонимы
template<class T>  using _function = char*(T, Graphics*); // псевдоним функции

// функции
// меню "Файл"
DLLExport char* OpenMap(const char* name_map); // открытие карты
DLLExport char* CloseMap(const char* name_map); // закрываем карту

// меню "Инструменты"
DLLExport char* ReShowNet(); // показ / сокрытие сетки
DLLExport char* ReMode(int mode); // смена режима отрисовки

// блок манипуляторов(слева)
DLLExport char* SetModeManipulations(int mode); // задаем режим управления сценой
DLLExport char* PastModelForMap(char* path, int x, int y); // установка модели на карту

// функции управления окном
DLLExport char* ResizeRenderWindow(int* size); // изменяем размер окна
DLLExport char* PressKey(int keyCode); // отлавливаем нажатие клавиши
DLLExport char* UnPressKey(int keyCode); // отлавливаем отпускание клавиши



// общие функции
template<class Function, class T>
char* ReadFunctionForDLL(const char* dll_name, T function_name, Function* func)
{
	HINSTANCE hLib = LoadLibraryA(dll_name); // загружаем нужную dll
	if (hLib == NULL) // проверяем, что dll загрузилась корректно
	{
		// если нет - пишем в лог - файл + сообщение на экран
		// и возвращаем ошибку в вызывающую программу
	}
	func = (Function*)GetProcAddress((HMODULE)hLib, function_name); // загружаем функцию из dll
	if (func == NULL)
	{
		// если функция не загрузилась - пишем в лог - файл + сообщение на экран
		// и возвращаем ошибку в вызывающую программу
	}
	return ""; 
}