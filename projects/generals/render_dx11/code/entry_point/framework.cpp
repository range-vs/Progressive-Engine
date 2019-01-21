#include "Framework.h"

// камера
void MKListener::MousePressed(const MouseData & kc)
{
	Device->getCamera().setMauseLastPosition({ kc.x, kc.y }); // устанавливает последнюю позицию ДО нажатия ЛКМ
	Device->setMauseCapture(true); // устанавливаем флаг, что сцену надо обновить
	Device->runFrustum();
}

void MKListener::MouseUnPressed(const MouseData & md)
{
	Device->setMauseCapture(false); // снимаем флаг, более сцену обновлять не надо
	//Device->setDebugData();
	Device->runFrustum();
}

void MKListener::MouseMove(const MouseData & md)
{
	if (Device->getMauseCapture())
	{
		Device->getCamera().rePosition(md); // меняем положение камеры
		Device->runFrustum();
	}
}

void MKListener::MouseWheel(const MouseData & md)
{
	Device->getCamera().rePositionWheel(md); // меняем положение камеры
	//Device->setDebugData();
	Device->runFrustum();
}




// выбор объектов мышью
void SelectObjectListener::MousePressed(const MouseData & kc)
{
	switch (Device->isModeManipulator())
	{
	case 0: // при нуле просто можем крутить сцену мышью
		break;

	case 1: // можно выделять модель
	{
		POINT p = { kc.x, kc.y };
		SendMessage(Device->getDesctiptor(), WM_RAYPICK, (WPARAM)&p, NULL);
		break;
	}

	default:
		break;
	}
}

void SelectObjectListener::KeyPressed(const KeyCode & kc)
{
	switch (kc.virtualKey)
	{

	case vkControl:
	{
		Device->setPressControl(true);
		break;
	}

	case vkLeft:
	{
		Device->editModel(-1);
		break;
	}

	case vkRight:
	{
		Device->editModel(1);
		break;
	}


	default: break;
	}

	/*switch (kc.charKey)
	{
	default: break;
	}*/
}

void SelectObjectListener::KeyUnPressed(const KeyCode & kc)
{
	switch (kc.virtualKey)
	{
	case vkControl:
	{
		Device->setPressControl(false);
		//MessageBox(NULL, L"test", L"test", MB_OK);
		break;
	}
	case vkHome:
	{
		// debug
		CharactersModel m;
		auto v = Device->getFrustumCulling().getVertexFrustum();
		auto ind = Device->getFrustumCulling().getIndexFrustum();
		Device->addModelColor(TypePrimitive::Frustum, nullptr, &v, &ind, &m);
		Device->runFrustum();
		break;
	}
	}

	/*switch (kc.charKey)
	{
	default: break;
	}*/
}

// фрэймворк
UINT Framework::RunEditor(const HINSTANCE& hInst, const HWND& hwnd_main)
{
	brain_memory<Window> Main(new Window);
	if (!Main)
	{
		LOGGINGERR("Error create window: no memory");
		return -1;
	}
	else
		LOGGING("Create window: OK");

	if (!cameraListener)
	{
		LOGGINGERR("Error create listener #1: no memory");
		return -1;
	}
	else
		LOGGING("Create listener #1: OK");


	if (!kpselect)
	{
		LOGGINGERR("Error create listener #2: no memory");
		return -1;
	}
	else
		LOGGING("Create listener #2: OK");


	RECT rect = { 0 };
	GetWindowRect(hwnd_main, &rect);
	bool result = Main->Init(hInst, L"NoName Engine Editor", L"engine_window", CW_USEDEFAULT, CW_USEDEFAULT,
		rect.right - rect.left, rect.bottom - rect.top, hwnd_main); // запускаем инициализацию окна
	if (!result)
		return -1;

	Main->AddListener(kpselect.get()); // добавляем слушателя - выбор объекта/треугольника
	Main->AddListener(cameraListener.get()); // добавляем слушателя - управление камерой
	result = Device->initEditor(Main->get_hwnd()); // здесь запускаем инициализацию устройства dx
	if (!result)
		return -1;

	/*SetParent(Main->GetHWND(), main);
	child = Main->GetHWND();
	SetFocus(main);*/

	WPARAM message(Main->RunEditor(SW_SHOW));
	Main->Release();
	return (UINT)message;
}
