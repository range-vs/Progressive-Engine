#include <windows.h>

BOOL WINAPI DllMain(
	HINSTANCE hinstDLL,  // дескриптор модуля DLL
	DWORD fdwReason,     // причина для вызова функции
	LPVOID lpReserved)  // зарезервировано
{
	switch (fdwReason)
	{
	case DLL_PROCESS_ATTACH:
		// Однократная инициализация каждого нового процесса.
		// Возвращает FALSE, чтобы завершить ошибкой загрузку.
		break;

	case DLL_THREAD_ATTACH:
		// Делаем специфическую для потока инициализацию.
		break;

	case DLL_THREAD_DETACH:
		// Делаем специфическую для потока очистку.
		break;

	case DLL_PROCESS_DETACH:
		// Выполняем любую необходимую очистку.
		break;
	}
	return TRUE;  // Успешное завершение DLL_PROCESS_ATTACH.
}