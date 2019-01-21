#pragma once

#include <string>

using namespace std;

struct DateTime
{
	int seconds;
	int minutes;
	int hours;
	int days;
	int mounts;
	int years;

	DateTime(int s, int m, int h, int d, int mo, int y) :seconds(s), minutes(m), hours(h), days(d), mounts(mo), years(1900 + y) {}
};

struct Vector3
{
	float x;
	float y;
	float z;

	Vector3():x(0), y(0), z(0){}
	Vector3(float x, float y, float z):x(x), y(y), z(z){}
	bool operator<(const Vector3& v)const;
	Vector3& operator+=(const Vector3& v);
	Vector3 operator*(const int& n);
};

struct Vertex
{
	Vector3 pos;
	Vector3 uv;
	Vector3 normal;

	Vertex();
	Vertex(const Vector3& p, const Vector3& uv, const Vector3& n);

	~Vertex();
};

class VertexComparator
{
public:
	bool operator()(const Vertex& v1, const Vertex& v2)const;
};

struct Material
{
	string nameMaterial;
	string nameTexture;
};

class MaterialComparator
{
public:
	bool operator()(const pair<Material, int>& m1, const pair<Material, int>& m2)const;

};

struct Trio
{
	void* first;
	int second;
	bool third;

	Trio(void* f, const int&s, const bool& t = true):first(f), second(s), third(t){}
};

struct Normal
{
	Vector3 normal;
	unsigned int count;

	Normal():normal(), count(0){}
	Normal(const Vector3& v, unsigned int c):normal(v), count(c){}
};