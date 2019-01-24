#pragma once

// my
#include "fustrum_culling.h"
#include "ray_pick.h"
#include "model.h"
#include "../utilites/calculate.h"
#include "..\io\ListenersEngine.h"
#include "primitive.h"

#include "../utilites/debug_console.h"

//c++
#include <array>
#include <vector>
#include <list>
#include <map>


class Graphics // Одиночка
{
	IDXGISwapChain* swapChain; // цепочка буферов для переключения(передний и несколько задних)
	ID3D11Device* device; // служит для создания ресурсов
	ID3D11DeviceContext* deviceContext; // указатель на контекст устройства(в основном работа с видео памятью и рендеринг)
	ID3D11RenderTargetView* backBuffer; // целевой объект рендеринга(его мы будем рендерить)
	ID3D11Texture2D* DepthStancil; // текстура буфера глубины
	ID3D11DepthStencilView* DepthStancilBuffer; // буфер глубины
	ID3D11RasterizerState* rasterizer; // тип растеризации(линии, грани и т.д.)
	
	CollectionTextures collTextures; // буфер текстур
	CollectionShaders collShaders; // буфер шейдеров
	bool visibleNet; // видимость сети
	ModelDrawColorPtr net; // сеть в центре координат
	map<string, int> indexModels; // индексы моделей
	vector<Model> packModels; // все модели в памяти
	vector<vector<CharactersModel>> positionModels; // все позиции всех моделей на карте

	// ray pick
	bool isControl; // нажат ли шифт
	int modeManipulator; // режим манипулирования сценой
	map<int, vector<int>> selectedModels; // индексы выделенных моделей и их матриц
	HWND hwnd; // дескриптор окна, к которому привязано устройство
	RayPick rayPick; // осуществление технологии ray pick

	// Frustum culling
	FrustumCulling frustum; // класс фрустума

	// управление камерой
	Camera camera; // камера(матрица вида)
	Matrix matrixProjection; // матрица проекции перспективы
	bool isMouseCapture; // захвачена ли мышь?
	POINT lastPoint; // полученная позиция мыши после НАЖАТИЯ ЛКМ
	
	D3D11_TEXTURE2D_DESC descDepth; //
	D3D11_DEPTH_STENCIL_VIEW_DESC descDSV; // описание структуры буфера глубины и трафарета
	D3D11_VIEWPORT vp; // структура порта вывода(вида)
	IDXGIAdapter1* adapter; // выбранный адаптер
	D3D_FEATURE_LEVEL currentLevel; // текущий используемый DirectX
	bool vsync; // вертикальная синхронизация
	int numerator; // числитель частоты обновления картинки
	int denominator; // знаменатель
	D3D11_FILLMODE currentMode; // текущий режим отрисовки

	bool createTargetRender(int w, int h); // создаем целевое устройство рендеринга
	void createDescsTarget(int w, int h); // создаем описание поверхностей для целевого устройства рендеринга
	bool _draw(const D3D11_FILLMODE& m); // отрисовка с определённым режимом

	debug_console dc;
	pair<Point, Point> Ray;

	float SCREEN_DEPTH = 1000.f;
	float SCREEN_NEAR = 0.01f;
	float width;
	float height;
public:
	void setDebugData();

private:
	Graphics() {};
	Graphics(const Graphics& g);
	Graphics& operator=(const Graphics& g);
	static Graphics* MainDevice;

	void constructor(D3D11_FILLMODE cm = D3D11_FILLMODE_SOLID, bool vs = true);

public:
	static Graphics* getInstance(D3D11_FILLMODE cm = D3D11_FILLMODE_SOLID, bool vs = true)
	{
		if (!MainDevice)
		{
			MainDevice = new Graphics();
			MainDevice->constructor(cm, vs);
		}
		return MainDevice;
	}

public:
	// выравниваение памяти!!!
	void* operator new(size_t i);
	void operator delete(void* p);

	bool initEditor(HWND hwnd);
	bool initGame(HWND hwnd);
	bool initSceneEditor();
	void renderEditor();
	void renderGame();
	void release();
	bool resizeWindow(SIZE p);
	void renderStateEdit(const D3D11_FILLMODE&);
	void renderStateEdit(const D3D11_CULL_MODE&);
	ID3D11DeviceContext* getContext() { return deviceContext; }
	ID3D11Device* getDevice() { return device; }
	Matrix& getProjection() { return matrixProjection; }

	// массовое выделение
	void setPressControl(bool stat) { this->isControl = stat; }  // нажимаем/отжимаем контрол
	int IsControl()const { return this->isControl; } // нажат ли контрол

	// управление камерой
	Camera& getCamera() { return this->camera; } // геттер получения камеры
	void setMauseCapture(bool mc) {this->isMouseCapture = mc; }  // изменяем режим захвата мыши
	bool getMauseCapture() { return this->isMouseCapture; }  // получаем режим захвата мыши
	void updateScene(); // обновляем сцену

	// доступ к дескриптору
	HWND getDesctiptor() { return this->hwnd; }

	// ray pick 
	void selectModel(int _x, int _y); // метод выделения моделей
	void setModeManipulator(int s); // установка режима манипулирования сценой
	int& isModeManipulator(); // получаем режим манипулирования сценой

	// frustum culling
	FrustumCulling& getFrustumCulling() { return frustum; }
	void runFrustum(); // запускаем проверку на попадание

	// установка моделей на карту
	// TODO: привязка пока только к плоскости y = 0
	string addModelColor(TypePrimitive type, Coordinate2D* coord, vector<VertexColor>* v, vector<WORD>* ind, CharactersModel* cm); // добавляем новую модель в цвете
	string addModel(char* path, Coordinate2D* coord, CharactersModel* cm, vector<VertexNormal>* v, vector<WORD>* ind, vector<wstring>* sh, vector<wstring>* pathTextures); // добавляем новую модель
	void editModel(float scale)
	{
		positionModels[0][0].scale(0, 0) += scale;
		positionModels[0][0].scale(1, 1) += scale;
		positionModels[0][0].scale(2, 2) += scale;
	}

	// инструменты
	void reShowNet(); // показ / сокрытие сетки в центре координат
	void reMode(const D3D11_FILLMODE& mode); // смена режима отрисовки
};
