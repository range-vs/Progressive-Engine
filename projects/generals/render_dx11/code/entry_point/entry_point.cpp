#include "Framework.h"

// args:
// 0 - HINSTANCE ����������� ����������
// 1 - HWND ��������
// 2 - HWND ���� ����(��� �������� ���� ���������)

// ����� ����� � ��������
DLLExport UINT start_render(HINSTANCE hInst, HWND hwnd_main, HWND hwnd_console_log)
{
	UINT result(0);
	//Logger::InitLoggingSystem("log_editor", hwnd_console_log);
	Framework MainFrame;
	result = MainFrame.RunEditor(hInst, hwnd_main);
	//Logger::DestroyLoggindSystem();
	return result;
}

// ���������� ������ � ������ ������



 
// args:
// 0 - HINSTANCE ����������� ����������
// 1 - HWND ��������
// 2 - ������(���� ����) � ����������� // �����������, TODO

// ����� ����� � ����
//DLLExport UINT start_render(const char* type_program, const HINSTANCE& hInst, const HWND& hwnd_main/*��� ��������� � �������, �����������*/)
//{
//	UINT result(0);
//	Framework MainFrame;
//	result = MainFrame.Run(type_program, hInst, hwnd_main/*�������� ������ � �����������, ���� ����*/);
//	return result;
//}
