#include "Console.h"

bool Console::InitConsole(HWND cons)
{
	this->hwnd = cons;
	return true;
}

Console Console::console(L"console.exe", "Channel_dx_and_dxconsole");
// TODO: изменить местоположение консоли на корневое, переписать код
// вывести исключение, если консоль не обнаружена

void Console::DisposeConsole()
{
	
}