#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <dxgi.h>
#include <vector>
#include <fstream>
#include <string>
#include <algorithm>
#include <SetupAPI.h>
#include <devguid.h>
#include <batclass.h>
#include <iostream>
#include "../Logger/Logger.h"

#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "setupAPI.lib")
#pragma optimize("", off) // ��������� ����� ����������� ���� ������

using std::vector;
using std::wofstream;
using std::string;
using std::transform;

#define DLLExport extern "C" __declspec(dllexport)

enum TypeDevice
{
	PC,
	NOTEBOOK
};

class PhysicAdapter
{
	IDXGIFactory1* factory; // ������� - �������� ����� dxgi ��� ������� � �������� �������������
	IDXGIAdapter1* adapter; // ������� ������������
	string adapter_name; // ��� �������� �������������
	vector<IDXGIAdapter1*> all_adapters; // ������ ���������
	vector<string> all_adapters_names; // ����� ���������
	vector<UINT> vendor_adapters; // ��� ��������(���������������/ ����������)
	UINT vendor; // ��� �������� ��������
	TypeDevice type; // ��� ����������
	IDXGIOutput* display; // ������� ��� ������
	static PhysicAdapter adapt;
	static string file_name_postfix;

	PhysicAdapter():factory(nullptr), adapter(nullptr), display(nullptr) {}
	PhysicAdapter(const PhysicAdapter&);
	PhysicAdapter& operator= (const PhysicAdapter&);
public:
	bool Init(); // ��������� ������������� ������� 
	bool FindAdapter(const string& s); // ����� � ��������� ������� ��������
	static PhysicAdapter& GetInstance(); // ��������� ������ �� ��������� - ������
	const vector<string>& GetListAdapters() { return this->all_adapters_names; } // �������� �������� ���������
	void SetCurrentAdapter(int index); // ������������� ������� �������
	void WriteFileDataCurrentAdapter(); // ���������� ������ �������� �������� � ����
	string GetTypeDevice(int index_battery); // ��-��������� ����
	string GetNameCurrentAdapter() { return this->adapter_name; } // �������� ��� �������� ��������
	IDXGIAdapter1* GetCurrentAdapter() { return this->adapter; } // �������� ������� �������
	bool IsOutputs(); // ��������, ���� �� ������� � ���������� ����������(� ���������� � ���� �������� � ���-�� �������)
	void SetTypeDevice(); // ���������� ��� ���������� � ������ ���
	const char* TypeAdapter(int* type = nullptr); // ��� ����������(���������� ��� ���������������)
	void Release(); // ����������� �������
	void setFileNamePostfix(const char* n); //  ������������� ����������� ����� ����� �����
	bool getFrequencyMonitor(int* n, int* d); // �������� ������� ��������
	static string getFileNamePostfix() { return PhysicAdapter::file_name_postfix; } // �������� ������� ��� ����� ��� ������������
};

DLLExport  char**  GetListAdapters(int*); // �������� ������ ���������
DLLExport  char*  SetAdapter(int); // ���������� ��������� �������
DLLExport  char*  InfoTypeDevice(); // �������� ����������(�� ��� �������)
DLLExport  char*  GetTypeAdapter(int* t); // �������� ���������� � ���� ��������
DLLExport  void  ReleaseData(); // ����������� �������
DLLExport IDXGIAdapter1*  GetSelectedAdapter(const string&); // ���������� ������� �� ��� ����� � ���������� ���������
DLLExport void WriteFileDataCurrentAdapter(); // ����� ������ � ��������� �������� � ����
DLLExport bool GetSelectedAdapterOutput(const string&); // ��������, ���� �� �������� � ��������
DLLExport void SetPostfixFileName(char* n); //  ������������� ����������� ����� ����� �����
DLLExport bool GetFrequencyCurrentMonitor(int* n, int* d); // �������� ������� ��������

