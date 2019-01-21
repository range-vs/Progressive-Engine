#pragma once

#include "model_buffers.h"
#include "shaders_buffer.h"
#include "camera.h"

using namespace std;

class FrustumCulling;

struct PlanesFrustum
{
	Vector pl;

	PlanesFrustum(): pl() {}
	PlanesFrustum(const Vector3& n, float d)
	{
		pl = (Vector)n;
		pl[w] = d;
	}
	PlanesFrustum(const Vector& pl): pl(pl){}
};

class FrustumCulling
{
public:
	static const unsigned PLANES_SIZE = 6;

private:
	struct alignas(16) FustrumData
	{
		PlanesFrustum planes[PLANES_SIZE];
		Vector centerNear;
		int countModels;
	};

#pragma region Debug
	vector<VertexColor> v;
	vector<WORD> ind;
public:
	vector<VertexColor> getVertexFrustum() { return v; }
	vector<WORD> getIndexFrustum() { return ind; }
private:
#pragma endregion

	// ������ ��� CPU
	vector< AllTransformations> matrix; // ����� AllTransformations
	vector<int> flags; // �������� ����� ������(0 - �� ����� � Frustum, 1 - �����)
	vector<int> offset; // ����� �������� ������
	vector<int> offsetCurrent; // ����� �������� ���������� ������
	vector<Point> vertex; // ����� ������ bbox(�� 8 ������)
	vector<Point> centers; // ����� ������ ������� bbox

	int countMatrix; // ���-�� ������

	// ������ ��� GPU
	bool isReCreate; // ��������� �� ������������� ����������� ������
	// �������
	ID3D11Buffer* physicBufferMatrix;
	ID3D11ShaderResourceView* physicShaderResourceViewMatrix;
	// ����� - �������� �����
	ID3D11Buffer* physicBufferFlags;
	ID3D11UnorderedAccessView* physicShaderResourceViewFlags;
	ID3D11Buffer* outFlags; // ��������� �����, ��� ����������� � ���� ����������
	// �������
	ID3D11Buffer* physicBufferOffset;
	ID3D11ShaderResourceView* physicShaderResourceViewOffset;
	// ������� �������
	ID3D11Buffer* physicBufferModels;
	ID3D11ShaderResourceView* physicShaderResourceViewModels;
	// ������� �������
	ID3D11Buffer* physicBufferCenters;
	ID3D11ShaderResourceView* physicShaderResourceViewCenters;

	// ������
	ID3D11ComputeShader* shader;
	wstring pathToShader; // ���� � �������

	// ����������� �����
	ID3D11Buffer* constantBufferData; // ����������� �����, ������
	FustrumData data;
	
	template<class Type>
	bool recreateStructuredBuffer(ID3D11Device* device, const vector<Type>& arr, ID3D11Buffer** buf, ID3D11ShaderResourceView** resView); // ������������ ���� ����������� �������
	AllTransformations allTransponse(AllTransformations m);
public:
	FrustumCulling():physicBufferMatrix(nullptr), physicShaderResourceViewMatrix(nullptr),
		physicBufferFlags(nullptr), physicShaderResourceViewFlags(nullptr), outFlags(nullptr),
		physicBufferOffset(nullptr), physicShaderResourceViewOffset(nullptr),
		physicBufferModels(nullptr), physicShaderResourceViewModels(nullptr),
		physicBufferCenters(nullptr), physicShaderResourceViewCenters(nullptr),
		shader(nullptr), constantBufferData(nullptr), isReCreate(false), countMatrix(0) {}

	void addModelInstansingFake(); // ��������� ���� ������� �������������� ������
	void addModel(const vector<Vertex>& bbox, AllTransformations& m, const Point& cen); // ��������� ����� ������
	bool addModelTransformation(int indexModel, AllTransformations& m); // ��������� ����� ������� ��� ������ �� �������
	bool eraseModel(int indexModel, int indexTransformation); // ������ ������������� �� ������� �� ������ �� �������
	bool updateModel(int indexModel, int indexTransformation, const vector<Vertex>& bbox, const Point& cen); // ��������� bbox � ������
	bool updateModel(int indexModel, int indexTransformation,  AllTransformations& m); // ��������� ������������� � ������
	vector<int> getIndexForDrawing(); // �������� ������ ������, �� ������� ���� �������� ������
	bool reCreateStructuredBuffers(ID3D11Device* device); // ���������� ��� ����������� ������
	bool updateBuffers(ID3D11Device* device); // ���������� �������� �����

	//bool update(ID3D11Device* device, const Matrix & view, const Matrix& proj, float screenDepth); // ��������� ����������� ����� (��������� �������)
	bool update(ID3D11Device* device, const Camera & view, float angle, float ratio, float _near, float rear); // ��������� ����������� ����� (��������� �������)
	bool init(ID3D11Device* device, const wstring& path, CollectionShaders& shaders); // ������ �������� �����
	bool run(ID3D11Device* device, ID3D11DeviceContext* deviceContext); // ��������� Frustum culling �� �������������� �������

	~FrustumCulling();
};

template<class Type>
inline bool FrustumCulling::recreateStructuredBuffer(ID3D11Device * device, const vector<Type>& arr, ID3D11Buffer ** buf, ID3D11ShaderResourceView ** resView)
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
