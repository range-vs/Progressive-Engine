#pragma once

#include <vector>
#include <windows.h>

#include "../utilites/buffers.h"
#include "../utilites/calculate.h"

using namespace std;

enum class TypePrimitive: int
{
	NoPrimitive = -1,
	Plane = 0,
	Cube,
	Sphere,
	Frustum
};

class Primitive
{
	static void generateIndexPlane(vector<WORD>& ind, int c); // ���������� ������� ��� ������� ���������
	static void generateIndexSphere(vector<WORD>& ind, int count_usech, int count_h); // ���������� ������� ��� ������� �����
public:
	// planes
	static void generatePlane(vector<VertexColor>& v, vector<WORD>& ind, float w, int c, Color cl); // ������ ��������� � �����
	static void generatePlane(vector<VertexNormal>& v, vector<WORD>& ind, float w, int c); // ������ ��������� � ���������
	static void generatePlaneNet(vector<VertexColor>& v, vector<WORD>& ind, float w, int c); // ������� ��������� - �����(��� ���������)
	// spheres
	static void generateSphere(vector<VertexColor>& v, vector<WORD>& ind, float r, int count_usech, int count_h, Color cl); // ������ ����� � �����
	static void generateSphere(vector<VertexNormal>& v, vector<WORD>& ind, float r, int count_usech, int count_h); // ������ ����� � ���������
	static void generateSphereNet(vector<VertexColor>& v, vector<WORD>& ind, float r, int count_usech, int count_h); // ������� ����� - �����(��� ���������)
	// cubes
	static void generateCube(vector<VertexColor>& v, vector<WORD>& ind, float size, Color cl); // ������ ��� � �����
	static void generateCube(vector<VertexNormal>& v, vector<WORD>& ind, float size); // ������ ��� � ���������
	static void generateCubeNet(vector<VertexColor>& v, vector<WORD>& ind, float size); // ������ ��� - �����(��� ���������)
};
