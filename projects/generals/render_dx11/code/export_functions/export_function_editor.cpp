#pragma once

#include "export_function_editor.h"
#include "..\entry_point\framework.h"

// ���������������� ������� �� ��������������� dll ��� ������ ����������� � ����������

// ���� "����"
DLLExport char* OpenMap(const char* name_map)
{
	_function<const char*>* func(nullptr); // ������ ����� �������
	char* error = ReadFunctionForDLL("editor_mediator.dll", "OpenMap", func); // �������� ����� �������
	if (!func) // ���� ������� �� �����������
		return error; // ���������� ��������� �� ������ � ��������
	return func(name_map, Device); // ����� �������� ����������� ������� � ���������� � �������� ������������
}

DLLExport char* CloseMap(const char * name_map)
{
	_function<const char*>* func(nullptr); // ������ ����� �������
	char* error = ReadFunctionForDLL("editor_mediator.dll", "CloseMap", func); // �������� ����� �������
	if (!func) // ���� ������� �� �����������
		return error; // ���������� ��������� �� ������ � ��������
	return func(name_map, Device); // ����� �������� ����������� ������� � ���������� � �������� ������������
}

// ���� "�����������"
DLLExport char * ReShowNet()
{
	Device->reShowNet();
	return "OK";
}

DLLExport char * ReMode(int mode)
{
	D3D11_FILLMODE m;
	switch (mode)
	{
	case 0:
		m = D3D11_FILLMODE::D3D11_FILLMODE_WIREFRAME;
		break;

	case 1:
		m = D3D11_FILLMODE::D3D11_FILLMODE_SOLID;
		break;

	default:
		return "Error: no find mode!";
	}
	Device->reMode(m);
	return "OK";
}

// ���� �������������(�����)
DLLExport char* SetModeManipulations(int mode)
{
	Device->setModeManipulator(mode);
	return "OK";
}

DLLExport char * PastModelForMap(char * path, int x, int y)
{
	DataNewModel dnm;
	dnm.path = path;
	dnm.x = x;
	dnm.y = y;
	SendMessage(Device->getDesctiptor(), WM_ADDMODELMOUSE, (WPARAM)&dnm, NULL);
	return "OK";
}

// ������� ���������� �����
DLLExport char* ResizeRenderWindow(int* size)
{
	SetWindowPos(Device->getDesctiptor(), HWND_BOTTOM, 0, 0, size[0], size[1], SWP_NOMOVE);
	return "OK";
}

DLLExport char * PressKey(int keyCode)
{
	SendMessage(Device->getDesctiptor(), WM_KEYDOWN, (WPARAM)keyCode, NULL);
	return "OK";
}

DLLExport char * UnPressKey(int keyCode)
{
	SendMessage(Device->getDesctiptor(), WM_KEYUP, (WPARAM)keyCode, NULL);
	return "OK";
}