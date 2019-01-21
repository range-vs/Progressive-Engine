#include "..\general_code\func.h"

//int main(int argc, char* argv[])
using namespace System;
using namespace System::Diagnostics;
using namespace System::Runtime::InteropServices;

[STAThreadAttribute]
int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hIntOld, LPSTR cmdLine, int typeShowWindow)
{
	int size(0);
	LPWSTR lines = GetCommandLine();
	LPWSTR* args = CommandLineToArgvW(lines, &size);
	System::String ^ line = gcnew System::String("NVIDIA GeForse GTX 950M");   // args[1]

	HINSTANCE hLib(NULL);
	/*if (isMonitor(args[1], &hLib))
	{
		if (!wcscmp(L"-editor", args[2])) // запускаем редактор
		{*/
			WriteFile(&hLib);
			Program::Main(line);
		/*}
		else if (!wcscmp(L"-game", args[2])) // запускаем игру
		{
			// TODO
		}
	}
	else
	{
		FreeLibrary((HMODULE)hLib);
		line = line->Insert(0, "К адаптеру \"");
		line += "\" не подключен монитор. \nПодключите, или выберите другой адаптер.";
		MessageBox(NULL, (wchar_t*)(void*)Marshal::StringToHGlobalUni(line), L"Информация", MB_OK);
		System:Process::Start("launcher_engine");
	}*/

	return 0;
}

bool isMonitor(const wchar_t* devName, HINSTANCE* hist)
{
	HINSTANCE hLib = LoadLibrary(L"adapters.dll");
	if (hLib == NULL)
		return false;
	*hist = hLib;
	FuncAdr* func = (FuncAdr*)GetProcAddress((HMODULE)hLib, "GetSelectedAdapterOutput");
	if (func == NULL)
		return false;
	std::wstring name(devName);
	std::string inLine(name.begin(), name.end());
	bool result = func(inLine);
	return result;
}

bool WriteFile(HINSTANCE* hLib)
{
	FuncWriteFile* func = (FuncWriteFile*)GetProcAddress((HMODULE)*hLib, "WriteFileDataCurrentAdapter");
	if (func == NULL)
		return false;
	func();
	FreeLibrary((HMODULE)*hLib);
	return true;
}
