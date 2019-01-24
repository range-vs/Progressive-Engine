#pragma once

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <fstream>
#include <sstream>
#include <shellapi.h>

#ifdef DISCRETE // определен в настройках препроцессора в свойствах(у discrete_engine)
extern "C"
{
	__declspec(dllexport) int AmdPowerXpressRequestHighPerformance;
	// = 1;
}

extern "C"
{
	__declspec(dllexport) DWORD NvOptimusEnablement;
	// = 0x00000001;
}
#endif

using namespace gui;

using FuncAdr = bool * (const std::string&);
using StartGame = UINT (HINSTANCE);
using FuncWriteFile = void*();

bool isMonitor(const wchar_t* devName, HINSTANCE*);
bool WriteFile(HINSTANCE*);