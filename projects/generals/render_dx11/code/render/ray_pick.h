#pragma once

#include "model_buffers.h"
#include "camera.h"
#include "shaders_buffer.h"

using namespace std;
using namespace gmath;

struct CommonData // ����������� �����
{
	Vector cameraPosition; // ������� ������
	Vector rayPosition; // ������� ����
	Vector rayDirection; // ����������� ����
	Vector countModel; // ���������� �������(� x - ����������)

	CommonData():cameraPosition(), rayPosition(), rayDirection(), countModel(){}
};

struct DataOutput // �������� �����
{
	int indexModel;
	int indexTransformation;

	DataOutput():indexModel(-1), indexTransformation(-1){}
};

struct Offset // ��������
{
	unsigned countVertexAllPred;
	unsigned countVertex;
	unsigned countIndecedsAllPred;
	unsigned countIndeceds;
	unsigned countTransformations;

	Offset():countVertexAllPred(0), countVertex(0), countIndecedsAllPred(0), countIndeceds(0), countTransformations(0){}
	Offset(unsigned cvp, unsigned cv, unsigned cip, unsigned ci, unsigned ct) :countVertexAllPred(cvp), countVertex(cv), countIndecedsAllPred(cip), countIndeceds(ci), 
		countTransformations(ct){}

};

class RayPick
{
	// ������ ��� CPU
	vector<Point> points; // ������ ������
	vector<Point> bbox; // bbox ������ ������ -- �������� ������ 36 ������ ��� ������ ������ CommonData.countModel
	vector<int> indeceds; // ������ ��������
	vector<vector<AllTransformations>> transformations; // ������ ������������� 
	vector<Offset> offsets; // ������ ��������

	// ������ ��� GPU
	bool isReCreate; // ��������� �� ������������� ����������� ������
	// �������
	ID3D11Buffer* physicBufferPoints;
	ID3D11ShaderResourceView* physicShaderResourceViewPoints;
	// bbox
	ID3D11Buffer* physicBufferBBoxPoints;
	ID3D11ShaderResourceView* physicShaderResourceViewBBoxPoints;
	// �������
	ID3D11Buffer* physicBufferIndeceds;
	ID3D11ShaderResourceView* physicShaderResourceViewIndeceds;
	// �������������
	ID3D11Buffer* physicBufferTransformation;
	ID3D11ShaderResourceView* physicShaderResourceViewTransformation;
	// ��������
	ID3D11Buffer* physicBufferOffsets;
	ID3D11ShaderResourceView* physicShaderResourceViewOffsets;

	// �������� ����� ��� GPU
	ID3D11Buffer* physicBufferOut; 
	ID3D11UnorderedAccessView* physicShaderResourceViewOut; 
	ID3D11Buffer* outBuffer; // ��������� �����, ��� ����������� � ���� ����������


	// ������
	ID3D11ComputeShader* shader;

	// ����������� �����
	ID3D11Buffer* constantBufferData; // ����������� �����, ������
	CommonData BufferData;

	// ��������
	size_t countVertexAll; // ����� ���-�� ������
	size_t countIndecedAll; // ����� ���-�� ��������
	size_t countTransformationAll; // ����� ���-�� �������������

	// ���� - ���� � �������
	wstring pathToShader;

	template<class Type>
	bool recreateStructuredBuffer(ID3D11Device* device, const vector<Type>& arr, ID3D11Buffer** buf, ID3D11ShaderResourceView** resView); // ������������ ���� ����������� �������
	bool reCreateStructuredBufferTransformations(ID3D11Device* device); // ������������ ������������ ������ �������������
public:
	RayPick():isReCreate(false), countVertexAll(0), countIndecedAll(0), countTransformationAll(0), physicBufferPoints(nullptr), physicShaderResourceViewPoints(nullptr),
		physicBufferBBoxPoints(nullptr), physicShaderResourceViewBBoxPoints(nullptr),
		physicBufferIndeceds(nullptr), physicShaderResourceViewIndeceds(nullptr), physicBufferTransformation(nullptr), 
		physicShaderResourceViewTransformation(nullptr), physicBufferOffsets(nullptr), physicShaderResourceViewOffsets(nullptr),
		physicBufferOut(nullptr), physicShaderResourceViewOut(nullptr), outBuffer(nullptr), shader(nullptr), constantBufferData(nullptr){}

