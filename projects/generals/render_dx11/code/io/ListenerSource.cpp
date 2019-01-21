#include "ListenerSource.h"

void ListenerSource::keypressed(const KeyCode & kc)
{
	for (auto & e : listeners) // перебираем всех слушаетелей, и у каждого вызываем
		e->KeyPressed(kc); // нужный метод с текущими данными
}

void ListenerSource::keyunpressed(const KeyCode & kc)
{
	for (auto & e : listeners)
		e->KeyUnPressed(kc);
}

void ListenerSource::mousemove(const MouseData & md)
{
	for (auto & e : listeners)
		e->MouseMove(md);
}

void ListenerSource::mousepressed(const MouseData & md)
{
	for (auto & e : listeners)
		e->MousePressed(md);
}

void ListenerSource::mouseunpressed(const MouseData & md)
{
	for (auto & e : listeners)
		e->MouseUnPressed(md);
}

void ListenerSource::mousewheel(const MouseData & md)
{
	for (auto & e : listeners)
		e->MouseWheel(md);
}

void ListenerSource::Add(Listener * l)
{
	listeners.push_back(l);
}

void ListenerSource::Run(UINT msg, WPARAM wparam, LPARAM lparam)
{
	KeyCode kc;
	MouseData md;
	switch (msg)
	{
	case WM_KEYDOWN:
		kc.virtualKey = (unsigned)wparam;
		keypressed(kc);
		break;

	case WM_KEYUP:
		kc.virtualKey = (unsigned)wparam;
		keyunpressed(kc);
		break;

	case WM_CHAR:
		kc.charKey = (TCHAR)wparam;
		break;

	case WM_LBUTTONDOWN:
		md.button = GET_KEYSTATE_WPARAM(wparam);
		md.x = GET_X_LPARAM(lparam);
		md.y = GET_Y_LPARAM(lparam);
		mousepressed(md);
		break;

	case WM_LBUTTONUP:
		md.button = GET_KEYSTATE_WPARAM(wparam);
		md.x = GET_X_LPARAM(lparam);
		md.y = GET_Y_LPARAM(lparam);
		mouseunpressed(md);
		break;

	case WM_MBUTTONDOWN:
		md.button = GET_KEYSTATE_WPARAM(wparam);
		md.x = GET_X_LPARAM(lparam);
		md.y = GET_Y_LPARAM(lparam);
		mousepressed(md);
		break;

	case WM_RBUTTONDOWN:
		md.button = GET_KEYSTATE_WPARAM(wparam);
		md.x = GET_X_LPARAM(lparam);
		md.y = GET_Y_LPARAM(lparam);
		mousepressed(md);
		break;

	case WM_RBUTTONUP:
		md.button = GET_KEYSTATE_WPARAM(wparam);
		md.x = GET_X_LPARAM(lparam);
		md.y = GET_Y_LPARAM(lparam);
		mouseunpressed(md);
		break;

	case WM_MOUSEMOVE:
		md.button = GET_KEYSTATE_WPARAM(wparam);
		md.x = GET_X_LPARAM(lparam);
		md.y = GET_Y_LPARAM(lparam);
		mousemove(md);
		break;

		break;
	case WM_MBUTTONUP:

		break;

	case WM_MOUSEWHEEL:
		md.button = GET_KEYSTATE_WPARAM(wparam);
		md.CountWheel = GET_WHEEL_DELTA_WPARAM(wparam);
		md.x = GET_X_LPARAM(lparam);
		md.y = GET_Y_LPARAM(lparam);
		mousewheel(md);
		break;
		
	default: break;
	}
}