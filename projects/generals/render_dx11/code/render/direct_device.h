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


class Graphics // ��������
{
	IDXGISwapChain* swapChain; // ������� ������� ��� ������������(�������� � ��������� ������)
	ID3D11Device* device; // ������ ��� �������� ��������
	ID3D11DeviceContext* deviceContext; // ��������� �� �������� ����������(� �������� ������ � ����� ������� � ���������)
	ID3D11RenderTargetView* backBuffer; // ������� ������ ����������(��� �� ����� ���������)
	ID3D11Texture2D* DepthStancil; // �������� ������ �������
	ID3D11DepthStencilView* DepthStancilBuffer; // ����� �������
	ID3D11RasterizerState* rasterizer; // ��� ������������(�����, ����� � �.�.)
	
	CollectionTextures collTextures; // ����� �������
	CollectionShaders collShaders; // ����� ��������
	bool visibleNet; // ��������� ����
	ModelDrawColorPtr net; // ���� � ������ ���������
	map<string, int> indexModels; // ������� �������
	vector<Model> packModels; // ��� ������ � ������
	vector<vector<CharactersModel>> positionModels; // ��� ������� ���� ������� �� �����

	// ray pick
	bool isControl; // ����� �� ����
	int modeManipulator; // ����� ��������������� ������
	map<int, vector<int>> selectedModels; // ������� ���������� ������� � �� ������
	HWND hwnd; // ���������� ����, � �������� ��������� ����������
	RayPick rayPick; // ������������� ���������� ray pick

	// Frustum culling
	FrustumCulling frustum; // ����� ��������

	// ���������� �������
	Camera camera; // ������(������� ����)
	Matrix matrixProjection; // ������� �������� �����������
	bool isMouseCapture; // ��������� �� ����?
	POINT lastPoint; // ���������� ������� ���� ����� ������� ���
	
	D3D11_TEXTURE2D_DESC descDepth; //
	D3D11_DEPTH_STENCIL_VIEW_DESC descDSV; // �������� ��������� ������ ������� � ���������
	D3D11_VIEWPORT vp; // ��������� ����� ������(����)
	IDXGIAdapter1* adapter; // ��������� �������
	D3D_FEATURE_LEVEL currentLevel; // ������� ������������ DirectX
	bool vsync; // ������������ �������������
	int numerator; // ��������� ������� ���������� ��������
	int denominator; // �����������
	D3D11_FILLMODE currentMode; // ������� ����� ���������

	bool createTargetRender(int w, int h); // ������� ������� ���������� ����������
	void createDescsTarget(int w, int h); // ������� �������� ������������ ��� �������� ���������� ����������
	bool _draw(const D3D11_FILLMODE& m); // ��������� � ����������� �������

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
	// ������������� ������!!!
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

	// �������� ���������
	void setPressControl(bool stat) { this->isControl = stat; }  // ��������/�������� �������
	int IsControl()const { return this->isControl; } // ����� �� �������

	// ���������� �������
	Camera& getCamera() { return this->camera; } // ������ ��������� ������
	void setMauseCapture(bool mc) {this->isMouseCapture = mc; }  // �������� ����� ������� ����
	bool getMauseCapture() { return this->isMouseCapture; }  // �������� ����� ������� ����
	void updateScene(); // ��������� �����

	// ������ � �����������
	HWND getDesctiptor() { return this->hwnd; }

	// ray pick 
	void selectModel(int _x, int _y); // ����� ��������� �������
	void setModeManipulator(int s); // ��������� ������ ��������������� ������
	int& isModeManipulator(); // �������� ����� ��������������� ������

	// frustum culling
	FrustumCulling& getFrustumCulling() { return frustum; }
	void runFrustum(); // ��������� �������� �� ���������

	// ��������� ������� �� �����
	// TODO: �������� ���� ������ � ��������� y = 0
	string addModelColor(TypePrimitive type, Coordinate2D* coord, vector<VertexColor>* v, vector<WORD>* ind, CharactersModel* cm); // ��������� ����� ������ � �����
	string addModel(char* path, Coordinate2D* coord, CharactersModel* cm, vector<VertexNormal>* v, vector<WORD>* ind, vector<wstring>* sh, vector<wstring>* pathTextures); // ��������� ����� ������
	void editModel(float scale)
	{
		positionModels[0][0].scale(0, 0) += scale;
		positionModels[0][0].scale(1, 1) += scale;
		positionModels[0][0].scale(2, 2) += scale;
	}

	// �����������
	void reShowNet(); // ����� / �������� ����� � ������ ���������
	void reMode(const D3D11_FILLMODE& mode); // ����� ������ ���������
};
