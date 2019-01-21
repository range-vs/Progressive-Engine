#pragma once
#include <Windows.h>
#include <iostream>

class debug_console
{
	bool isInit;
public:
	debug_console():isInit(false){}
	void init(const wchar_t* nTitle);
	void init(const char* nTitle);
	void setDataPrint(const char* data);
	void setDataPrintW(const wchar_t* data);
};