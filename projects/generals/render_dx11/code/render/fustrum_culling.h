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

	// буферы дл€ CPU
	vector< AllTransformations> matrix; // буфер AllTransformations
	vector<int> flags; // выходной буфер флагов(0 - не попал в Frustum, 1 - попал)
	vector<int> offset; // буфер смещений матриц
	vector<int> offsetCurrent; // буфер текущего количества матриц
	vector<Point> vertex; // буфер вершин bbox(по 8 вершин)
	vector<Point> centers; // буфер вершин центров bbox

	int countMatrix; // кол-во матриц

	// буферы дл€ GPU
	bool isReCreate; // требуетс€ ли пересоздавать структурные буферы
	// матрицы
	ID3D11Buffer* physicBufferMatrix;
	ID3D11ShaderResourceView* physicShaderResourceViewMatrix;
	// флаги - выходной буфер
	ID3D11Buffer* physicBufferFlags;
	ID3D11UnorderedAccessView* physicShaderResourceViewFlags;
	ID3D11Buffer* outFlags; // временный буфер, дл€ копировани€ в него информации
	// индексы
	ID3D11Buffer* physicBufferOffset;
	ID3D11ShaderResourceView* physicShaderResourceViewOffset;
	// вершины моделей
	ID3D11Buffer* physicBufferModels;
	ID3D11ShaderResourceView* physicShaderResourceViewModels;
	// вершины моделей
	ID3D11Buffer* physicBufferCenters;
	ID3D11ShaderResourceView* physicShaderResourceViewCenters;

	// шейдер
	ID3D11ComputeShader* shader;
	wstring pathToShader; // путь к шейдеру

	// константный буфер
	ID3D11Buffer* constantBufferData; // константный буфер, данные
	FustrumData data;
	
	template<class Type>
	bool recreateStructuredBuffer(ID3D11Device* device, const vector<Type>& arr, ID3D11Buffer** buf, ID3D11ShaderResourceView** resView); // пересоздание всех структурных буферов
	AllTransformations allTransponse(AllTransformations m);
public:
	FrustumCulling():physicBufferMatrix(nullptr), physicShaderResourceViewMatrix(nullptr),
		physicBufferFlags(nullptr), physicShaderResourceViewFlags(nullptr), outFlags(nullptr),
		physicBufferOffset(nullptr), physicShaderResourceViewOffset(nullptr),
		physicBufferModels(nullptr), physicShaderResourceViewModels(nullptr),
		physicBufferCenters(nullptr), physicShaderResourceViewCenters(nullptr),
		shader(nullptr), constantBufferData(nullptr), isReCreate(false), countMatrix(0) {}

	void addModelInstansingFake(); // добавл€ем фейк текущей инстансинговой модели
	void addModel(const vector<Vertex>& bbox, AllTransformations& m, const Point& cen); // добавл€ем новую модель
	bool addModelTransformation(int indexModel, AllTransformations& m); // добавл€ем новую позицию дл€ модели по индексу
	bool eraseModel(int indexModel, int indexTransformation); // удал€ю трансформацию по индексу из модели по индексу
	bool updateModel(int indexModel, int indexTransformation, const vector<Vertex>& bbox, const Point& cen); // обновл€ем bbox у модели
	bool updateModel(int indexModel, int indexTransformation,  AllTransformations& m); // обновл€ем трансформацию у модели
	vector<int> getIndexForDrawing(); // получает массив флагов, по которым надо рисовать модели
	bool reCreateStructuredBuffers(ID3D11Device* device); // пересоздаЄм все структурные буферы
	bool updateBuffers(ID3D11Device* device); // пересоздаЄм выходной буфер

	//bool update(ID3D11Device* device, const Matrix & view, const Matrix& proj, float screenDepth); // обновл€ем константный буфер (конкретно фуструм)
	bool update(ID3D11Device* device, const Camera & view, float angle, float ratio, float _near, float rear); // обновл€ем константный буфер (конкретно фуструм)
	bool init(ID3D11Device* device, const wstring& path, CollectionShaders& shaders); // создаЄм выходной буфер
	bool run(ID3D11Device* device, ID3D11DeviceContext* deviceContext); // запускаем Frustum culling на вычислительном шейдере

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
