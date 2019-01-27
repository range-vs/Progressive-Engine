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
	static HHOOK ExistingKeyboardProc;

public:
	Window() : hwnd(NULL), hInst(NULL), ClassName(), Ls(new ListenerSource){};
	bool init(HINSTANCE hInst, const wchar_t* name, const wchar_t* _class, int x, int y, int w, int h, const HWND& hwnd_main, int style);
	static LRESULT CALLBACK windowProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam);
	static LRESULT CALLBACK keyboardProcLowLevel(int nCode, WPARAM wParam, LPARAM lParam);
	bool hookKeyboardProc(HINSTANCE hinst);
	int unHookKeyboardProc();
	WPARAM runEditor();
	WPARAM runGame();
	void release();
	void addListener(Listener* l);
	HWND& getDescriptor() { return this->hwnd; };
	void showWindow(UINT m);
};
