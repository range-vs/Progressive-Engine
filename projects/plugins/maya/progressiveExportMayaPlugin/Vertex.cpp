#include "Vertex.h"

bool Vector3::operator<(const Vector3 & v) const
{
	if (this->x == v.x) // abs(this->x - v.x) < 0.01f
	{
		if (this->y == v.y) // abs(this->y - v.y) < 0.01f
		{
			if (this->z >= v.z)
				return false;
			else
				return true;
		}
		else if (this->y > v.y)
			return false;
		else
			return true;
	}
	else if (this->x > v.x)
		return false;
	else
		return true;
}

Vector3 & Vector3::operator+=(const Vector3 & v)
{
	this->x += v.x;
	this->y += v.y;
	this->z += v.z;
	return *this;
}

Vector3 Vector3::operator*(const int & n)
{
	Vector3 v; v.x = x; v.y = y; v.z = z;
	v.x *= n;
	v.y *= n;
	v.z *= n;
	return v;
};

////////////////////////////////////////////////////////////

Vertex::Vertex()
{
}

Vertex::Vertex(const Vector3 & p, const Vector3 & uv, const Vector3 & n):pos(p), uv(uv), normal(n)
{
}


Vertex::~Vertex()
{
}

/////////////////////////////////////////////////////////////

bool VertexComparator::operator()(const  Vertex& v1, const Vertex& v2)const
{
	// —равнить все компоненты(позици€, текстрный координаты и нормаль)
	// TODO: протестировать данный метод

	if (v1.pos < v2.pos)
		return true;
	else
	{
		if (v1.uv < v2.uv)
			return true;
		/*else
		{
			if (v1.normal < v2.normal)
				return true;
		}*/
	}
	return false;
}

//////////////////////////////////////////////////////////////
bool MaterialComparator::operator()(const pair<Material, int>& m1, const pair<Material, int>& m2) const
{
	/*if (m1.first.nameMaterial < m2.first.nameMaterial)
		return true;
	else
	{
		*/if (m1.first.nameTexture < m2.first.nameTexture)
			return true;
	//}
	return false;
}
