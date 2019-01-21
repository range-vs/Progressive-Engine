#include "direct_device.h"

Graphics* Graphics::MainDevice(nullptr);

void* Graphics::operator new(size_t i)
{
	return _aligned_malloc(i, 16);
}

void Graphics::operator delete(void* p)
{
	_aligned_free(p);
}

void Graphics::constructor(D3D11_FILLMODE cm, bool vs)
{
	swapChain = nullptr;
	device = nullptr;
	deviceContext = nullptr;
	backBuffer = nullptr;
	DepthStancil = nullptr;
	DepthStancilBuffer = nullptr;
	currentMode = cm;
	rasterizer = nullptr;
	adapter = nullptr;
	currentLevel = D3D_FEATURE_LEVEL();
	vsync = vs;
	if (!vsync)
	{
		numerator = 0;
		denominator = 1;
	}
	isControl = false;
	isMouseCapture = false;
	visibleNet = true;
	net = nullptr;
}

bool Graphics::initEditor(HWND hwnd)
{
	srand((unsigned)time(nullptr)); // рандомизация чисел

	this->hwnd = hwnd;

	RECT rect;
	GetWindowRect(hwnd, &rect);
	int h = rect.bottom - rect.top;
	int w = rect.right - rect.left;

	D3D_FEATURE_LEVEL levels[] = {
		D3D_FEATURE_LEVEL_12_0, 
		D3D_FEATURE_LEVEL_11_1, 
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0,
		D3D_FEATURE_LEVEL_9_3,
		D3D_FEATURE_LEVEL_9_2,
		D3D_FEATURE_LEVEL_9_1,
	};
	// применён dx10(изменена шейдерная модель на 4_0) - сделать выбор dx
	// завести массив версий dx и массив версий шейдерной модели

	DXGI_SWAP_CHAIN_DESC sdesc;
	ZeroMemory(&sdesc, sizeof(sdesc));
	sdesc.BufferCount = 1;
	sdesc.BufferDesc.RefreshRate.Numerator = this->numerator;
	sdesc.BufferDesc.RefreshRate.Denominator = this->denominator;
	sdesc.OutputWindow = hwnd;
	sdesc.SampleDesc.Count = 4;
	sdesc.SampleDesc.Quality = 0;
	sdesc.Windowed = true;
	sdesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH; // разрешить переключение по alt + enter
	sdesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sdesc.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	sdesc.BufferDesc.Height = h;
	sdesc.BufferDesc.Width = w;

	width = (float)w;
	height = (float)h;

	UINT flags(NULL);
#ifdef DEBUG
	flags = D3D11_CREATE_DEVICE_DEBUG;
#endif
	flags |= D3D11_CREATE_DEVICE_DISABLE_GPU_TIMEOUT;

	HRESULT hr = D3D11CreateDeviceAndSwapChain(/*this->adapter*/NULL, /*D3D_DRIVER_TYPE_UNKNOWN*/D3D_DRIVER_TYPE_HARDWARE,
		NULL, flags, levels, 8, D3D11_SDK_VERSION, &sdesc,
		&swapChain, &device, NULL, &deviceContext);

	if (FAILED(hr)) // если устройство никак не создалось, то выходим
	{
		string line = "Error create DX11 device";
		LOGGINGERR(line);
		return false;
	}

	this->currentMode = D3D11_FILLMODE::D3D11_FILLMODE_SOLID;
	this->currentLevel = device->GetFeatureLevel();
	this->createDescsTarget(w, h);
	this->createTargetRender(w, h);
	this->renderStateEdit(currentMode);

	camera.Init(1.5f * GeneralMath::PI, GeneralMath::PI / 4, 180); // создаем камеру

	// свет //
	Color color = { 1,1,1,1 }; // white
	//lights.emplace_back(new DirectionLight({ -30.f, 80.f, 0.f}, color));
	//lights.back()->Enable(true);

	if (!collTextures.addTexture(device, LR"(engine_resource\textures\no_texture.dds)"))
	{
		// тут описываем фатал еррор, что нет заглушки
		return false;
	}

	char* versionShader = "5_0"; // версия шейдерной модели
	collShaders.init(device, LR"(engine_resource\shaders)", versionShader); // загружаем все шейдеры
	if (!rayPick.init(device, L"ray_picking", collShaders))
	{
		// описываем ошибку...
		return false;
	}
	if (!frustum.init(device, L"frustum_culling", collShaders))
	{
		// описываем ошибку...
		return false;
	}

	this->initSceneEditor(); // здесь нужна инициализация сцены для РЕДАКТОРА!!!

	matrixProjection = Matrix::CreatePerspectiveFovLHMatrix(GeneralMath::PI / 4, w / (float)h, SCREEN_NEAR, SCREEN_DEPTH); // перспективная матрица
	runFrustum(); // первый запуск Frustum Culling

    dc.init("Debug output");

	return true;
}

