#pragma once

#include "..\render\direct_device.h"
#include "macros_general.h"

// макрокоманды
#define DLLExport extern "C" __declspec(dllexport)
#define Device (Graphics::getInstance(D3D11_FILLMODE_SOLID))

// TODO, макросы, на синглетон и т.д.