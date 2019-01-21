#include "adapters.h"

PhysicAdapter PhysicAdapter::adapt;

string PhysicAdapter::file_name_postfix;

void PhysicAdapter::setFileNamePostfix(const char* n)
{
	PhysicAdapter::file_name_postfix = "adapters_";
	PhysicAdapter::file_name_postfix += n;
}

PhysicAdapter& PhysicAdapter::GetInstance()
{
	return adapt;
}

bool PhysicAdapter::Init()
{
	HRESULT result(S_FALSE);
	result = CreateDXGIFactory1(__uuidof(IDXGIFactory1), (void**)&factory);
	if (FAILED(result))
		return false;

	for (int i(0);; i++)
	{
		IDXGIAdapter1* adpt(nullptr);
		result = factory->EnumAdapters1(i, &adpt);
		if (FAILED(result))
			break;
		DXGI_ADAPTER_DESC1 adesc;
		ZeroMemory(&adesc, sizeof(adesc));
		adpt->GetDesc1(&adesc);
		if ((adesc.VendorId == 0x1414) && (adesc.DeviceId == 0x8c)) // исключаем WARP - устройство(имитация видеокарты для разработчиков)
		{
			adpt->Release();
			continue;
		}
		std::string buf; buf.resize(128);
		WideCharToMultiByte(1251, NULL, adesc.Description, -1, (char*)buf.c_str(), 128, NULL, NULL);
		buf.resize(strlen(buf.c_str()));
		this->all_adapters_names.push_back(buf);
		this->vendor_adapters.push_back(adesc.VendorId);
		all_adapters.push_back(adpt); // не забыть потом выгрузить эти ресурсы
	}
	if (all_adapters.empty())
		return false;
	this->SetTypeDevice(); // определяем тип устройства
}

void PhysicAdapter::SetCurrentAdapter(int index)
{
	this->adapter = this->all_adapters[index]; // устанавливаем текущий видеоадаптер
	this->adapter_name = this->all_adapters_names[index]; // устанавливаем и его имя
	this->vendor = this->vendor_adapters[index]; // устанавливаем тип адаптера
}

void PhysicAdapter::WriteFileDataCurrentAdapter()
{
	std::ofstream out("tmp/adapter.info"/*, std::ios::app*/);
	//out.imbue(std::locale("russian"));
	std::locale rl;
	rl = rl.combine< std::numpunct<wchar_t> >(std::locale::classic());
	out.imbue(rl);

	char* type_pc = InfoTypeDevice();
	out << type_pc << std::endl;
	delete[] type_pc;
	out << this->adapter_name << std::endl;
	const char* type = TypeAdapter();
	out << type << std::endl;
	delete[] type;
	DXGI_ADAPTER_DESC1 desc;
	this->adapter->GetDesc1(&desc);
	out << (int)(desc.DedicatedVideoMemory / 1024 / 1024) << " MB" << std::endl;

	// пишем название дисплея в файл
	if (display)
	{
		DXGI_OUTPUT_DESC descDispl;
		display->GetDesc(&descDispl);
		MONITORINFOEX moninfo; ZeroMemory(&moninfo, sizeof MONITORINFOEX);
		moninfo.cbSize = sizeof(moninfo);
		GetMonitorInfo(descDispl.Monitor, &moninfo);
		DISPLAY_DEVICE ddev; ZeroMemory(&ddev, sizeof DISPLAY_DEVICE);
		ddev.cb = sizeof(ddev);
		EnumDisplayDevices(moninfo.szDevice, 0, &ddev, 0);
		out << ddev.DeviceString << std::endl;
	}
	out.close();
}

