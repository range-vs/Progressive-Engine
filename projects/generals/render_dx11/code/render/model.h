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
#include <list>

using std::function;
using std::vector;
using std::array;
using std::pair;
using std::map;
using std::set;
using std::string;
using std::wstring;
using std::list;

// my
#include "Camera.h"
#include "Light.h"
#include "textures_buffer.h"
#include "shaders_buffer.h"
#include "model_buffers.h"
#include "primitive.h"

// ������������:
// ����� - � ��������� �����, ����� ������, � ������� �����
// ������� ������  - � ��������� �����, ����� ����� "_"
// �������� ������ - � ��������� �����, ����� ������, � ������� �����

// ����������
class BoundingBox;
class BoundingBoxInstansing;
class Triangle;
class TrianglePred;
class TriangleInstancedPred;
class _model;
class ModelDraw;
class ModelDrawColor;
class ModelDrawLight;
class ModelInstansing;
class ModelInstansingColor;
class ModelInstansingLight;

// ���� ������ ����������
using BBox = brain_memory<BoundingBox>;
using Model = brain_memory<_model>;
using ModelDrawPtr = brain_memory< ModelDraw>;
using ModelInstansingPtr = brain_memory< ModelInstansing>;
using ModelDrawColorPtr = brain_memory< ModelDrawColor>;
using ModelDrawLightPtr = brain_memory< ModelDrawLight>;
using BBoxInst = brain_memory<BoundingBoxInstansing>;
using ModelInstansingColorPtr = brain_memory<ModelInstansingColor>;
using ModelInstansingLightPtr = brain_memory<ModelInstansingLight>;
using PhysicPositions = brain_memory< PhysicBuffer<AllTransformations>>;

// ������

class BoundingBox // �������������� �����
{
protected:
	struct LastPoints
	{
		float xl, yd, zf; // ����, ��� � �����
		float xr, yu, zr; // �����, ���� � ���
	};
	Vector center; // ����� �������. �����
	vector<VertexColor> arrayPointsBox; // ������ ������ ����� ��� ray pick
	vector<Vertex> arrayPointsFrustum; // ������ ������ ����� ��� frustum culling
	vector<VertexColor> arrayForDraw; // ������ ������ ����� ��� ���������
	ID3D11VertexShader* vertexShader; // ��������� ������
	ID3D11PixelShader* pixelShader; // ���������� ������
	ID3D11InputLayout* vertexFormat; // ������ ������
	ID3D11Buffer* vertexBuffer; // ����� ������
	ID3D11Buffer* constantsBufferVS; // ����������� ����� ���������
	bool visible; // ����� �� ���� �����
	ID3D11Buffer* constantsBufferPS; // ����������� ����� ������ ���������

	void shiftCoordinate(LastPoints& lp, float c); // ����� ���� ���������
	void shiftCoordinateProcent(LastPoints& lp, float pr); // ����� ���� ��������� �� �������
	vector<VertexColor> generateVertexForDraw(const LastPoints& lp); // ��������� ������� ������ ��� ���������
	vector<Vertex> generateVertexForFrustumCulling(const LastPoints& lp); // ��������� ������� ������ ��� frustum culling
	Vector calculateCenterBoundingBox(const LastPoints& lp); // ������� ������
	vector<VertexColor> generateVertexForRayPick(const LastPoints& lp); // ��������� ������� ������ ��� ray pick
	void move_data(BoundingBox&& bb); // ����������� ������
	bool _init(ID3D11Device* device, CollectionShaders& collShaders, const wstring& vertexShader); // ������� ����� �������������, ����������

	BoundingBox(){}
public:
	// ������������
	BoundingBox(float xl, float yd, float zf, float xr, float yu, float zr);
	BoundingBox(float xl, float yd, float zf, float xr, float yu, float zr, const Vector& c);
	BoundingBox(BoundingBox&& bb);
	const BoundingBox& operator=(BoundingBox&& bb);

	// ������������� � �������
	Vector& getCenterBoundingBox(); // �������� ����� ������
	vector<VertexColor>& getVertexForRayPick(); // �������� ����� ��� �������� ��������� ����
	vector<VertexColor>& getVertexForDraw(); // �������� ����� ��� ���������
	vector<Vertex>& getVertexForFrustumCulling(); // �������� ����� ��� frustum culling


	// ������
	bool init(ID3D11Device* device, CollectionShaders& collShaders, const wstring& shader); // ������������� BBox
	void draw(ID3D11DeviceContext * devcontx, const ConstantBufferNoLight& buff); // ��������� BBox

