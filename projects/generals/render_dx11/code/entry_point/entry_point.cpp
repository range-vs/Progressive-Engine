#include "Framework.h"

// args:
// 0 - HINSTANCE ����������� ����������
// 1 - HWND ��������
// 2 - HWND ���� ����(��� �������� ���� ���������)

// ����� ����� � ��������
DLLExport UINT startEditor(HINSTANCE hInst, HWND hwnd_main, HWND hwnd_console_log)
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

 //����� ����� � ����
DLLExport UINT startGame(HINSTANCE hInst, HWND hwnd_main/*��� ��������� � �������, �����������*/)
{
	UINT result(0);
	Framework MainFrame;
	result = MainFrame.RunGame(hInst, hwnd_main/*�������� ������ � �����������, ���� ����*/);
	return result;
}