string PhysicAdapter::GetTypeDevice(const int batteryIndex)
{
	HDEVINFO hDeviceInfoList = SetupDiGetClassDevs(&GUID_DEVCLASS_BATTERY, 0, 0, DIGCF_PRESENT | DIGCF_DEVICEINTERFACE);
	if (hDeviceInfoList != INVALID_HANDLE_VALUE)
	{
		SP_DEVICE_INTERFACE_DATA did = { 0 };
		did.cbSize = sizeof(did);
		if (::SetupDiEnumDeviceInterfaces(hDeviceInfoList, 0, &GUID_DEVCLASS_BATTERY, batteryIndex, &did)) 
		{
			/*DWORD dwRequiredSize = 0;
			::SetupDiGetDeviceInterfaceDetail(hDeviceInfoList, &did, 0, 0, &dwRequiredSize, 0);
			if (GetLastError() == ERROR_INSUFFICIENT_BUFFER)
			{
				PSP_DEVICE_INTERFACE_DETAIL_DATA pdidd = (PSP_DEVICE_INTERFACE_DETAIL_DATA)::LocalAlloc(LPTR, dwRequiredSize);
				if (pdidd)
				{
					pdidd->cbSize = sizeof(*pdidd);
					if (::SetupDiGetDeviceInterfaceDetail(hDeviceInfoList, &did, pdidd, dwRequiredSize, &dwRequiredSize, 0))
					{
						result = pdidd->DevicePath;
					}

					::LocalFree(pdidd);
				}
			}*/

			// если батарея обнаружена на ПК
			SetupDiDestroyDeviceInfoList(hDeviceInfoList);
			return "Notebook";
		}
	}
	return "PC";
}

void PhysicAdapter::SetTypeDevice()
{
	string name = this->GetTypeDevice(0);
	if (name == "PC")
		this->type = PC;
	else if (name == "Notebook")
		this->type = NOTEBOOK;
}

void PhysicAdapter::Release()
{
	if (factory)
		factory->Release();
	adapter = nullptr;
	for (int i(0); i < all_adapters.size(); i++)
	{
		if (all_adapters[i])
		{
			all_adapters[i]->Release();
			all_adapters[i] = nullptr;
		}
	}
	if (display)
		display->Release();
	display = nullptr;
	all_adapters_names.clear();
	vendor_adapters.clear();
}

const char* PhysicAdapter::TypeAdapter(int* type)
{
	string message; // = this->adapter_name;
		if (this->vendor == 0x8086)
			message = "Интегрированное видеооборудование";
		else
			message = "Дискретное видеооборудование";
		if (type)
			if (this->vendor == 0x8086)
				*type = 0;
			else
				*type = 1;
		char* _type = (char*)message.c_str();
		char* res = new char[strlen(_type) + 1];
		strcpy_s(res, strlen(_type) + 1, _type);
		return res;
}

bool PhysicAdapter::IsOutputs()
{
	IDXGIOutput* output(nullptr);
	if (this->adapter->EnumOutputs(0, &output) != DXGI_ERROR_NOT_FOUND)
	{
		this->display = output;
		return true;
	}
	return false;
}

bool PhysicAdapter::FindAdapter(const string& s)
{
	int i(0);
	for (auto& n : this->all_adapters_names)
	{
		if (n == s)
		{
			this->adapter = this->all_adapters[i];
			this->adapter_name = this->all_adapters_names[i];
			return true;
		}
		i++;
	}
	return false;
}

