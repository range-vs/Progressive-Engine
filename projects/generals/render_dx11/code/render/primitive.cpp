#include "primitive.h"

void Primitive::generateIndexPlane(vector<WORD>& ind, int c)
{
	int counter(0);
	int count(c + 1);
	for (int i(0); i < count - 1; i++)
	{
		for (int j(0); j < count - 1; j++)
		{
			ind.push_back(counter);
			ind.push_back(counter + 1);
			ind.push_back((i + 1) * count + j);
			ind.push_back((i + 1) * count + j);
			ind.push_back(counter + 1);
			ind.push_back((i + 1) * count + (j + 1));
			counter++;
		}
		counter++;
	}
}

void Primitive::generatePlane(vector<VertexColor>& v, vector<WORD>& ind, float w, int c, Color cl)
{
	w = abs(w);
	c = abs(c);
	float z(-w);
	float x(w);
	float y(0.f);
	float z_end(w);
	float x_end(-w);
	float h((z_end - z) / c);

	for (; z_end >= z; z_end -= h)
		for (float _x(x_end); _x <= x; _x += h)
			v.push_back({ Point(_x, y, z_end), cl });

	generateIndexPlane(ind, c);
}

void Primitive::generatePlane(vector<VertexNormal>& v, vector<WORD>& ind, float w, int c)
{
	w = abs(w);
	c = abs(c);
	float z(-w);
	float x(w);
	float y(0.f);
	float z_end(w);
	float x_end(-w);
	float h((z_end - z) / c);
	float tex(1.f / c);

	for (float u(0); z_end >= z; z_end -= h, u += tex)
		for (float _x(x_end), _v(0); _x <= x; _x += h, _v += tex)
			v.push_back({ Point(_x, y, z_end), Point(u, _v, 0), Point(0,y + 1,0) });

	generateIndexPlane(ind, c);
}

void Primitive::generatePlaneNet(vector<VertexColor>& v, vector<WORD>& ind, float w, int c)
{
	w = abs(w);
	c = abs(c);
	float z(w);
	float x(w);
	float y(0);
	float z_end(-w);
	float x_end(-w);
	float h((x - x_end) / c);

	for (; z_end <= z; z_end += h)
		for (float _x(x_end); _x <= x; _x += h)
			v.push_back({ _x, y, z_end });

	int f(0);
	int s(c + 1);
	for (int i(0); i < c; i++)
	{
		for (int j(0); j < c; j++)
		{
			ind.push_back(f);
			ind.push_back(f + 1);
			ind.push_back(s + 1);
			ind.push_back(s);
			ind.push_back(f);
			f++; s++;
		}
		ind.push_back(s - 1);
		ind.push_back(s - c);
		f++; s++;
	}
}

void Primitive::generateSphere(vector<VertexColor>& v, vector<WORD>& ind, float r, int count_usech, int count_h, Color cl)
{
	if (r < 3) r = 3;
	if (count_usech < 3) count_usech = 3;
	if (count_h < 3) count_h = 3;
	count_h++;

	int latitudeBands = count_usech; // количество широтных полос
	int longitudeBands = count_h; // количество полос долготы
	float radius = r; // радиус сферы

	for (float latNumber = 0; latNumber <= latitudeBands; latNumber++)
	{
		float theta = latNumber * GeneralMath::PI / latitudeBands;
		float sinTheta = sin(theta);
		float cosTheta = cos(theta);

		for (float longNumber = 0; longNumber <= longitudeBands; longNumber++)
		{
			float phi = longNumber * 2 * GeneralMath::PI / longitudeBands;
			float sinPhi = sin(phi);
			float cosPhi = cos(phi);

			float xn = cosPhi * sinTheta;   // x
			float yn = cosTheta;            // y
			float zn= sinPhi * sinTheta;   // z

			v.push_back({ Point(r * xn, r * yn, r * zn), cl });
		}
	}
	generateIndexSphere(ind, latitudeBands, longitudeBands);
}

