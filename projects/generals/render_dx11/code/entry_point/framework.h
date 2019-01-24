#pragma once

#include "../render/window.h"

class Framework
{
	UINT message_return_os;
	brain_memory<SelectObjectListener> kpselect;
	brain_memory<MKListener> cameraListener;
	brain_memory<GameInputListener> gameKey;
public:
	Framework() :message_return_os(0){}
	UINT RunEditor(const HINSTANCE& hInst, const HWND& hwnd_main); // для редактора
	UINT RunGame(const HINSTANCE& hInst, const HWND& hwnd_main); // для игры
};