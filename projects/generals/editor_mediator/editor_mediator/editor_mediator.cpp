// функции, экспортируемые в редактор, для доступа к функционалу render.dll

#include <direct_device.h>
#include <..\utilites\macros_global.h>

DLLExport char* SetModeManipulations(int mode, Graphics* mainDevice)
{
	mainDevice->setModeSelect(mode);
}
