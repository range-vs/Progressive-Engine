#include "Window.h"

bool Window::Init(HINSTANCE hInst, const wchar_t * name, const wchar_t * _class, int x, int y, int w, int h, const HWND& hwnd_main, int style)
{
	ClassName = _class;
	this->hInst = hInst;

	if (!Ls)
	{
		char buf[150];
		WideCharToMultiByte(1251, 0, ClassName.c_str(), -1,
			buf, 150, NULL, NULL); // unicode в ascii
		LOGGINGERR("Error create list listeners: no memory");
		return false;
	}

	WNDCLASSEX wnd;
	ZeroMemory(&wnd, sizeof WNDCLASSEX);
	wnd.cbSize = sizeof WNDCLASSEX;
	wnd.hInstance = hInst;
	wnd.lpfnWndProc = Window::WindowProc;
	wnd.lpszClassName = _class;
	wnd.style = CS_HREDRAW | CS_VREDRAW;
	wnd.hCursor = LoadCursor(NULL, IDC_CROSS);
	if (!RegisterClassEx(&wnd)) // не создан класс
	{
		char buf[150];
		WideCharToMultiByte(1251, 0, ClassName.c_str(), -1,
			buf, 150, NULL, NULL); // unicode в ascii
		string out("Error register class \""); out += buf;
		out += "\"";
		LOGGINGERR(out);
		return false;
	}

	hwnd = CreateWindowEx(NULL, _class, name, style, x, y, w, h, hwnd_main, NULL,
		hInst, (LPVOID)this);
	if (!hwnd)
	{
		string line = "Error create window";
		LOGGINGERR(line);
		return false;
	}
	Window::hwnd = hwnd;
	return true;
}

LRESULT CALLBACK Window::WindowProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam)
{
	//LOGGING
	static Window* w(nullptr);
	CREATESTRUCT* data;

	switch (message)
	{
	case WM_CREATE:
		data = (CREATESTRUCT*)lparam;
		w = (Window*)data->lpCreateParams;
		break;

	case WM_SIZE:
	{
		if (Device->getContext())
		{
			SIZE p;
			p.cx = LOWORD(lparam);
			p.cy = HIWORD(lparam);
			if (p.cx == 0 || p.cy == 0)
				break;
			if (!Device->resizeWindow(p)) // обработать ошибки
			{
				PostQuitMessage(0);
			}
		}
		break;
	}

	case WM_ADDMODELMOUSE:
	{
		DataNewModel *data = (DataNewModel*)wparam;
		Coordinate2D coord(data->x, data->y);
		Device->addModel(data->path, &coord, nullptr, nullptr, nullptr, nullptr, nullptr);
		Device->runFrustum();
		break;
	}

	case WM_RAYPICK:
	{
		POINT *mouseCoordinate = (POINT*)wparam;
		Device->selectModel(mouseCoordinate->x, mouseCoordinate->y);
		break;
	}

	case WM_RENDER:
	{
		Device->renderEditor();
		break;
	}

	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;

	case WM_MOUSEMOVE: case WM_LBUTTONUP: case WM_LBUTTONDOWN: case WM_MBUTTONUP: 
	case WM_MBUTTONDOWN: case WM_RBUTTONUP: case WM_RBUTTONDOWN: case WM_MOUSEWHEEL: 
	case WM_KEYDOWN: case WM_KEYUP:
	{
		w->Ls->Run(message, wparam, lparam); // отправляем слушателям событие
		break;
	}

	default:
		break;
	}
	return DefWindowProc(hwnd, message, wparam, lparam);
}

WPARAM Window::RunEditor(UINT style)
{
	ShowWindow(hwnd, style);
	MSG msg;
	while (true)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		if (msg.message == WM_QUIT)
			break;
		else
			//SendMessage(Device->getDesctiptor(), WM_RENDER, NULL, NULL);
			Device->renderEditor();
	}
	return msg.wParam;
}

WPARAM Window::RunGame(UINT style)
{
	ShowWindow(hwnd, style);
	MSG msg;
	while (true)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		if (msg.message == WM_QUIT)
			break;
		else
			//SendMessage(Device->getDesctiptor(), WM_RENDER, NULL, NULL);
			Device->renderGame();
	}
	return msg.wParam;
}

void Window::Release()
{
	//Device->Release();
	DestroyWindow(hwnd);
	UnregisterClass(ClassName.c_str(), hInst);
	//delete Graphics::getInstance();
}

void Window::AddListener(Listener * l)
{
	Ls->Add(l);
}