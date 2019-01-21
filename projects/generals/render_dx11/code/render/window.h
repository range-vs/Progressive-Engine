#pragma once

#include "../utilites/macros_global.h"

#pragma optimize("", off) 
using std::wstring;

class Window
{
	HWND hwnd;
	HINSTANCE hInst;
	wstring ClassName;
	brain_memory<ListenerSource> Ls;

public:
	Window() : hwnd(NULL), hInst(NULL), ClassName(), Ls(new ListenerSource){};
	bool Init(HINSTANCE hInst, const wchar_t* name, const wchar_t* _class, int x, int y, int w, int h, const HWND& hwnd_main);
	static LRESULT CALLBACK WindowProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam);
	WPARAM RunEditor(UINT style);
	void Release();
	void AddListener(Listener* l);
	HWND& get_hwnd() { return this->hwnd; };
};