#pragma once

#include "../render/camera.h"
#include <vector>

using namespace gmath;
using namespace gmtypes;

class Computer
{
	template<typename T> static T _genereateAverageNormal(T p);
	template<typename T, typename... Vertexs> static T _genereateAverageNormal(T sum, Vertexs&... vert);
public:
	static std::pair<Point, Point> calculateRay(int _x, int _y, const D3D11_VIEWPORT& vp, const Matrix& matrixProjection, const Camera& camera); // генерируем луч от мышки по направлению камеры
	static float intersectionRayAndPlane(const Point & point, const Point & normal, const std::pair<Point, Point>& ray); // пересекает ли луч плоскость?
	static Point getPointIntersectionRayAndPlane(const std::pair<Point, Point>& ray, float t); // получаем точку пересечения луча и плоскости
	static bool belongsPointInTriangle(const Point & a, const Point & b, const Point & c, const Point & m, const Point & n); // лежит ли точка в треугольнике
	static Point generateNormal(const Point & a, const Point & b, const Point & c); // генерирование усреднённой нормали
	template<class T> static void transformModel(std::vector<T>&  v, const Matrix & m); // трансформирование модели
	template<typename T, typename... Vertexs> static T genereateAverageNormal(int c, T sum, Vertexs&... vert);
};

template<class T>
inline void Computer::transformModel(std::vector<T>&  v, const Matrix & m)
{
	auto center = modelBBox->getCenterBoundingBox(-1);
	Matrix transform = Matrix::CreateTranslationMatrixXYZ(-center[x], -center[y], -center[z]) * m * Matrix::CreateTranslationMatrixXYZ(center[x], center[y], center[z]);
	for (auto& vert : v)
	{
		Vector3 vertex = (Vector3)vert.Pos;
		vertex.vector3TransformCoord(transform);
		vert.Pos = vertex;
	}
}

template<typename T>
T Computer::_genereateAverageNormal(T p)
{
	return p;
}

template<typename T, typename... Vertexs>
T Computer::_genereateAverageNormal(T sum, Vertexs&... vert)
{
	return sum += _genereateAverageNormal(vert...);
}

template<typename T, typename ...Vertexs>
T Computer::genereateAverageNormal(int c, T sum, Vertexs & ...vert)
{
	Point s = _genereateAverageNormal(sum, vert...);
	s *= 1.f / c;
	return s;
}