void Primitive::generateSphere(vector<VertexNormal>& v, vector<WORD>& ind, float r, int count_usech, int count_h)
{
	if (r < 3) r = 3;
	if (count_usech < 3) count_usech = 3;
	if (count_h < 3) count_h = 3;
	count_h++;

	int latitudeBands = count_usech; // количество широтных полос
	int longitudeBands = count_h; // количество полос долготы
	float radius = r; // радиус сферы

	for (float latNumber = 0; latNumber <= latitudeBands; latNumber++)
	{
		float theta = latNumber * GeneralMath::PI / latitudeBands;
		float sinTheta = sin(theta);
		float cosTheta = cos(theta);

		for (float longNumber = 0; longNumber <= longitudeBands; longNumber++)
		{
			float phi = longNumber * 2 * GeneralMath::PI / longitudeBands;
			float sinPhi = sin(phi);
			float cosPhi = cos(phi);

			float xn = cosPhi * sinTheta;   // x
			float yn = cosTheta;            // y
			float zn = sinPhi * sinTheta;   // z
			float u = 1 - (longNumber / longitudeBands); // u
			float _v = 1 - (latNumber / latitudeBands);   // v

			v.push_back({ Point(r * xn, r * yn, r * zn), Point(xn, yn, zn), Point(u, _v, 0) });
		}
	}
	generateIndexSphere(ind, latitudeBands, longitudeBands);
}

void Primitive::generateSphereNet(vector<VertexColor>& v, vector<WORD>& ind, float r, int count_usech, int count_h)
{
	if (r < 3) r = 3;
	if (count_usech < 3) count_usech = 3;
	if (count_h < 3) count_h = 3;
	count_h++;

	int latitudeBands = count_usech; // количество широтных полос
	int longitudeBands = count_h; // количество полос долготы
	float radius = r; // радиус сферы

	for (float latNumber = 0; latNumber <= latitudeBands; latNumber++)
	{
		float theta = latNumber * GeneralMath::PI / latitudeBands;
		float sinTheta = sin(theta);
		float cosTheta = cos(theta);

		for (float longNumber = 0; longNumber <= longitudeBands; longNumber++)
		{
			float phi = longNumber * 2 * GeneralMath::PI / longitudeBands;
			float sinPhi = sin(phi);
			float cosPhi = cos(phi);

			float xn = cosPhi * sinTheta;   // x
			float yn = cosTheta;            // y
			float zn = sinPhi * sinTheta;   // z

			v.push_back({ Point(r * xn, r * yn, r * zn), Colors::white });

		}
	}
	for (int latNumber(0); latNumber < latitudeBands; latNumber++)
	{
		for (int longNumber(0); longNumber < longitudeBands; longNumber++)
		{
			int first = (latNumber * (longitudeBands + 1)) + longNumber;
			int second = first + longitudeBands + 1;
			ind.push_back(first);
			ind.push_back(second);
			ind.push_back(second + 1);
			ind.push_back(first + 1);
		}
	}
}

void Primitive::generateIndexSphere(vector<WORD>& ind, int latitudeBands, int longitudeBands)
{
	for (int latNumber(0); latNumber < latitudeBands; latNumber++)
	{
		for (int longNumber(0); longNumber < longitudeBands; longNumber++)
		{
			int first = (latNumber * (longitudeBands + 1)) + longNumber;
			int second = first + longitudeBands + 1;
			ind.push_back(first + 1);
			ind.push_back(second);
			ind.push_back(first);
			ind.push_back(first + 1);
			ind.push_back(second + 1);
			ind.push_back(second);
			if (latNumber == 1)
				return;
		}
	}
}

void Primitive::generateCube(vector<VertexColor>& v, vector<WORD>& ind, float size, Color c)
{
	float value = fabs(size);
	v = {
	{ Point(-value, value, -value), c },
	{ Point(value, value, -value), c   },
	{ Point(value, value, value), c  },
	{ Point(-value, value, value), c   },
	{ Point(-value, -value, -value), c   },
	{ Point(value, -value, -value), c },
	{ Point(value, -value, value), c},
	{ Point(-value, -value, value), c  }
	};

	ind = {
		3,1,0,
		2,1,3,
		0,5,4,
		1,5,0,
		3,4,7,
		0,4,3,
		1,6,5,
		2,6,1,
		2,7,6,
		3,7,2,
		6,4,5,
		7,4,6,
	};
}

