#include "Framework.h"

// args:
// 0 - HINSTANCE вызывающего приложения
// 1 - HWND родителя
// 2 - HWND окна лога(для отправки туда сообщений)

// точка входа в редактор
DLLExport UINT start_render(HINSTANCE hInst, HWND hwnd_main, HWND hwnd_console_log)
{
	UINT result(0);
	//Logger::InitLoggingSystem("log_editor", hwnd_console_log);
	Framework MainFrame;
	result = MainFrame.RunEditor(hInst, hwnd_main);
	//Logger::DestroyLoggindSystem();
	return result;
}

// расставить логгер в нужных местах



 
// args:
// 0 - HINSTANCE вызывающего приложения
// 1 - HWND родителя
// 2 - данные(если игра) с настройками // разработать, TODO

// точка входа в игру
//DLLExport UINT start_render(const char* type_program, const HINSTANCE& hInst, const HWND& hwnd_main/*тут структура с данными, разработать*/)
//{
//	UINT result(0);
//	Framework MainFrame;
//	result = MainFrame.Run(type_program, hInst, hwnd_main/*передаем данные с настройками, если есть*/);
//	return result;
//}