bool Graphics::initSceneEditor()
{
	Matrix m;
	array<wstring, 3> colorInstShaders;
	char* versionShaderModel = "5_0";
	wstring shadersName = L"textures";
	//char* nameFile;
	CharactersModel cm;
	DataFromFile data;

	vector<VertexColor> v;
	vector<WORD> ind;
	Primitive::generatePlaneNet(v, ind, 150, 30);
	net.reset(new ModelDrawColor(versionShaderModel, v, ind, L"color")); // сеть в центре координат
	net->init(device, collTextures, collShaders);


	//// test RayPick - конструирование буферов //
	//RayPick rayPick;
	//// добавляем 3 позиции 1го
	//vector<Vertex> v1(5);
	//vector<WORD> i1(17);
	//Matrix m1_1(1.1);
	//Matrix m1_2(1.2);
	//Matrix m1_3(1.3);
	//rayPick.addModel(v1, i1, m1_1);
	//rayPick.addModelTransformation(0, m1_2);
	//rayPick.addModelTransformation(0, m1_3);
	//// добавляем 2 позиции 2го
	//vector<Vertex> v2(8);
	//vector<WORD> i2(9);
	//Matrix m2_1(2.1);
	//Matrix m2_2(2.2);
	//rayPick.addModel(v2, i2, m2_1);
	//rayPick.addModelTransformation(1, m2_2);
	//// вставляем 1 позицию 1го
	//Matrix m1_4(1.4);
	//rayPick.addModelTransformation(0, m1_4);
	//// добавляем 1 позицию 3го
	//vector<Vertex> v3(50);
	//vector<WORD> i3(80);
	//Matrix m3_1(3.1);
	//Matrix m3_2(3.2);
	//rayPick.addModel(v3, i3, m3_1);
	//rayPick.addModelTransformation(2, m3_2);
	//// вставляем 1 позицию 2го
	//Matrix m2_3(2.3);
	//rayPick.addModelTransformation(1, m2_3);
	//// удаление: тест
	//rayPick.eraseModel(0, 3);
	//rayPick.eraseModel(0, 1);
	//rayPick.eraseModel(0, 1);
	//rayPick.eraseModel(0, 0);
	//rayPick.addModelTransformation(0, m1_1);
	//rayPick.addModelTransformation(0, m1_2);
	//rayPick.addModelTransformation(0, m1_3);
	//return false;
	////
	
	// ОТЛАДКА RAY PICK на CPU
	//nameFile = "engine_resource/models/el_box.pro_object";
	//cm.direction = Matrix::CreateRotateMatrixY(2.78);
	//this->addModel(nameFile, cm);
	//nameFile = "engine_resource/models/stol.pro_object";
	//cm.position = Matrix::CreateTranslationMatrixXYZ(-100, 0, 0);
	//cm.direction = Matrix::Identity();
	//cm.scale = Matrix::Identity();
	//this->addModel(nameFile, cm);
	//Ray = { Point(0.003105, 282.842, -282.8432), Point(-0.2373, -0.5019, 0.8317) };
	/*_model::loadFileFromExpansion("engine_resource/models/el_box.pro_object", &data);
	for (auto& vert : data.vertex)
	{
		Vector3 vertex = (Vector3)vert.Pos;
		vertex.vector3TransformCoord(cm.scale);
		vertex.vector3TransformCoord(cm.direction);
		vertex.vector3TransformCoord(cm.position);
		vert.Pos = vertex;
	}
	cm.position = Matrix::CreateTranslationMatrixXYZ(-100, 0, 0);
	ModelDrawLightPtr model(new ModelDrawLight(versionShaderModel, data.vertex, data.index, data.shaders[0], data.texturesListOut, &data.bboxData));
	auto t = model->calcRayFromModel(Ray, camera);
	_model::loadFileFromExpansion("engine_resource/models/stol.pro_object", &data);
	for (auto& vert : data.vertex)
	{
		Vector3 vertex = (Vector3)vert.Pos;
	vertex.vector3TransformCoord(cm.scale);
	vertex.vector3TransformCoord(cm.direction);
	vertex.vector3TransformCoord(cm.position);
		vert.Pos = vertex;
	}
	model.reset(new ModelDrawLight(versionShaderModel, data.vertex, data.index, data.shaders[0], data.texturesListOut, &data.bboxData));
	t = model->calcRayFromModel(Ray, camera);*/


	// создание пака деревьев
	/*nameFile = "engine_resource/models/tree.pro_object";
	for (int z(-10000); z <= 10000; z += 500) 
	{
		for (int x(-10000); x <=10000 ; x += 500)
		{
			cm.position = Matrix::CreateTranslationMatrixXYZ(x, 0, z);
			float rotate = (0 + rand() % 3) + ((rand() % 100) / 100.);
			cm.direction = Matrix::CreateRotateMatrixY(rotate);
			int scale = (1 + rand() % 2) + +((rand() % 100) / 100.);
			cm.scale = Matrix::CreateScaleMatrixXYZ(scale, scale, scale);
			this->addModel(nameFile, cm);
		}
	}*/

	// зелёный куб
	//cm.color = Color(0, 1, 0, 1);
	//this->addModelColor(0, cm);

	// зелёный куб
	//cm.color = Color(0, 1, 0, 1);
	//cm.position = Matrix::CreateTranslationMatrixZ(500);
	//this->addModelColor(0, cm);

	// создание пака столов
	//nameFile = "engine_resource/models/stol.pro_object";
	//for (int z(-10000); z <= 10000; z += 500)
	//{
	//	for (int x(-10000); x <= 10000; x += 500)
	//	{
	//		cm.position = Matrix::CreateTranslationMatrixXYZ(x, 0, z);
	//		float rotate = (0 + rand() % 3) + ((rand() % 100) / 100.);
	//		cm.direction = Matrix::CreateRotateMatrixY(rotate);
	//		/*int scale = (1 + rand() % 2) + +((rand() % 100) / 100.);
	//		cm.scale = Matrix::CreateScaleMatrixXYZ(scale, scale, scale);*/
	//		this->addModel(nameFile, nullptr, &cm, nullptr, nullptr, nullptr, nullptr);
	//	}
	//}

	// создание пака кубов
	/*for (int z(-1000); z <= 1000; z += 50)
	{
		for (int x(-1000); x <= 1000; x += 50)
		{
			cm.position = Matrix::CreateTranslationMatrixXYZ(x, 0, z);
			this->addModelColor(0, cm);
		}
	}*/

	/*cm.position = Matrix::CreateTranslationMatrixXYZ(0.f, 0.f, 350.f);
	addModel(R"(engine_resource\models\stol_center.pro_object)", nullptr, &cm, nullptr, nullptr, nullptr, nullptr);
	vector<VertexColor> _v;
	vector<WORD> _ind;*/
	/*Primitive::generateSphereNet(_v, _ind, 144.f, 40, 40);
	cm.top = D3D10_PRIMITIVE_TOPOLOGY_LINESTRIP;
	addModelColor(TypePrimitive::Sphere, nullptr,& _v,& _ind, &cm);*/

	return true;
}

