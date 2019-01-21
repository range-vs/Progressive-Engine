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
#pragma optimize("", off) // отключаем новый оптимизатор кода студии

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
	IDXGIFactory1* factory; // фабрика - основной метод dxgi для доступа к ресурсам видеоадаптера
	IDXGIAdapter1* adapter; // текущий видеоадаптер
	string adapter_name; // имя текущего видеоадаптера
	vector<IDXGIAdapter1*> all_adapters; // список адаптеров
	vector<string> all_adapters_names; // имена адаптеров
	vector<UINT> vendor_adapters; // тип адаптера(интегрированный/ дискретный)
	UINT vendor; // тип текущего адаптера
	TypeDevice type; // тип устройства
	IDXGIOutput* display; // дисплей для вывода
	static PhysicAdapter adapt;
	static string file_name_postfix;

	PhysicAdapter():factory(nullptr), adapter(nullptr), display(nullptr) {}
	PhysicAdapter(const PhysicAdapter&);
	PhysicAdapter& operator= (const PhysicAdapter&);
public:
	bool Init(); // стартовая инициализация системы 
	bool FindAdapter(const string& s); // поиск и установка нужного адаптера
	static PhysicAdapter& GetInstance(); // получение ссылки на синглетон - объект
	const vector<string>& GetListAdapters() { return this->all_adapters_names; } // получаем название адаптеров
	void SetCurrentAdapter(int index); // устанавливаем текущий адаптер
	void WriteFileDataCurrentAdapter(); // записываем данные текущего адаптера в файл
	string GetTypeDevice(int index_battery); // по-умолчанию ноль
	string GetNameCurrentAdapter() { return this->adapter_name; } // получаем имя текущего адаптера
	IDXGIAdapter1* GetCurrentAdapter() { return this->adapter; } // получаем текущий адаптер
	bool IsOutputs(); // проверка, есть ли дисплеи у выбранного устройство(и дописываем в файл название и хар-ки дисплея)
	void SetTypeDevice(); // определяем тип устройства и храним его
	const char* TypeAdapter(int* type = nullptr); // тип устройства(дискретное или интегрированное)
	void Release(); // освобождаем ресурсы
	void setFileNamePostfix(const char* n); //  устанавливаем постфиксную часть имени файла
	bool getFrequencyMonitor(int* n, int* d); // получаем частоту монитора
	static string getFileNamePostfix() { return PhysicAdapter::file_name_postfix; } // получаем текущее имя файла для логгирования
};

DLLExport  char**  GetListAdapters(int*); // получить список адаптеров
DLLExport  char*  SetAdapter(int); // установить выбранный адаптер
DLLExport  char*  InfoTypeDevice(); // получить информацию(пк или ноутбук)
DLLExport  char*  GetTypeAdapter(int* t); // получить информацию о типе адаптера
DLLExport  void  ReleaseData(); // освобождаем ресурсы
DLLExport IDXGIAdapter1*  GetSelectedAdapter(const string&); // возвращаем адаптер по его имени в вызывающую программу
DLLExport void WriteFileDataCurrentAdapter(); // пишем данные о выбранном адаптере в файл
DLLExport bool GetSelectedAdapterOutput(const string&); // получаем, есть ли мониторы у адаптера
DLLExport void SetPostfixFileName(char* n); //  устанавливаем постфиксную часть имени файла
DLLExport bool GetFrequencyCurrentMonitor(int* n, int* d); // получить частоту монитора

