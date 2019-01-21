#include "Calculate.h"

std::pair<Point, Point> Computer::calculateRay(int _x, int _y, const D3D11_VIEWPORT& vp, const Matrix& matrixProjection, const Camera& camera)
{
	Vector vectorLine = matrixProjection[0];
	float px = (((2.f * _x) / vp.Width) - 1.f) / vectorLine[x];
	vectorLine = matrixProjection[1];
	float py = (((-2.f*_y) / vp.Height) + 1.f) / vectorLine[y];

	Vector rayOr(0.f, 0.f, 0.f, 0.f);
	Vector rayDirection(px, py, 1.f, 0.f);

	Matrix notview(Matrix::Inverse(camera.getView()));
	rayOr.vector4TransformCoord(notview);
	rayDirection.vector4TransformNormal(notview);
	rayDirection.normalize();

	return { (Point)rayOr, (Point)rayDirection };
}

float Computer::intersectionRayAndPlane(const Point & point, const Point & normal, const std::pair<Point, Point>& ray)
{
	return -Point::dot(ray.first - point, normal) / Point::dot(ray.second, normal);
}

Point Computer::getPointIntersectionRayAndPlane(const std::pair<Point, Point>& ray, float t)
{
	return ray.first + (Point)(t * ray.second); // получаем точку пересечения
}

bool Computer::belongsPointInTriangle(const Point & a, const Point & b, const Point & c, const Point & m, const Point & n)
{
	Vector p(n);
	Vector p0(a);
	Vector p1(b);
	Vector p2(c);

	Vector o(m); // проверяемая точка
	Vector tmp;

	// первые два вектора
	Vector oa(p0 - o);
	Vector ob(p1 - o);
	tmp = p * Vector4::cross(oa, ob);
	float v1(tmp[x]);

	// вторые два вектора
	Vector oc(p2 - o);
	tmp = p * Vector4::cross(ob, oc);
	float v2(tmp[x]);

	// третьи два вектора
	tmp = p * Vector4::cross(oc, oa);
	float v3(tmp[x]);

	// теперь проверяем знаки
	if ((v1 < 0 && v2 < 0 && v3 < 0) || (v1 >= 0 && v2 >= 0 && v3 >= 0))
		return true;
	return false;
}

Point Computer::generateNormal(const Point & a, const Point & b, const Point & c)
{
	Vector p0(a);
	Vector p1(b);
	Vector p2(c);
	Vector u = p1 - p0;
	Vector v = p2 - p0;
	Vector p = Vector::cross(u, v);
	p = Vector::Normalize(p);
	return (Point)p;
}