void Graphics::renderEditor()
{
	// очищаем задний буфер
	// arg: указатель на целевой объект рендеринга, цвет
	this->updateScene();

	float color[] = { 0.36f, 0.36f, 0.36f, 1.0f }; // структуры color больше нет
	deviceContext->ClearDepthStencilView(DepthStancilBuffer, D3D11_CLEAR_DEPTH, 1.0f, 0);
	deviceContext->ClearRenderTargetView(backBuffer, color);

	// рисуем сетку в центре координат в любом случае
	if (visibleNet)
	{
		deviceContext->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP);
		net->draw(deviceContext, D3D11_FILLMODE::D3D11_FILLMODE_SOLID, camera, matrixProjection);
	}
	//
	auto count = currentMode == D3D11_FILLMODE::D3D11_FILLMODE_MIXED ? 2 : 1;
	_draw(currentMode);


	// осуществляем обмен переднего и заднего буферов
	// arg: как выводить(0 - немедленно, выше - зависимость от частоты экрана), параметры(
	// 0 - отрисовать, DXGI_PRESENT_TEST - тест состояния цепочки переключений(для получения информации, 
	// лежит ли это окно на верху z-последовательности)

	 //debug ray pick на GPU
	/*rayPick.reCreateStructuredBuffers(device); 
	rayPick.update((Point)camera.getViewPosition(), Ray.first, Ray.second);
	DataOutput _do = rayPick.run(device, deviceContext);*/

	HRESULT hr(NULL);
	if (!this->vsync)
		hr = swapChain->Present(0, 0);
	else
		hr = swapChain->Present(1, 0);
	/*if (hr != S_OK)
		LOGMESSAGE("Error present swap chain!");*/
}