	// ����������
	virtual ~BoundingBox();
};

class BoundingBoxInstansing : public BoundingBox // �������������� ����� ��� �����������
{

	PhysicVisibles physicVisible; // ���������� ����� ��� ������
	ID3D11GeometryShader* geometryShader; // �������������� ������
public:
	// ������������
	BoundingBoxInstansing(float xl, float yd, float zf, float xr, float yu, float zr);
	BoundingBoxInstansing(float xl, float yd, float zf, float xr, float yu, float zr, const Vector& v);
	BoundingBoxInstansing(BoundingBoxInstansing&& bbi);
	const BoundingBoxInstansing& operator=(BoundingBoxInstansing&& bbi);

	// ������������� � �������
	void reVisible(int index); // �����/�������� �������� �� �������
	void show(int index); // �������� ������� �� �������
	void hide(int index); // ������ ������� �� �������
	bool isVisible(int index); // ����� �� ������� �� �������

	// ������
	void reCreateStructuredBuffer(ID3D11Device* device); // ����������� ����� ������� �����������
	void addModel(); // ��������� ����� �������
	bool eraseModel(int index); // ������� ������� �� �������
	void draw(ID3D11DeviceContext * devcontx, const ConstantBufferNoLightInstansing& buff, ID3D11ShaderResourceView* arrayPosition); // ��������� BBoxInst
	bool init(ID3D11Device* device, CollectionShaders& collShaders, const array<wstring,3 >& _shaders); // ������������� BBox
	template<class T> void transformModel(vector<T>& v, const Matrix& m); // ��������� ���� ������ �� ������� ��������������

	// ����������
	~BoundingBoxInstansing();
};

class Triangle
{
	Point3 a; // ������� ������������
	Point3 b;
	Point3 c;
	Point3 o; // ����� ������������

	Point3 calculate_center(const Point3& a, const Point3& b, const Point3&c); // ���������� ����� ������������
public:
	// ������������
	Triangle(const Point3& a, const Point3& b, const Point3&c); // ����������� ������������ �� ���� ������
	Triangle(); // ��������������� "������������"(���������������) ������������

	// ������
	void createEmpty(); // ����� ��������������� "������������"(���������������) ������������
	bool isEmpty()const; // �������� �� ����������� �����������
	Point3 getCenter()const; // �������� ���������� ������ �����������

	// ������������� ���������
	bool operator<(const Triangle & v) const;
};

class TrianglePred // ���������� ��� ��������� �������������
{
	Vector cam;
public:
	TrianglePred(const Vector& v);
	bool operator()(const pair<int, Triangle>& t1, const pair<int, Triangle>& t2);
};

class TriangleInstancedPred // ���������� ��� ��������� ������������� �������������� �������
{
	Vector cam;
public:
	TriangleInstancedPred(const Vector& v);
	bool operator()(const pair<pair<int, int>, Triangle>& t1, const pair<pair<int, int>, Triangle>& t2);
};

class _model // ������� ����� - ������
{
	
protected:
	ID3D11Buffer* vertexBuffer; // ����� ������
	ID3D11Buffer* indexBuffer; // ����� ��������
	vector<WORD> index;  // ����� �������� � ���, ��� raypick

	size_t countVertex; // ���������� ������
	size_t countIndex; // ���������� ��������
	ID3D11Buffer* constantsBufferVS; // ����������� ����� ���������
	ID3D11Buffer* constantsBufferPS; // ����������� ����� ����������
	ID3D11InputLayout* vertexFormat; // ������ ������

	Matrix translation; // ������� ������� �������
	Matrix direction; // ������� ������� �������
	Matrix scale; // ������� ������� �������

	string versionShaderModel; // ������ ������ ��������
	wstring shader; // ��� ������� ��� ���� ������
	shaders shaderBuf; // ������ ��������

	// ������������
	UINT sizeByteTypeVertex; // ������ � ������ ����� �������

	// ���
	TypePrimitive type;
	
	// ����������� ������(������������� ����������, ����������� ������)
	void init_data(char* versionShaderModel); // ������������� ����������
	void move_data(_model&& m); // ����������� ������
	void clear_data(); // �������� ������

	_model(){} // ����������� - ����� ������ ���� ������� ��������� ������
public:
	// ������������
	_model(_model&& m);
	const _model& operator=(_model&& m);

