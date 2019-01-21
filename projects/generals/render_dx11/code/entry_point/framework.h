#pragma once

#include "../render/window.h"

class Framework
{
	UINT message_return_os;
	brain_memory<SelectObjectListener> kpselect;
	brain_memory<MKListener> cameraListener;
public:
	Framework() :message_return_os(0), kpselect(new SelectObjectListener), cameraListener(new MKListener()){}
	UINT RunEditor(const HINSTANCE& hInst, const HWND& hwnd_main); // для редактора
	// для игры
};