bool Graphics::_draw(const D3D11_FILLMODE & mode)
{
	// debug
	//runFrustum();
	//

	renderStateEdit(mode);
	deviceContext->RSSetState(rasterizer);
	static int count(-1);
	int tmpCount(0);
	for (int i(0), k(0) ;i<positionModels.size();i++)
	{
		if (positionModels[i].size() != 0)
		{
			for (int j(0); j < positionModels[i].size(); j++)
			{
				if (frustum.getIndexForDrawing()[k++] || packModels[i]->getType() == TypePrimitive::Frustum) // если модель попала во фрустум // и она усеченная пирамида
				{
					tmpCount++;
					auto character = positionModels[i][j];
					packModels[i]->setWorld(character);
					deviceContext->IASetPrimitiveTopology(character.top);
					if (character.isVisible)
						packModels[i]->draw(deviceContext, mode, camera, matrixProjection);
					if (character.isSelectVisible)
						packModels[i]->drawSelected(deviceContext);
				}
			}
		}
		else
		{
			(dynamic_cast<ModelInstansing*>(packModels[i].get())->reCreateStructuredBuffer(device));
			packModels[i]->draw(deviceContext, mode, camera, matrixProjection);
			packModels[i]->drawSelected(deviceContext);
		}
	}
	if (tmpCount != count)
	{
		count = tmpCount;
		dc.setDataPrint(string("Count render models(no instansing): " + to_string(count)).c_str());
	}
	return true;
}

void Graphics::release()
{
	if (swapChain->SetFullscreenState(false, NULL) != S_OK) // при выходе обязательно переключить из полноэкранного режима
	{
		//LOGMESSAGE("Error reenabled full screen!");
	}
	// на обычный
	if (swapChain)
		swapChain->Release();
	if (device)
		device->Release();
	if (deviceContext)
		deviceContext->Release();
	if (backBuffer)
		backBuffer->Release();
	if (DepthStancil)
		DepthStancil->Release();
	if (DepthStancilBuffer)
		DepthStancilBuffer->Release();
	if (rasterizer)
		rasterizer->Release();
}