	// ������
	static bool loadLineFromFile(string& line, std::ifstream& file); // �������� ������ �� �����
	static bool loadFileFromExpansion(const string& fileName, DataFromFile* data); // ����� ������� ������ �������� ������ �� ���������� �����
	static bool loadFromFileObj(const string& path, DataFromFile* data); // �������� *.obj - ������
	static bool loadFromFileProObject(const string& path, DataFromFile* data); // �������� *.pro_object - ������
	void setWorld(const CharactersModel& cm); // ������������� ������� �������
	void setType(const TypePrimitive& t); // ������������� ���
	TypePrimitive& getType(); // �������� ���

	// �����������
	virtual void draw(ID3D11DeviceContext * devcontx, D3D11_FILLMODE mode, Camera camera, const Matrix & Projection) = 0; // ����� ���������
	virtual void drawSelected(ID3D11DeviceContext * devcontx) = 0; // ����� ��������� bbox
	virtual vector<VertexColor> getBoundingBox() = 0; // �������� bbox ������

	// ����������
	virtual ~_model();
};

class ModelDraw : public _model // ������� ��� ������� �������
{
protected:
	BBox modelBBox; // �������������� ����� ������(������������ ������ ������, ��� ���������)

public:
	// ������������
	ModelDraw(); // �� - ���������
	ModelDraw(ModelDraw&& m); // �����������
	const ModelDraw& operator=(ModelDraw&& m); // ���������� �����������

	// ����������� ������
	virtual void draw(ID3D11DeviceContext * devcontx, D3D11_FILLMODE mode, Camera camera, const Matrix & Projection) {}; // ����� ���������
	virtual bool init(ID3D11Device* device, CollectionTextures& collTextures, CollectionShaders& collShaders) = 0; // ����� �������������
	virtual void drawSelected(ID3D11DeviceContext * devcontx) {}; // ��������� ���������� ������

	// ������
	vector<VertexColor> getBoundingBox();
	vector<Vertex> getBoundingBoxFrustumCulling();
	Vector getCenter();

	// ����������
	virtual ~ModelDraw() {}
};

class ModelDrawColor : public ModelDraw // ������ ��� �����, � �����
{
	vector<VertexColor> vertex; // ������ ������ � ���, ��� raypick
	ConstantBufferNoLight cb; // ����������� ����� �� GPU

	void initBoundingBox(const vector<VertexColor>& v); // �������� BBox
public:
	// ������������ ������
	void* operator new(size_t i); // ���������� ������
	void operator delete(void* p); // �������� ������ �� ������

	// ������������
	ModelDrawColor(char* versionShaderModel,  // �����������, ������ ��������
									const vector<VertexColor>& v,  // �������
									const vector<WORD>& ind,  // �������
									const wstring& s
									); // ������ ����� � ��������
	ModelDrawColor(char* versionShaderModel,  // �����������, ������ ��������
									const wstring& s // ��� �������
								    ); // ������� �������
	ModelDrawColor(ModelDrawColor&& m); // ����������� �����������
	const ModelDrawColor& operator=(ModelDrawColor&& m); // ����������� ���������� �����������

	// ���������� ������ ����������� �������
	void draw(ID3D11DeviceContext * devcontx, D3D11_FILLMODE mode, Camera camera, const Matrix & Projection);
	bool init(ID3D11Device* device, CollectionTextures& collTextures, CollectionShaders& collShaders);
	void drawSelected(ID3D11DeviceContext * devcontx);

	void setVertex(const vector< VertexColor>& v) { this->vertex = v; }
	void setIndex(const vector< WORD>& i) { this->index = i; }


	// ���������� ������
	std::pair<vector<VertexColor>, vector<WORD>> generateCube(const Color& c = Color(1.f, 1.f, 1.f, 1.f)); // ���������� ���

	// ����������
	virtual ~ModelDrawColor() {}
};

class ModelDrawLight : public ModelDraw // ������ ����������(���� ������ directional light)
{
	vector<VertexNormal> vertex; // ������ ������ � ���, ��� raypick
	ID3D11ShaderResourceView* textures; // ������ ������� ��� �������(���������� ������!!!)
	vector<wstring> listTextures; // ������ ����� � ���������
	ID3D11SamplerState* samplerState; // ������ �������
	ConstantBufferDirectionalLightPS psb; // ����������� ����� ��� ����������� �������(������ ����, ��� - �� �� ������� �������)
	ConstantBufferVS cb;

	void constructor(char * versionShaderModel, const wstring& s, BoundingBoxData* bbd = nullptr); // ����� ����� �������������
public:
	// ������������ ������
	void* operator new(size_t i); // ���������� ������
	void operator delete(void* p); // �������� ������ �� ������

