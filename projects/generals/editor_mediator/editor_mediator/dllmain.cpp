#include <windows.h>

BOOL WINAPI DllMain(
	HINSTANCE hinstDLL,  // ���������� ������ DLL
	DWORD fdwReason,     // ������� ��� ������ �������
	LPVOID lpReserved)  // ���������������
{
	switch (fdwReason)
	{
	case DLL_PROCESS_ATTACH:
		// ����������� ������������� ������� ������ ��������.
		// ���������� FALSE, ����� ��������� ������� ��������.
		break;

	case DLL_THREAD_ATTACH:
		// ������ ������������� ��� ������ �������������.
		break;

	case DLL_THREAD_DETACH:
		// ������ ������������� ��� ������ �������.
		break;

	case DLL_PROCESS_DETACH:
		// ��������� ����� ����������� �������.
		break;
	}
	return TRUE;  // �������� ���������� DLL_PROCESS_ATTACH.
}