	template<class Vert>
	void addModel(const vector<VertexColor>& bbox, const vector<Vert>& v, const vector<WORD>& ind, AllTransformations& m); // ����� ������
	void addModelTransformation(int index, AllTransformations& m); // ��������� ����� ������������� ��� ������ �� �������
	bool eraseModel(int index, int indexTransformation = -1); // ������� ������������� �� indexTransformation, ������ �� index (-1 - �������� ���� ������, �� ����������� ���������)
	bool reCreateStructuredBuffers(ID3D11Device* device); // ���������� ��� ����������� ������

	void update(const Point& cameraPosition, const Point& rayPosition, const Point& rayDirection); // ��������� ����������� �����
	bool init(ID3D11Device* device, const wstring& path, CollectionShaders& shaders); // ������ �������� �����
	DataOutput run(ID3D11Device* device, ID3D11DeviceContext* deviceContext); // ��������� ray pick �� �������������� �������

	~RayPick();
};

template<class Type>
inline bool RayPick::recreateStructuredBuffer(ID3D11Device* device, const vector<Type>& arr, ID3D11Buffer ** buf, ID3D11ShaderResourceView ** resView)
{
	if (arr.empty())
		return true;
	if (!this->isReCreate)
		return true;
	size_t vfStride = sizeof(arr[0]);

	if (*buf != nullptr)    (*buf)->Release();
	if (*resView != nullptr) (*resView)->Release();

	D3D11_BUFFER_DESC bufferDesc;
	ZeroMemory(&bufferDesc, sizeof(bufferDesc));
	bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	bufferDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	bufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	bufferDesc.StructureByteStride = (UINT)vfStride;
	bufferDesc.ByteWidth = (UINT)(arr.size() * vfStride);
	D3D11_SUBRESOURCE_DATA bufferData;
	ZeroMemory(&bufferData, sizeof(bufferData));
	bufferData.pSysMem = arr.data();
	HRESULT hr = device->CreateBuffer(&bufferDesc, &bufferData, buf);
	if (hr != S_OK)
		return false;

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	ZeroMemory(&srvDesc, sizeof(srvDesc));
	srvDesc.Format = DXGI_FORMAT_UNKNOWN;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
	srvDesc.Buffer.ElementWidth = (UINT)arr.size();
	hr = device->CreateShaderResourceView(*buf, &srvDesc, resView);
	if (hr != S_OK)
		return false;
	return true;
}


template<class Vert>
void RayPick::addModel(const vector<VertexColor>& bbox, const vector<Vert>& v, const vector<WORD>& ind, AllTransformations& m)
{
	vector<Point> * p = &this->bbox;
	for_each(bbox.begin(), bbox.end(), [p](const VertexColor& vert) {
		p->push_back(vert.Pos);
	});
	p = &this->points;
	for_each(v.begin(), v.end(), [p](const Vert& vert) {
		p->push_back(vert.Pos);
	});
	copy(ind.begin(), ind.end(), back_inserter(indeceds));
	m.position.transponse();
	m.rotation.transponse();
	m.scale.transponse();
	transformations.push_back({ m });
	Offset offset(-1, (unsigned)v.size(), -1, (unsigned)ind.size(), 1);
	if (offsets.size() == 0)
	{
		offset.countVertexAllPred = 0;
		offset.countIndecedsAllPred = 0;
	}
	else
	{
		offset.countVertexAllPred = (unsigned)countVertexAll;
		offset.countIndecedsAllPred = (unsigned)countIndecedAll;
	}
	countVertexAll += v.size();
	countIndecedAll += ind.size();
	countTransformationAll++;
	offsets.push_back(offset);
	isReCreate = true;
}

// TODO: ���������� ����������� � ���������� ����������� - �������
// TODO: ��������� ������ � ����������������� ���������� � ���
// TODO: ������� ����� ���������� ������ � ���������� ������ 