bool Graphics::createTargetRender(int w, int h)
{
	ID3D11Texture2D* texture(nullptr);
	HRESULT hr = swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&texture);
	hr = device->CreateRenderTargetView(texture, NULL, &backBuffer);
	if (hr != S_OK)
	{
		string line = "Error Create render target view";
		return false;
	}
	texture->Release();

	if(DepthStancil)
		DepthStancil->Release();
	if (DepthStancilBuffer)
		DepthStancilBuffer->Release();

	hr = device->CreateTexture2D(&descDepth, NULL, &DepthStancil);
	if (hr != S_OK)
	{
		string line = "Error create CreateTexture2D back buffer";
		return false;
	}
	hr = device->CreateDepthStencilView(DepthStancil, &descDSV, &DepthStancilBuffer);
	if (hr != S_OK)
	{
		string line = "Error create ID3D11RenderTargetView";
		return false;
	}

	deviceContext->OMSetRenderTargets(1, &backBuffer, DepthStancilBuffer);
	this->deviceContext->RSSetViewports(1, &vp);

	return true;
}

bool Graphics::resizeWindow(SIZE p)
{
	HRESULT hr(S_OK);

	int w = p.cx;
	int h = p.cy;

	width = (float)w;
	height = (float)h;

	descDepth.Width = w;
	descDepth.Height = h;
	vp.Width = (float)w;
	vp.Height = (float)h;

	this->deviceContext->OMSetRenderTargets(0, 0, 0); // освобождаем устройство от установленных буферов
	this->backBuffer->Release();
	hr = this->swapChain->ResizeBuffers(0, 0, 0, DXGI_FORMAT_UNKNOWN, 0); // вызываем изменение размеров
	// у цепочки переключения буферов. Метод вычисляет сам нужный размер по клиентской части окна
	if (hr != S_OK)
	{
		//LOGMESSAGE("Error resize swap chain!");
		return false;
	}
	this->createTargetRender(w, h);

	matrixProjection = Matrix::CreatePerspectiveFovLHMatrix(GeneralMath::PI / 4, w / (float)h, SCREEN_NEAR, SCREEN_DEPTH); // перспективная матрица
	runFrustum();

	return true;
}

void Graphics::createDescsTarget(int w, int h)
{
	ZeroMemory(&descDepth, sizeof(descDepth));
	descDepth.Width = w;
	descDepth.Height = h;
	descDepth.MipLevels = 1;
	descDepth.ArraySize = 1;
	descDepth.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	descDepth.SampleDesc.Count = 4;
	descDepth.SampleDesc.Quality = 0;
	descDepth.Usage = D3D11_USAGE_DEFAULT;
	descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	descDepth.CPUAccessFlags = 0;
	descDepth.MiscFlags = 0;

	ZeroMemory(&descDSV, sizeof(descDSV));
	descDSV.Format = descDepth.Format;
	descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS; // для качества MSAA = 4, нужен этот параметр
	descDSV.Texture2D.MipSlice = 0;

	vp.Width = (float)w;
	vp.Height = (float)h;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0; 
}

void Graphics::renderStateEdit(const D3D11_FILLMODE& fm)
{
	if (rasterizer)
		rasterizer->Release();
	D3D11_RASTERIZER_DESC wfd;
	ZeroMemory(&wfd, sizeof wfd);
	wfd.FillMode = (D3D11_FILL_MODE)fm;
	wfd.CullMode = D3D11_CULL_BACK;
	wfd.DepthClipEnable = true;
	if (device->CreateRasterizerState(&wfd, &rasterizer) != S_OK)
	{
		//LOGMESSAGE("Error create rasterizer state!");
	}
}

void Graphics::renderStateEdit(const D3D11_CULL_MODE & cm)
{
	if (rasterizer)
		rasterizer->Release();
	D3D11_RASTERIZER_DESC wfd;
	ZeroMemory(&wfd, sizeof wfd);
	wfd.FillMode = D3D11_FILL_MODE::D3D11_FILL_SOLID;
	//wfd.FillMode = (D3D11_FILL_MODE)this->current_mode;
	wfd.CullMode = cm;
	wfd.DepthClipEnable = true;
	if (device->CreateRasterizerState(&wfd, &rasterizer) != S_OK)
	{
		//LOGMESSAGE("Error create rasterizer state!");
	}
}

