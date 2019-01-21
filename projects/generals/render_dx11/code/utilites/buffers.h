#pragma once

#include <vector>
#include <string>
#include <windows.h>
#include <mathematics.h>
#include <d3d11.h> 
#include <d3d10.h>
#include <D3Dcompiler.h>
#include <functional>
#include <map>
#include <filesystem>

using namespace gmath;
using namespace gmtypes;
namespace fs = std::experimental::filesystem;

using std::wstring;
using std::string;
using std::function;
using std::map;

struct Vertex
{
	Point Pos;

	Vertex() :Pos() {  }
	Vertex(const Point& p) :Pos(p) {  }
};

struct VertexColor : public Vertex
{
	Color color;

	VertexColor() :Vertex(), color() {  }
	VertexColor(float x, float y, float z):Vertex(Point(x,y,z)), color(Colors::white){  }
	VertexColor(const Point& p, const Color& c) : Vertex(p), color(c) {  }	
	/*VertexColor CreateVertex(double cosPhi, double sinTheta, double cosTheta, double sinPhi, double radius);
	bool operator==(const VertexColor& v)const;*/
};

struct VertexNormal : public Vertex
{
	Point Texture;
	Point Normal;

	VertexNormal() :Vertex(), Texture(), Normal() {  }
	VertexNormal(float x, float y, float z) :Vertex(Point(x, y, z)), Texture(), Normal() {  }
	VertexNormal(const Point& p, const Point& t, const Point& n) : Vertex(p), Texture(t), Normal(n) {  }
};

struct alignas(16) ConstantBufferNoLight
{
	Matrix Translation;
	Matrix Rotation;
	Matrix Scale;
	Matrix View;
	Matrix Projection;
	Vector center; // центр модели

	ConstantBufferNoLight(const Matrix& w, const Matrix& w1, const Matrix& w2, const Matrix& v, const Matrix& p, const Vector& po):
		Translation(w), Rotation(w1), Scale(w2), View(v), Projection(p), center(po){}
	ConstantBufferNoLight(){}
};

struct alignas(16) ConstantBufferVS
{
	Matrix Translation;
	Matrix Rotation;
	Matrix Scale;
	Matrix View;
	Matrix Projection;
	Vector center; // центр модели
	Point4 CamPos;


	operator ConstantBufferNoLight();
};

struct alignas(16) ConstantBufferNoLightInstansing
{
	Matrix View;
	Matrix Projection;
	Vector center; // центр модели

	ConstantBufferNoLightInstansing(const Matrix& v, const Matrix& p, const Vector& po) :
		View(v), Projection(p), center(po) {}
	ConstantBufferNoLightInstansing() {}
};

struct alignas(16) ConstantBufferVSInstansing
{
	Matrix View;
	Matrix Projection;
	Vector center; // центр модели
	Point4 CamPos;


	operator ConstantBufferNoLightInstansing();
};


struct alignas(16) ConstantBufferModePS
{
	ConstantBufferModePS() :mode() {};
	int mode;
};

struct alignas(16) ConstantBufferDirectionalLightPS
{
	ConstantBufferDirectionalLightPS() :Direction(), Color(), factors() {  };
	Point4 Direction;
	Point4 Color;
	Point4 factors; // коэффициент фонового освещения(x), и блика(y)
	Vector View; // вектор вида(позиция камеры)
};


struct shaders
{
	ID3D11VertexShader* vertexShader;
	ID3D11PixelShader* pixelShader;
	ID3D11GeometryShader* geometryShader;
};

enum D3D11_FILLMODE // расширенное перечисление режимов отрисовки
{
	D3D11_FILLMODE_WIREFRAME = 2,
	D3D11_FILLMODE_SOLID = 3,
	D3D11_FILLMODE_MIXED = 4
};

