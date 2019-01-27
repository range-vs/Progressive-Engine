#pragma once

#include "buffers.h"
#include "util.h"

// ����������
using Func = IDXGIAdapter1*(const string&);
using SetNameModule = void*(const char*);
using HZMonitor = bool*(int*, int*);
using Transform = function<Matrix(Matrix)>;
using IVNBuffers = std::pair<std::vector<VertexNormal>, std::vector<WORD>>;
using IVCBuffers = std::pair<std::vector<VertexColor>, std::vector<WORD>>;
template <class T> using IVBuffers = std::pair<std::vector<T>, std::vector<WORD>>;
using _2DTextureBuffers = std::vector<std::pair<ID3D11ShaderResourceView*, int>>;
using DVertexNormal = brain_memory<VertexNormal>;
using DVertexColor = brain_memory<VertexColor>;

// ��������� windows
#define WM_ADDMODELMOUSE WM_USER + 100
#define WM_RAYPICK WM_USER + 101
#define WM_RENDERE WM_USER + 103
#define WM_RENDERG WM_USER + 104

// ���� ������
struct BoundingBoxData // ������ ��� �������� BBox
{
	float xl, yd, zf; // ����, ��� � �����
	float xr, yu, zr; // �����, ���� � ���
	Vector center; // ����� ������

	BoundingBoxData(float xl, float yd, float zf, float xr, float yu, float zr) :xl(xl), yd(yd), zf(zf), xr(xr), yu(yu), zr(zr) {}
	BoundingBoxData() :xl(0), yd(0), zf(0), xr(0), yu(0), zr(0) {}
};

struct DataFromFile // ���������, ������������ ��� �������� ����� �� obj
{
	std::vector<wstring> texturesListOut; // �������� ������ ������� ��� ������
	std::vector< VertexNormal> vertex; // ������ ������
	size_t vertexCount; // ���������� ������
	std::vector<WORD> index; // ������ ��������
	size_t indexCount; // ���������� ��������
	std::vector<wstring> shaders;
	BoundingBoxData bboxData; // ������� ����� BBox
	bool isBBox; // ���� �� ������ ������
	bool isInstansing; // �������������� �� ������

	DataFromFile() :vertexCount(0), isBBox(false){}
	DataFromFile(const std::vector<wstring>& tLO, const std::vector< VertexNormal>& v, size_t vC, const std::vector<WORD>& i, size_t iC, const std::vector<wstring>& shaders = std::vector<wstring>(), const BoundingBoxData& bbd = BoundingBoxData()) :
		texturesListOut(tLO), vertex(v), vertexCount(vC), index(i), indexCount(iC), shaders(shaders), bboxData(bbd), isInstansing(false)
	{
		if ((bboxData.xl + bboxData.xr + bboxData.yd + bboxData.yu + bboxData.zf + bboxData.zr) < 0.001f)
			isBBox = false;
		isBBox = true;
	}
};

struct DateTime // ����� � ����
{
	int seconds;
	int minutes;
	int hours;
	int days;
	int mounts;
	int years;

	DateTime(int s, int m, int h, int d, int mo, int y) :seconds(s), minutes(m), hours(h), days(d), mounts(mo), years(1900 + y) {}
	DateTime():seconds(0), minutes(0), hours(0), days(0), mounts(0), years(1900) {}
};

struct CharactersModel // �������������� ������, ������������� �� �����
{
	bool isVisible;
	bool isSelectVisible;
	char* nameModel;
	Matrix position;
	Matrix direction;
	Matrix scale;
	Color color;
	D3D_PRIMITIVE_TOPOLOGY top;

	CharactersModel():isVisible(true), isSelectVisible(false), nameModel(nullptr), position(Matrix::Identity()), direction(Matrix::Identity()), scale(Matrix::Identity()), color(1.f, 1.f, 1.f, 1.f),
	top(D3D_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST){}
};

struct AllTransformations // ������ ��� ������������ ������
{
	Matrix position;
	Matrix rotation;
	Matrix scale;

	AllTransformations() {}
	AllTransformations(const Matrix& p, const Matrix& r, const Matrix& s) :position(p), rotation(r), scale(s) {}
};

struct DataNewModel // ��������� ������ ����� ��������������� ������ �� �����
{
	char* path;
	int x;
	int y;
};

struct Coordinate2D
{
	int x;
	int y;
	Coordinate2D(int x, int y):x(x), y(y){}
};