bool PhysicAdapter::getFrequencyMonitor(int* n, int* d) 
{
	// Метод получения частоты монитора(установленного на конкретном пк)
	IDXGIOutput* output(nullptr);
	// получаем монитор, привязанный к этой видеокарте
	HRESULT result = adapter->EnumOutputs(0, &output);
	if (FAILED(result))
	{
		return false;
	}
	UINT countModes(0);
	// получаем количество режимов отображения, соответстующих конкретному монитору
	result = output->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &countModes, NULL);
	if (FAILED(result))
	{
		return false;
	}

	// создаем массив для хранения режимов отображения
	DXGI_MODE_DESC* displayModeList = new DXGI_MODE_DESC[countModes];
	if (!displayModeList)
	{
		return false;
	}

	// помещаем все режимы отображения в созданный массив
	result = output->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &countModes, displayModeList);
	if (FAILED(result))
	{
		return false;
	}

	DXGI_OUTPUT_DESC desc; output->GetDesc(&desc); 
	int screenWidth(desc.DesktopCoordinates.right);
	int screenHeight(desc.DesktopCoordinates.bottom); 
	// Перебираем все режимы отображения и ищем ту комбинацию, соответстующую разрешению нашего экрана
	for (int i(0); i<countModes; i++)
	{
		if (displayModeList[i].Width == (unsigned int)screenWidth)
		{
			if (displayModeList[i].Height == (unsigned int)screenHeight)
			{
				*n = displayModeList[i].RefreshRate.Numerator;
				*d = displayModeList[i].RefreshRate.Denominator;
				break;
			}
		}
	}
}

//------------------------------------ FUNCTION DLL EXPORT ------------------------------------------//

DLLExport char**  GetListAdapters(int* size)
{
	char** res(nullptr);
	{
		PhysicAdapter& pa = PhysicAdapter::GetInstance();
		pa.Init();
		*size = pa.GetListAdapters().size();
		res = new char*[*size];
		for (int i(0); i < *size; i++)
			res[i] = (char*)pa.GetListAdapters()[i].c_str();
	}
	return res;
}

DLLExport  char*  SetAdapter(int index)
{
	char* name(nullptr);
	{
		PhysicAdapter& pa = PhysicAdapter::GetInstance();
		pa.SetCurrentAdapter(index);
		int size = strlen(pa.GetNameCurrentAdapter().c_str()) + 1;
		name = new char[size];
		strcpy_s(name, size, pa.GetNameCurrentAdapter().c_str());
	}
	return name;
}

DLLExport char*  InfoTypeDevice()
{
	char* message(nullptr);
	{
		PhysicAdapter& pa = PhysicAdapter::GetInstance();
		string name = pa.GetTypeDevice(0);
		message = new char[50];
		if (name == "PC")
			strcpy_s(message, 50, "Персональный компьютер");
		else if (name == "Notebook")
			strcpy_s(message, 50, "Беспроводное устройство(ноутбук)");
	}
	return message;
}

DLLExport void  ReleaseData()
{
	{
		PhysicAdapter& pa = PhysicAdapter::GetInstance();
		pa.Release();
	}
}

DLLExport IDXGIAdapter1*  GetSelectedAdapter(const string& n)
{
	IDXGIAdapter1* adptr(nullptr);
	{
		PhysicAdapter& pa = PhysicAdapter::GetInstance();
		pa.Init();
		pa.FindAdapter(n);
		adptr = pa.GetCurrentAdapter();
	}
	return adptr;
}

DLLExport bool GetSelectedAdapterOutput(const string& n)
{
	bool result(false);
	{
		PhysicAdapter& pa = PhysicAdapter::GetInstance();
		pa.Init();
		pa.FindAdapter(n);
		result = pa.IsOutputs();
	}
	return result;
}

DLLExport  char*  GetTypeAdapter(int* t)
{
	char* _type(nullptr);
	{
		PhysicAdapter& pa = PhysicAdapter::GetInstance();
		_type = (char*)pa.TypeAdapter(t);
	}
	return _type;
}

DLLExport void WriteFileDataCurrentAdapter()
{
	{
		PhysicAdapter& pa = PhysicAdapter::GetInstance();
		pa.WriteFileDataCurrentAdapter();
	}
}

DLLExport void SetPostfixFileName(char* n)
{
	PhysicAdapter& pa = PhysicAdapter::GetInstance();
	pa.setFileNamePostfix(n);
}

DLLExport bool GetFrequencyCurrentMonitor(int* n, int* d)
{
	bool result(false);
	{
		PhysicAdapter& pa = PhysicAdapter::GetInstance();
		result = pa.getFrequencyMonitor(n, d);
	}
	return result;
}

//--DEBUG--//

