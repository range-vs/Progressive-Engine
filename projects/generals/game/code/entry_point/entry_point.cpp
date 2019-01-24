#include "entry_point.h"

extern "C" _declspec(dllexport) UINT startGame(HINSTANCE hInst)
{
	UINT res;
	HWND hwndMain(NULL);
	std::thread renderWindow(runRenderWindow, hInst, hwndMain, std::ref(res));
	renderWindow.join();

	// здесь будет дальше работать игра (персонажи и т.п.)
	// верхний поток надо detach

	return res;
}

void runRenderWindow(HINSTANCE hInst, HWND hwnd, UINT& result)
{
	HINSTANCE hLib = LoadLibrary(L"render_dx11.dll");
	if (hLib == NULL) // descr error
		return;
	StartGame* func = (StartGame*)GetProcAddress((HMODULE)hLib, "startGame");
	if (func == NULL) // descr error
		return;
	UINT res = func(hInst, hwnd);
	result = res;
	FreeLibrary((HMODULE)hLib);
}