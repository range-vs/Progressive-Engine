#pragma once
#include <Windows.h>
#include <string>

using std::wstring;
using std::string;

class Console
{
	HWND hwnd;
	wstring name_process;
	string name_file;
	HANDLE hMapFile;
public:
	static Console console;

	Console(const wstring& np, const string & nf):hwnd(NULL), name_process(np), name_file(nf) {}
	bool InitConsole(HWND cons);
	HWND GetHWND() { return hwnd; }
	void DisposeConsole();
};