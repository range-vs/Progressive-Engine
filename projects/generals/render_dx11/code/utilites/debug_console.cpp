#include "debug_console.h"

void debug_console::init(const wchar_t * nTitle)
{
	isInit = true;
	AllocConsole();
	SetConsoleTitleW(nTitle);
	FILE* stream;
	freopen_s(&stream, "CONOUT$", "w", stdout);
	std::cout << "Console initialization" << std::endl;
}

void debug_console::init(const char * nTitle)
{
	isInit = true;
	AllocConsole();
	SetConsoleTitleA(nTitle);
	FILE* stream;
	freopen_s(&stream, "CONOUT$", "w", stdout);
	std::cout << "Console initialization" << std::endl;
}

void debug_console::setDataPrint(const char * data)
{
	if (!isInit)
		return;
	std::cout << data << std::endl;
}

void debug_console::setDataPrintW(const wchar_t * data)
{
	if (!isInit)
		return;
	std::wcout << data << std::endl;
}
