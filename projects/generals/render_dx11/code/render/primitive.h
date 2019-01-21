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
	static void generateIndexPlane(vector<WORD>& ind, int c); // генерируем индексы для будущей плоскости
	static void generateIndexSphere(vector<WORD>& ind, int count_usech, int count_h); // генерируем индексы для будущей сферы
public:
	// planes
	static void generatePlane(vector<VertexColor>& v, vector<WORD>& ind, float w, int c, Color cl); // создаём плоскость в цвете
	static void generatePlane(vector<VertexNormal>& v, vector<WORD>& ind, float w, int c); // создаём плоскость с текстурой
	static void generatePlaneNet(vector<VertexColor>& v, vector<WORD>& ind, float w, int c); // создаем плоскость - сетку(для редактора)
	// spheres
	static void generateSphere(vector<VertexColor>& v, vector<WORD>& ind, float r, int count_usech, int count_h, Color cl); // создаём сферу в цвете
	static void generateSphere(vector<VertexNormal>& v, vector<WORD>& ind, float r, int count_usech, int count_h); // создаём сферу с текстурой
	static void generateSphereNet(vector<VertexColor>& v, vector<WORD>& ind, float r, int count_usech, int count_h); // создаем сферу - сетку(для редактора)
	// cubes
	static void generateCube(vector<VertexColor>& v, vector<WORD>& ind, float size, Color cl); // создаём куб в цвете
	static void generateCube(vector<VertexNormal>& v, vector<WORD>& ind, float size); // создаём куб с текстурой
	static void generateCubeNet(vector<VertexColor>& v, vector<WORD>& ind, float size); // создаём куб - сетку(для редактора)
};
