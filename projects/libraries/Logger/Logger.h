#pragma once

#define WIN32_LEAN_AND_MEAN

#include <string>
#include <sstream>
#include <time.h>
#include <Windows.h>
#include <fstream>
#include <algorithm>

#include "..\..\generals\render_dx11\code\utilites\macros_general.h"

using std::string;
using std::stringstream;
using std::ofstream;

class Logger
{
	static string name_file; // имя лога
	static ofstream file; // файл лога
	static HWND hwnd_console_log; // дескриптор консоли вывода

	string clearFuncName(const char* fn);
	void _log(const char* f, string func, int l, const char* message, int error);
public:
	Logger(const char* f, const char* func, int l, const char* message, int error = 0);
	Logger(const char* f, const char* func, int l, string message, int error = 0);
	static bool InitLoggingSystem(const char* n, const HWND& hwnd);
	static void DestroyLoggindSystem();
};

// сообщение
#define LOGGING(message) Logger LocalVariableLogger(__FILE__, __FUNCSIG__, __LINE__, message, -1);

#ifndef NDEBUG
// ошибка с окном
#define LOGGINGERR(message) Logger LocalVariableLogger(__FILE__, __FUNCSIG__, __LINE__, message);
#else
// ошибка без окна
#define LOGGINGERR(message) Logger LocalVariableLogger(__FILE__, __FUNCSIG__, __LINE__, message, 1);
#endif