void Primitive::generateCube(vector<VertexNormal>& v, vector<WORD>& ind, float size)
{
	// generate all vertex's
	float value = fabs(size);
	vector<Point> _v = {
	 Point(-value, value, -value), // top ( против часовой, с самой нижней левой)
	 Point(value, value, -value) ,
	 Point(value, value, value),
	 Point(-value, value, value),
	 Point(-value, -value, -value), // bottom ( против часовой, с самой нижней левой)
	 Point(value, -value, -value),
	 Point(value, -value, value),
	 Point(-value, -value, value)
	};
	// generate all normal's
	vector<Point> norm = {
		Computer::generateNormal(_v[0], _v[1], _v[4]), // near
		Computer::generateNormal( _v[5], _v[1], _v[2]), // right
		Computer::generateNormal(_v[6], _v[2], _v[3]), // far
		Computer::generateNormal(_v[7], _v[3], _v[0]), // left
		Computer::generateNormal(_v[0], _v[3], _v[2]), // top
		Computer::generateNormal(_v[7], _v[4], _v[5]), // bottom
	};
	// generate real normal's
	vector<Point> real_norm = {
		Computer::genereateAverageNormal(3, norm[0], norm[3], norm[4]),  // top ( против часовой, с самой нижней левой)
		Computer::genereateAverageNormal(3, norm[0], norm[1], norm[4]),
		Computer::genereateAverageNormal(3, norm[2], norm[1], norm[4]),
		Computer::genereateAverageNormal(3, norm[2], norm[3], norm[4]),
		Computer::genereateAverageNormal(3, norm[0], norm[3], norm[5]),  // bottom ( против часовой, с самой нижней левой)
		Computer::genereateAverageNormal(3, norm[0], norm[1], norm[5]),
		Computer::genereateAverageNormal(3, norm[2], norm[1], norm[5]),
		Computer::genereateAverageNormal(3, norm[2], norm[3], norm[5]),
	};
	// generate final vertex + uv coordinate
	v = {
		{_v[0], {0.35f, 1.f, 0}, real_norm[0]},
		{_v[1], {0.63f, 1.f, 0}, real_norm[1]},
		{_v[2], {0.63f, 0.75f, 0}, real_norm[2]},
		{_v[3], {0.35f, 0.75f, 0}, real_norm[3]},
		{_v[7], {0.12f, 0.75f, 0}, real_norm[7]},
		{_v[4], {0.12f, 1.f, 0}, real_norm[4]},
		{_v[5], {0.85f, 1.f, 0}, real_norm[5] },
		{_v[6], {0.85f, 0.75f, 0}, real_norm[6]},
		{_v[7], {0.35f, 0.5f, 0}, real_norm[7]}, // 8
		{_v[6], {0.63f, 0.5f, 0}, real_norm[6]}, // 9 
		{_v[4], {0.35f, 0.25f, 0}, real_norm[4]}, // 10
		{_v[5], {0.63f, 0.25f, 0}, real_norm[5]},
		{_v[0], {0.35f, 0.f, 0}, real_norm[0]},
		{_v[1], {0.63f, 0.f, 0}, real_norm[1]}
	};
	// generate index
	ind = {
		10,12,11,
		11,12,13,
		8,10,9,
		9,10,11,
		3,8,2,
		2,8,9,
		5,4,0,
		0,4,3,
		0,3,1,
		1,3,2,
		1,2,6,
		6,2,7
	};
}

void Primitive::generateCubeNet(vector<VertexColor>& v, vector<WORD>& ind, float size)
{
	float value = fabs(size);
	v = {
		{ -value, value, -value },
		{ value, value, -value  },
		{ value, value, value  },
		{ -value, value, value  },
		{ -value, -value, -value   },
		{ value, -value, -value},
		{ value, -value, value},
		{ -value, -value, value  }
	};
	ind = {
		0, 1,2,3, 0, // верхний квардрат и черта вниз
		4,5,6,7,4, // нижний квадрат
		5,1,5, // против часовой, черта вверх первая
		6,2,6, // вторая
		7,3,7 // третья
	};

}