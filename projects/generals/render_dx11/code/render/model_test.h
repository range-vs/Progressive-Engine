#pragma once

// c++
#include <sstream>
#include <fstream>
#include <vector>
#include <array>
#include <functional>
#include <map>
#include <algorithm>
#include <iostream>
#include <set>
#include <string>

using std::function;
using std::vector;
using std::array;
using std::pair;
using std::map;
using std::set;
using std::string;
using std::wstring;

// my
#include "Camera.h"
#include "Light.h"

struct LogicBuffer
{
	Matrix position;

	LogicBuffer(Matrix v);
};

struct PhysicBuffer
{
	ID3D11Buffer* physicBuffer;
	ID3D11ShaderResourceView* physicShaderResourceView;
	size_t size; 

	std::vector< LogicBuffer> bufferCPU;

	PhysicBuffer() = default;

	void allocate(LogicBuffer lb);
	void release(int index);
	void rebuildPages(ID3D11Device* device);
};


class Model
{
	PhysicBuffer pBuffer;

	ID3D11Buffer* VertexBuffer;
	ID3D11Buffer* IndexBuffer;

	size_t count_vertex;
	size_t count_index;
	ID3D11Buffer* ConstantsBufferVS;
	ID3D11InputLayout* VertexFormat;
	Matrix World;

	ID3D11VertexShader* VertexShader;
	ID3D11PixelShader* PixelShader;


	// иницализация
	ID3DBlob* tmpVertexBuffer; // набор байтов вершинного буфера(нужно для создания модели)
	UINT size_byte_type_vertex; // размер в байтах одной вершины

public:

	Model(D3D11_FILLMODE mode = D3D11_FILLMODE_SOLID,
		D3D_PRIMITIVE_TOPOLOGY top = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST,
		bool vis = true) :VertexFormat(nullptr), VertexBuffer(nullptr), IndexBuffer(nullptr),
		ConstantsBufferVS(nullptr), World(Matrix::Identity()),
		count_vertex(0), count_index(0), size_byte_type_vertex(0) {}

	// инициализация и отрисовка
	bool Init(ID3D11Device* device, const vector<VertexColor>& v, const vector<WORD>& ind);
	bool Draw(ID3D11DeviceContext * devcontx, Camera camera, const Matrix & Projection);
	HRESULT CompileShaderFromFile(WCHAR* szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut);
};