void Graphics::selectModel(int _x, int _y)
{
	std::pair<Point, Point> ray = Ray = Computer::calculateRay(_x, _y, vp, matrixProjection, camera); // генерируем луч

	this->setDebugData();

	rayPick.update((Point)camera.getViewPosition(), ray.first, ray.second); // обновляем константный буфер
	rayPick.reCreateStructuredBuffers(device); // пересоздаём структурные буферы
	DataOutput _do = rayPick.run(device, deviceContext); // получили индексы 

	if (!isControl) // если контрол не нажат - сначала снимаем выделение у всех моделей
	{
		for (auto iter = selectedModels.begin(); iter != selectedModels.end(); iter++)
		{
			for (auto& i : iter->second)
			{
				if (dynamic_cast<ModelInstansing*>(packModels[iter->first].get()))
					dynamic_cast<ModelInstansing*>(packModels[iter->first].get())->hideBBox(i);
				else
					positionModels[iter->first][i].isSelectVisible = false;
			}
		}
		selectedModels.clear();
	}

	if (_do.indexModel == -1 || _do.indexTransformation == -1) // выделять нечего
		return;
	// выделяем/снимаем выделение у нужных моделей
	if (dynamic_cast<ModelInstansing*>(packModels[_do.indexModel].get()))
		dynamic_cast<ModelInstansing*>(packModels[_do.indexModel].get())->reVisibleBBox(_do.indexTransformation);
	else
	{
		bool cur = positionModels[_do.indexModel][_do.indexTransformation].isSelectVisible;
		positionModels[_do.indexModel][_do.indexTransformation].isSelectVisible = cur ? false : true;
	}

	// и добавляем индекс выделения к карте, хранящей все выделенные модели
	if (selectedModels.find(_do.indexModel) == selectedModels.end()) // индекса нет
		selectedModels.insert({ _do.indexModel, {_do.indexTransformation} });
	else // индекс есть
		selectedModels[_do.indexModel].push_back(_do.indexTransformation);
}

void Graphics::setModeManipulator(int s)
{
	modeManipulator = s;
}

int & Graphics::isModeManipulator()
{
	return this->modeManipulator;
}

