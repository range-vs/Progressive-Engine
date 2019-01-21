#pragma once

#include "..\entry_point\framework.h"

// ���������������� ������� �� ��������������� dll ��� ������ ����������� � ����������

// ����������
template<class T>  using _function = char*(T, Graphics*); // ��������� �������

// �������
// ���� "����"
DLLExport char* OpenMap(const char* name_map); // �������� �����
DLLExport char* CloseMap(const char* name_map); // ��������� �����

// ���� "�����������"
DLLExport char* ReShowNet(); // ����� / �������� �����
DLLExport char* ReMode(int mode); // ����� ������ ���������

// ���� �������������(�����)
DLLExport char* SetModeManipulations(int mode); // ������ ����� ���������� ������
DLLExport char* PastModelForMap(char* path, int x, int y); // ��������� ������ �� �����

// ������� ���������� �����
DLLExport char* ResizeRenderWindow(int* size); // �������� ������ ����
DLLExport char* PressKey(int keyCode); // ����������� ������� �������
DLLExport char* UnPressKey(int keyCode); // ����������� ���������� �������



// ����� �������
template<class Function, class T>
char* ReadFunctionForDLL(const char* dll_name, T function_name, Function* func)
{
	HINSTANCE hLib = LoadLibraryA(dll_name); // ��������� ������ dll
	if (hLib == NULL) // ���������, ��� dll ����������� ���������
	{
		// ���� ��� - ����� � ��� - ���� + ��������� �� �����
		// � ���������� ������ � ���������� ���������
	}
	func = (Function*)GetProcAddress((HMODULE)hLib, function_name); // ��������� ������� �� dll
	if (func == NULL)
	{
		// ���� ������� �� ����������� - ����� � ��� - ���� + ��������� �� �����
		// � ���������� ������ � ���������� ���������
	}
	return ""; 
}