	// ������������
	ModelDrawLight(char* versionShaderModel,  // �����������(�� ������ ������ � ��������), ������ ��������
								   const vector<VertexNormal>& v, // �������
								   const vector<WORD>& ind, // �������
							       const wstring& s, // ������ ������ ��������
								   const vector<wstring>& text, // ������ ����� � ���������
							   	   BoundingBoxData* bBoxData = nullptr // ������ � bbox'�
								   ); // ���������
	ModelDrawLight(ModelDrawLight&& m); // �����������
	const ModelDrawLight& operator=(ModelDrawLight&& m); // ���������� �����������

	// ���������� ������ ����������� �������
	void draw(ID3D11DeviceContext * devcontx, D3D11_FILLMODE mode, Camera camera, const Matrix & Projection);
	bool init(ID3D11Device* device, CollectionTextures& collTextures, CollectionShaders& collShaders);
	void drawSelected(ID3D11DeviceContext * devcontx);

	// ����������
	virtual ~ModelDrawLight();
};

class ModelInstansing : public _model // ������� ������ �����������
{
protected:
	vector<CharactersModel> characters; // ������ ������������� ������� ��������
	vector<bool> visible; // ������ ������, ���������� ����� ������ �����, � ����� ���
	BBoxInst modelBBox; // BBox 

	wstring shaderGeometry; // ��� ��������������� ������� ��� ���� ������
	wstring shaderPixel; // ��� ����������� ������� ��� ���� ������
	PhysicPositions physicPosition; // ���������� ����� ��� �������
	PhysicVisibles physicVisible; // ���������� ����� ��� ������

public:
	// ������������
	ModelInstansing(); // �� - ���������
	ModelInstansing(ModelInstansing&& m); // �����������
	const ModelInstansing& operator=(ModelInstansing&& m); // �������� �����������

	// ����������� ������
	virtual void reCreateStructuredBuffer(ID3D11Device* device) = 0; // ����������� ��� ����������� ������
	virtual void addModel(const CharactersModel& m) = 0; // ��������� �������
	virtual bool eraseModel(int index) = 0; // ������� ������� �� �������
	virtual void reVisible(int index) = 0; // �����/�������� ������
	virtual void show(int index) = 0; // �������� ������
	virtual void hide(int index) = 0; // ������ ������
	virtual bool isVisible(int index) = 0; // ����� �� ������� �� �������
	virtual void reVisibleBBox(int index) = 0; // �����/�������� ������
	virtual void showBBox(int index) = 0; // �������� ������
	virtual void hideBBox(int index) = 0; // ������ ������
	virtual bool isVisibleBBox(int index) = 0; // ����� �� bbox �������'� �� �������
	virtual CharactersModel getCharactersInstanse(int index) = 0; // �������� �������������� ��������� ��������
	virtual void draw(ID3D11DeviceContext * devcontx, D3D11_FILLMODE mode, Camera camera, const Matrix & Projection) {}; // ����� ���������
	virtual bool init(ID3D11Device* device, CollectionTextures& collTextures, CollectionShaders& collShaders) = 0; // ����� �������������
	virtual void drawSelected(ID3D11DeviceContext * devcontx) {}; // ��������� ���������� ������

	// ������
	template<class T>  void transformModel(vector<T>& v, const Matrix& m); // ��������� ���� ������ �� ������� ��������������
	vector<VertexColor> getBoundingBox();

	// ����������
	virtual ~ModelInstansing() {};
};

class ModelInstansingColor : public ModelInstansing
{
	vector<VertexColor> vertex; // �����, ������� ����� ������������ �� GPU

	ConstantBufferNoLightInstansing cb;

	void add_model(const CharactersModel& m); // ����� ����� ���������� ��������
public:
	// ������������
	ModelInstansingColor(char* versionShaderModel,  // �����������, ������ ��������
											 const vector<VertexColor>& v,  // �������
											 const vector<WORD>& ind,  // �������
											 const array<wstring, 3>& s, // ������ ����� � ��������
											 const CharactersModel& m // �������������� ������� ��������
											 ); // ���������
	ModelInstansingColor(ModelInstansingColor&& m); // ����������� �����������
	const ModelInstansingColor& operator=(ModelInstansingColor&& m); // ����������� ���������� �����������

	// ���������� ������ ����������� �������
	void reCreateStructuredBuffer(ID3D11Device* device);
	void addModel(const CharactersModel& m);
	bool eraseModel(int index);
	void reVisible(int index); 
	void show(int index); 
	void hide(int index);
	bool isVisible(int index);
	void reVisibleBBox(int index);
	void showBBox(int index);
	void hideBBox(int index);
	bool isVisibleBBox(int index);
	CharactersModel getCharactersInstanse(int index);
	void draw(ID3D11DeviceContext * devcontx, D3D11_FILLMODE mode, Camera camera, const Matrix & Projection);
	bool init(ID3D11Device* device, CollectionTextures& collTextures, CollectionShaders& collShaders);
	void drawSelected(ID3D11DeviceContext * devcontx);