string Graphics::addModel(char * path, Coordinate2D * coord, CharactersModel * cm, vector<VertexNormal>* v, vector<WORD>* ind, vector<wstring>* sh, vector<wstring>* pathTextures)
{
	if ((coord && cm) || (!coord && !cm))
	{
		return "Undefined model installation coordinates";
	}
	else
	{
		int count(0);
		if (ind) count++;
		if (v) count++;
		if (sh) count++;
		if (pathTextures) count++;
		if(count != 0 && count != 4)
			return "Inputs parametrs buffer vertex or buffer index or rextures path buffer is null";
	}

	CharactersModel charModel;
	if (coord)
	{
		// сначала генерируем луч + позицию установки модели
		auto ray = Computer::calculateRay(coord->x, coord->y, vp, matrixProjection, camera);
		float t = Computer::intersectionRayAndPlane(Point(0, 0, 0), Point(0, 1, 0), ray); // плоскость, почти в центре, нормаль смотри вверх
		Point p = Computer::getPointIntersectionRayAndPlane(ray, t); // получили точку пересечени плоскости и луча
		charModel.position = Matrix::CreateTranslationMatrixXYZ(p[x], p[y], p[z]); // генерируем матрицу перемещения
	}
	else
		charModel = *cm;

	if (indexModels.find(path) != indexModels.end()) // модель есть
	{
		_model* m = packModels[indexModels.find(path)->second].get();
		m->setType(TypePrimitive::NoPrimitive);
		if (dynamic_cast<ModelInstansing*>(m)) // если модель - инстансинг
		{
			frustum.addModelInstansingFake();
			dynamic_cast<ModelInstansing*>(m)->addModel(charModel); // позицию храним прям в модели
		}
		else
		{
			frustum.addModelTransformation(indexModels.find(path)->second, AllTransformations(charModel.position, charModel.direction, charModel.scale));
			positionModels[indexModels.find(path)->second].push_back(charModel);
		}
		rayPick.addModelTransformation(indexModels.find(path)->second, AllTransformations(charModel.position, charModel.direction, charModel.scale)); // добавляем новую позицию для модели по индексу для ray pick
		return "complete";
	}

	// модели нет, грузим модель
	indexModels.insert({ path, (int)indexModels.size() }); // новая ячейка для моделей 
	DataFromFile data;
	char* versionShaderModel = "5_0";
	vector<VertexColor> bbox;
	vector<Vertex> bboxFrustum;
	BoundingBoxData* bboxData(nullptr);
	if (pathTextures)
	{
		data.vertex = *v;
		data.index = *ind;
		data.shaders = *sh;
		data.texturesListOut = *pathTextures;
		data.isInstansing = false;
	}
	else if (_model::loadFileFromExpansion(path, &data))
	{
		bboxData = &data.bboxData;
	}

	if (data.isInstansing)
	{
		ModelInstansingLightPtr model(new ModelInstansingLight(versionShaderModel, data.vertex, data.index, data.shaders, data.texturesListOut, charModel, bboxData));
		model->init(device, collTextures, collShaders);
		bbox = model->getBoundingBox();
		frustum.addModelInstansingFake();
		model->setType(TypePrimitive::NoPrimitive);
		packModels.push_back(std::move(model));
		positionModels.push_back({}); // в векторе позиций для инстансинговой модели всегда пусто, позиции берутся из самой модели
	}
	else
	{
		ModelDrawLightPtr model(new ModelDrawLight(versionShaderModel, data.vertex, data.index, data.shaders[0], data.texturesListOut, bboxData)); 
		model->init(device, collTextures, collShaders);
		bbox = model->getBoundingBox();
		frustum.addModel(model->getBoundingBoxFrustumCulling(), AllTransformations(charModel.position, charModel.direction, charModel.scale), (Point)model->getCenter());
		model->setType(TypePrimitive::NoPrimitive);
		packModels.push_back(std::move(model));
		positionModels.push_back({ charModel }); // добавляем первую позицию
	}
	rayPick.addModel(bbox, data.vertex, data.index, AllTransformations(charModel.position, charModel.direction, charModel.scale)); // добавляем новую модель для ray pick
	
	return "complete";
}

