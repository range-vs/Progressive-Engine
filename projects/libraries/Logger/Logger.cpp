// Logger.cpp: определяет точку входа для консольного приложения.
//

#include "Logger.h"

// static //

string Logger::name_file;

ofstream Logger::file;

HWND Logger::hwnd_console_log;

bool Logger::InitLoggingSystem(const char * n, const HWND& hwnd)
{
	Logger::hwnd_console_log = hwnd; // запоминаем hwnd консоли, для отображения лога

	time_t t = time(0); // получаем текущее время и дату
	tm now_date; // переменная хранит текущую дату и время
	gmtime_s(&now_date, &t); // записываем текщую дату и время

	Logger::name_file = string(n + string("_") + std::to_string(now_date.tm_mday) + "_" + std::to_string(now_date.tm_mon) + "_" +
		std::to_string(now_date.tm_year) + "__" + std::to_string(now_date.tm_hour) + "_" + std::to_string(now_date.tm_min) + "_" +
		std::to_string(now_date.tm_sec) + ".log"); // создаем имя лога
	file.open(Logger::name_file); // создаем новый лог
	if (!file)
		return false;
	return true;
}

void Logger::DestroyLoggindSystem()
{
	file.close();
}

// local //

Logger::Logger(const char* f, const char* func, int l, const char* message, int error)
{
	_log(f, clearFuncName(func), l, message, error);
}

Logger::Logger(const char * f, const char * func, int l, string message, int error)
{
	_log(f, clearFuncName(func), l, message.c_str(), error);
}

void Logger::_log(const char* f, string func, int l, const char* message, int error)
{
	stringstream format_out;
	char buf[9];
	char buf2[9];
	_strdate_s(buf);
	_strtime_s(buf2);

	format_out << message << std::endl;
	file << format_out.str();
	if (error >= 0)
	{
		format_out << "[" << buf << " " << buf2 << "]   ";
		format_out << "Error program: " << "file name: " << f <<
			", function name: " << func << ", line: " << l << std::endl;
		file << format_out.str();
		SendMessage(Logger::hwnd_console_log, WM_SENDLOGMAINPROGRAM, NULL, (LPARAM)&message); // посылаем сообщение в консоль editor
		if (error == 0) // debug
		{
			if (MessageBoxA(NULL, message, "Fatal Error!", MB_OKCANCEL) == IDCANCEL)
			{
				file.close();
				exit(1);
			}
		}
		else if (error == 1) // release
		{
			file.close();
			exit(1);
		}
	}
	else // message(debug and release)
	{
		file << format_out.str();
		SendMessage(Logger::hwnd_console_log, WM_SENDLOGMAINPROGRAM, NULL, (LPARAM)&message); // посылаем сообщение в консоль editor
	}

	file.close();
}

string Logger::clearFuncName(const char* fn)
{
	string func(fn);
	string arr[] = { "__thiscall", "_cdecl" };
	for (int i(0); i < 2; i++)
	{
		auto iter = std::find_end(func.begin(), func.end(), arr[i].begin(), arr[i].end());
		if (iter != func.end())
			func.erase(iter - 1, iter + arr[i].length() + 1);
	}
	return func;
}