//int main()
//{
//	PhysicAdapter& pa = PhysicAdapter::GetInstatnce();
//	pa.Init();
//	pa.SetCurrentAdapter(0);
//	int *t = nullptr;
//	char* rr = GetTypeAdapter(t);
//
//	return 0;
//}
//
//
//void test()
//{
//	std::wofstream f("file.txt");
//	HRESULT result(S_FALSE);
//	IDXGIFactory1* factory(nullptr);
//	result = CreateDXGIFactory1(__uuidof(IDXGIFactory1), (void**)&factory);
//	if (FAILED(result))
//		return;
//
//	vector<IDXGIAdapter1*> all_adapters;
//	vector<WCHAR*> all_desc_adapters;
//	for (int i(0);; i++)
//	{
//		IDXGIAdapter1* adpt(nullptr);
//		result = factory->EnumAdapters1(i, &adpt);
//		if (FAILED(result))
//			break;
//		DXGI_ADAPTER_DESC1 adesc;
//		ZeroMemory(&adesc, sizeof(adesc));
//		adpt->GetDesc1(&adesc);
//		if ((adesc.VendorId == 0x1414) && (adesc.DeviceId == 0x8c)) // исключаем WARP - устройство(имитация видеокарты для разработчиков)
//		{
//			adpt->Release();
//			continue;
//		}
//		WCHAR *buf = new WCHAR[128]; wcscpy_s(buf, 128, adesc.Description);
//		all_desc_adapters.push_back(buf);
//		f << adesc.Description << std::endl;
//		all_adapters.push_back(adpt); // не забыть потом выгрузить эти ресурсы
//	}
//
//	int i = 1;
//	if (i == 1)
//	{
//		HINSTANCE hLib = LoadLibrary("nvidia_optimus.dll");
//		if (hLib == NULL)
//		{
//			f << L"Error load dll" << std::endl;
//			return;
//		}
//		func Func = (func)GetProcAddress((HMODULE)hLib, "Load");
//		if (Func == NULL)
//		{
//			f << L"Error loading function \'Load\' for dll\n";
//			return;
//		}
//		f << L"Status adapter: " << Func() << std::endl;
//		FreeLibrary((HMODULE)hLib);
//	}
//
//	// enable nvidia(test)
//
//	factory->Release();
//	for (auto& a : all_adapters)
//		a->Release();
//	all_adapters.resize(0);
//	for (auto& a : all_desc_adapters)
//		delete a;
//	all_desc_adapters.resize(0);
//
//
//	result = CreateDXGIFactory1(__uuidof(IDXGIFactory1), (void**)&factory);
//	if (FAILED(result))
//		return;
//
//	for (int i(0);; i++)
//	{
//		IDXGIAdapter1* adpt(nullptr);
//		result = factory->EnumAdapters1(i, &adpt);
//		if (FAILED(result))
//			break;
//		DXGI_ADAPTER_DESC1 adesc;
//		ZeroMemory(&adesc, sizeof(adesc));
//		adpt->GetDesc1(&adesc);
//		if ((adesc.VendorId == 0x1414) && (adesc.DeviceId == 0x8c)) // исключаем WARP - устройство(имитация видеокарты для разработчиков)
//		{
//			adpt->Release();
//			continue;
//		}
//		WCHAR *buf = new WCHAR[128]; wcscpy_s(buf, 128, adesc.Description);
//		all_desc_adapters.push_back(buf);
//		f << adesc.Description << std::endl;
//		all_adapters.push_back(adpt); // не забыть потом выгрузить эти ресурсы
//	}
//	IDXGIOutput* display(nullptr);
//	f <<L"Current adapter: " << all_desc_adapters[i] << std::endl;
//	result = all_adapters[i]->EnumOutputs(0, &display); // получаем ОДИН монитор(может в будущем сделать поддержку выбора монитора)
//	if (FAILED(result))
//	{
//		f << "Output number 0 not found" << std::endl;
//		return;
//	}
//}