string Graphics::addModelColor(TypePrimitive type, Coordinate2D * coord, vector<VertexColor>* v, vector<WORD>* ind, CharactersModel * cm)
{
	if ((coord && cm) || (!coord && !cm))
	{
		return "Undefined model installation coordinates";
	}
	else if (!(!ind && !v) && !(ind && v))
	{
		return "Inputs parametrs buffer vertex or buffer index is null";
	}

	// select coordinate
	CharactersModel charModel;
	if (coord)
	{
		// сначала генерируем луч + позицию установки модели
		auto ray = Computer::calculateRay(coord->x, coord->y, vp, matrixProjection, camera);
		float t = Computer::intersectionRayAndPlane(Point(1, 0, 1), Point(0, 1, 0), ray); // плоскость, почти в центре, нормаль смотри вверх
		Point p = Computer::getPointIntersectionRayAndPlane(ray, t); // получили точку пересечени плоскости и луча
		charModel.position = Matrix::CreateTranslationMatrixXYZ(p[x], p[y], p[z]); // генерируем матрицу перемещения
	}
	else
		charModel = *cm;

	// select type model

	ModelDrawColorPtr modelColor;
	char* versionShaderModel = "5_0";
	string name_type;
	vector<VertexColor> bbox;
	vector<VertexColor> _v;
	vector<WORD> _ind;

	// select type model
	switch (type) // TODO: заполнить остальные типы примитивов для создания
	{
	case TypePrimitive::Plane:
	{
		name_type = "plane_color";
		break;
	}

	case TypePrimitive::Cube:
	{
		name_type = "cube_color";
		break;
	}

	case TypePrimitive::Sphere:
	{
		name_type = "sphere_color";
		break;
	}

	case TypePrimitive::Frustum:
	{
		name_type = "frustum_color";
		break;
	}

	default: break;
	}

	// если модель есть - просто добавляем позицию
	if (indexModels.find(name_type) != indexModels.end()) 
	{
		_model* m = packModels[indexModels.find(name_type)->second].get();
		m->setType(type);
		positionModels[indexModels.find(name_type)->second].push_back(charModel);
		frustum.addModelTransformation(indexModels.find(name_type)->second, AllTransformations(charModel.position, charModel.direction, charModel.scale));
		rayPick.addModelTransformation(indexModels.find(name_type)->second, AllTransformations(charModel.position, charModel.direction, charModel.scale)); // добавляем новую позицию для модели по индексу для ray pick
		return "complete";
	}

	// модели нет - создаем
	if (v && ind) // были переданы буферы
	{
		_v = *v;
		_ind = *ind;
	}
	else // был передан только тип
	{
		switch (type) // TODO: заполнить остальные типы примитивов для создания
		{
		case TypePrimitive::Plane:
		{
			Primitive::generatePlane(_v, _ind, 150.f, 30, charModel.color);
			break;
		}
		default: break;
		}
	}
	// создаем модель
	modelColor.reset(new ModelDrawColor(versionShaderModel, _v, _ind, L"color"));
	modelColor->init(device, collTextures, collShaders);
	modelColor->setType(type);
	bbox = modelColor->getBoundingBox();

	// добавляем ее в различные технологии
	frustum.addModel(modelColor->getBoundingBoxFrustumCulling(), AllTransformations(charModel.position, charModel.direction, charModel.scale), (Point)modelColor->getCenter());
	rayPick.addModel(bbox, _v, _ind, AllTransformations(charModel.position, charModel.direction, charModel.scale)); // добавляем новую модель для ray pick

	positionModels.push_back({ charModel }); // в векторе позиций для инстансинговой модели всегда пусто, позиции берутся из самой модели
	indexModels.insert({ name_type, (int)indexModels.size() }); // новая ячейка для моделей 

	// добавляем созданную модель
	packModels.push_back(std::move(modelColor));
	
	return "complete";
}

void Graphics::updateScene()
{
	if (this->isMouseCapture)
		camera.updatePosition();
}

void Graphics::reShowNet()
{
	visibleNet = visibleNet ? false : true;
}

void Graphics::reMode(const D3D11_FILLMODE & mode)
{
	currentMode = mode;
}

void Graphics::setDebugData()
{
	wstring camera_print = L"Camera pos: " + camera.getViewPosition().c_wstr() + L", dir: " + camera.getViewDirection().c_wstr();
	dc.setDataPrintW(camera_print.c_str());
	wstring ray_print = L"Ray pos: " + Ray.first.c_wstr() + L", dir" + Ray.second.c_wstr() + L"\n";
	dc.setDataPrintW(ray_print.c_str());
}

void Graphics::runFrustum()
{
	//if (!frustum.update(device, camera.getView(), matrixProjection, SCREEN_DEPTH*6))
	//{
	//	// error, описать ошибку
	//	MessageBox(NULL, L"Error for frustum culling tecnology", L"Critical error", MB_OK);
	//}
	if (!frustum.update(device, camera, GeneralMath::PI / 4, width / height, SCREEN_NEAR, SCREEN_DEPTH * 6))
	{
		// error, описать ошибку
		MessageBox(NULL, L"Error for frustum culling tecnology", L"Critical error", MB_OK);
		// и выйти
	}
	frustum.run(device, deviceContext);
}

// TODO: перетаскивание моделей при свернутом экране - баг(скорее всего матрица vp)
// TODO: иногда программа не запускается - отловить, гейзенбаг
// TODO: все выделенные индексы надо запоминать -- ЗАПОМИНАЮТСЯ
// TODO: ray pick - добавить метод ИЗМЕНЕНИЯ модели
// TODO: переделать класс raypick с поддержкой распараллеливания(1 поток == 50 моделей, 20 потоков == 1 группа потоков


// план:
// залить на гитхаб
// выделение каёмки модели
// следующее: заняться движком(запустить двиг как игру, продумать основные моменты, подключить освещение(амбиент, спекулар и диффуз))