	// ����������
	virtual ~ModelInstansingColor() {}
};

class ModelInstansingLight: public ModelInstansing
{
	vector<VertexNormal> vertex; // �����, ������� ����� ������������ �� GPU
	ID3D11ShaderResourceView* textures; // ������ ������� ��� �������(���������� ������!!!)
	vector<wstring> listTextures; // ������ ����� � ���������
	ID3D11SamplerState* samplerState; // ������ �������
	ConstantBufferDirectionalLightPS psb; // ����������� ����� ��� ����������� �������(������ ����, ��� - �� �� ������� �������)
	ConstantBufferVSInstansing cb;

	void add_model(const CharactersModel& m); // ����� ����� ���������� ��������
	void constructor(char * versionShaderModel, const vector<wstring>& s, const CharactersModel & m, BoundingBoxData* bbd = nullptr); // ������� �����������
public:
	// ������������
	ModelInstansingLight(char* versionShaderModel,  // �����������(�� ������ ������ � ��������), ������ ��������
											const vector<VertexNormal>& v,  // �������
											const vector<WORD>& ind,  // �������
											const vector<wstring>& s, // ������ ����� � ��������
											const vector<wstring>& text, // ������ ����� � ���������
											const CharactersModel& m, // ������� ������� ��������
											BoundingBoxData* bBoxData = nullptr // ������ � bbox'�
											); // ���������
	ModelInstansingLight(ModelInstansingLight&& m); // ����������� �����������
	const ModelInstansingLight& operator=(ModelInstansingLight&& m); // ����������� ���������� �����������

	// ���������� ������ ����������� �������
	void reCreateStructuredBuffer(ID3D11Device* device);
	void addModel(const CharactersModel& m);
	bool eraseModel(int index);
	void reVisible(int index);
	void show(int index);
	void hide(int index);
	bool isVisible(int index);
	void reVisibleBBox(int index);
	void showBBox(int index);
	void hideBBox(int index);
	bool isVisibleBBox(int index);
	CharactersModel getCharactersInstanse(int index);
	void draw(ID3D11DeviceContext * devcontx, D3D11_FILLMODE mode, Camera camera, const Matrix & Projection);
	bool init(ID3D11Device* device, CollectionTextures& collTextures, CollectionShaders& collShaders);
	void drawSelected(ID3D11DeviceContext * devcontx);

	// ����������
	virtual ~ModelInstansingLight();
};


// �������� ������� ��� ��������� raypick �� �������
// �������� ������ ��������
// �������� ����:
// ��� - ������, �������, ����������, ��������, �����������
// ������ - ���� �����.
// � ������ ��������: ���� ����� + ��������
// ����������� ���������� �������� - ���������
// �����������: �������� Frustum Culling ��� ���� ����� �������(�� ��������)
// ModelDrawLight: � init ���������� ����, � draw - ������������� ���� ����� �����
// �� ������ ��� ��������, ���� ����� ������ ���������
// �������� ����� ��������� �����, � ���������(��������� ��� ����������� �����)


// ��������� ������

// BoundingBoxInstansing
template<class T>
inline void BoundingBoxInstansing::transformModel(vector<T>& v, const Matrix & m)
{
	Matrix transform = Matrix::CreateTranslationMatrixXYZ(-center[x], -center[y], -center[z]) * m * Matrix::CreateTranslationMatrixXYZ(center[x], center[y], center[z]);
	for (auto& vert : v)
	{
		Vector3 vertex = (Vector3)vert.Pos;
		vertex.vector3TransformCoord(transform);
		vert.Pos = vertex;
	}
}

// ModelInstansing
template<class T>
inline void ModelInstansing::transformModel(vector<T>&  v, const Matrix & m)
{
	auto center = modelBBox->getCenterBoundingBox(-1);
	Matrix transform = Matrix::CreateTranslationMatrixXYZ(-center[x], -center[y], -center[z]) * m * Matrix::CreateTranslationMatrixXYZ(center[x], center[y], center[z]);
	for (auto& vert: v)
	{
		Vector3 vertex = (Vector3)vert.Pos;
		vertex.vector3TransformCoord(transform);
		vert.Pos = vertex;
	}
}



