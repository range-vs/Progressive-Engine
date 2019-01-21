#include "Model.h"

// класс ограничивающий бокс
BoundingBox::BoundingBox(float xl, float yd, float zf, float xr, float yu, float zr): visible(false)
{
	LastPoints lp = { xl, yd, zf, xr, yu, zr };
	this->shiftCoordinateProcent(lp, 5);
	center = this->calculateCenterBoundingBox(lp);
	arrayPointsBox = this->generateVertexForRayPick(lp);
	arrayPointsFrustum = this->generateVertexForFrustumCulling(lp);
	arrayForDraw = this->generateVertexForDraw(lp);
}

BoundingBox::BoundingBox(float xl, float yd, float zf, float xr, float yu, float zr, const Vector & c)
{
	LastPoints lp = { xl, yd, zf, xr, yu, zr };
	this->shiftCoordinateProcent(lp, 5);
	center = c;
	arrayPointsBox = this->generateVertexForRayPick(lp);
	arrayPointsFrustum = this->generateVertexForFrustumCulling(lp);
	arrayForDraw = this->generateVertexForDraw(lp);
}

BoundingBox::BoundingBox(BoundingBox && bb)
{
	this->move_data(std::move(bb));
}

const BoundingBox & BoundingBox::operator=(BoundingBox && bb)
{
	if (this != &bb)
		this->move_data(std::move(bb));
	return *this;
}

bool BoundingBox::init(ID3D11Device * device, CollectionShaders & collShaders, const wstring & shader)
{
	// создаем вершинный буфер
	vertexShader = collShaders.getShadersVertex(shader); 
	pixelShader = collShaders.getShadersPixel(shader); 
	return _init(device, collShaders, shader);
}

void BoundingBox::draw(ID3D11DeviceContext * devcontx, const ConstantBufferNoLight & buff)
{
	UINT start = 0;
	UINT sizeVertex = sizeof VertexColor;
	devcontx->IASetVertexBuffers(NULL, 1, &vertexBuffer, &sizeVertex, &start);
	devcontx->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP);

	devcontx->IASetInputLayout(vertexFormat);
	devcontx->VSSetShader(vertexShader, NULL, NULL);
	devcontx->GSSetShader(NULL, NULL, NULL); // установка геометрического шейдера
	devcontx->PSSetShader(pixelShader, NULL, NULL);

	devcontx->UpdateSubresource(constantsBufferVS, 0, NULL, &buff, 0, 0);
	devcontx->VSSetConstantBuffers(0, 1, &constantsBufferVS);

	ConstantBufferModePS cbps;
	cbps.mode = 1;
	devcontx->UpdateSubresource(constantsBufferPS, 0, NULL, &cbps, 0, 0);
	devcontx->PSSetConstantBuffers(0, 1, &constantsBufferPS);

	devcontx->Draw((UINT)arrayForDraw.size(), 0);
}

void BoundingBox::shiftCoordinate(LastPoints& lp, float c) // сдвиг всех координат
{
	if (c <= 0)
		c = abs(c);
	lp.xl -= c;
	lp.yd -= c;
	lp.zf -= c;
	lp.xr += c;
	lp.yu += c;
	lp.zr += c;
}

void BoundingBox::shiftCoordinateProcent(LastPoints & lp, float pr)
{
	// x
	float xShift = abs(lp.xl / 100 * pr) > abs(lp.xr / 100 * pr) ? abs(lp.xl / 100 * pr) : abs(lp.xr / 100 * pr);
	lp.xl -= xShift;
	lp.xr += xShift;
	// y
	float yShift = abs(lp.yd / 100 * pr) > abs(lp.yu / 100 * pr) ? abs(lp.yd / 100 * pr) : abs(lp.yu / 100 * pr);
	lp.yd -= yShift;
	lp.yu += yShift;
	// z
	float zShift = abs(lp.zf / 100 * pr) > abs(lp.zr / 100 * pr) ? abs(lp.zf / 100 * pr) : abs(lp.zr / 100 * pr);
	lp.zf -= zShift;
	lp.zr += zShift;
	
	//lp.xl -= abs(lp.xl / 100 * pr);
	//lp.yd -= abs(lp.yd / 100 * pr);
	//lp.zf -= abs(lp.zf / 100 * pr);
	//lp.xr += abs(lp.xr / 100 * pr);
	//lp.yu += abs(lp.yu / 100 * pr);
	//lp.zr += abs(lp.zr / 100 * pr);
}

vector<VertexColor> BoundingBox::generateVertexForDraw(const LastPoints& lp)
{
	Point vert[] =
	{
		// первая сторона
		Point(lp.xl, lp.yd, lp.zf),
		Point(lp.xl, lp.yu, lp.zf),
		Point(lp.xr, lp.yu, lp.zf),
		Point(lp.xr, lp.yd, lp.zf),
		Point(lp.xl, lp.yd, lp.zf),
		// вторая сторона
		Point(lp.xr, lp.yd, lp.zf),
		Point(lp.xr, lp.yu, lp.zf),
		Point(lp.xr, lp.yu, lp.zr),
		Point(lp.xr, lp.yd, lp.zr),
		Point(lp.xr, lp.yd, lp.zf),
		//// третья сторона
		Point(lp.xr, lp.yd, lp.zr),
		Point(lp.xr, lp.yu, lp.zr),
		Point(lp.xl, lp.yu, lp.zr),
		Point(lp.xl, lp.yd, lp.zr),
		Point(lp.xr, lp.yd, lp.zr),
		//// четвертая сторона
		Point(lp.xl, lp.yd, lp.zr),
		Point(lp.xl, lp.yu, lp.zr),
		Point(lp.xl, lp.yu, lp.zf),
		Point(lp.xl, lp.yd, lp.zf),
		Point(lp.xl, lp.yd, lp.zr)
	}; // массив вершин для обработки

	vector<VertexColor> vertex;
	int size(sizeof vert / sizeof vert[0]);
	Color color(1, 1, 1, 1);
	for (int i(0); i < size; i++)
		vertex.push_back({ vert[i], color });
	return vertex;
}

vector<Vertex> BoundingBox::generateVertexForFrustumCulling(const LastPoints & lp)
{
	return vector<Vertex>
	{
		Vertex(Point(lp.xl, lp.yd, lp.zf)),
		Vertex(Point(lp.xl, lp.yu, lp.zf)),
		Vertex(Point(lp.xr, lp.yu, lp.zf)),
		Vertex(Point(lp.xr, lp.yd, lp.zf)),
		Vertex(Point(lp.xl, lp.yd, lp.zr)),
		Vertex(Point(lp.xl, lp.yu, lp.zr)),
		Vertex(Point(lp.xr, lp.yu, lp.zr)),
		Vertex(Point(lp.xr, lp.yd, lp.zr))
	};
}

Vector BoundingBox::calculateCenterBoundingBox(const LastPoints& lp)
{
	return Vector((lp.xr + lp.xl) / 2, (lp.yu + lp.yd) / 2, (lp.zr + lp.zf) / 2, 0);
}

vector<VertexColor> BoundingBox::generateVertexForRayPick(const LastPoints& lp)
{
	VertexColor vc[] =
	{
		// первая сторона
		VertexColor(lp.xl, lp.yd, lp.zf),
		VertexColor(lp.xl, lp.yu, lp.zf),
		VertexColor(lp.xr, lp.yu, lp.zf),
		VertexColor(lp.xr, lp.yd, lp.zf),
		VertexColor(lp.xl, lp.yd, lp.zf),
		VertexColor(lp.xr, lp.yu, lp.zf),
		// вторая сторона
		VertexColor(lp.xr, lp.yd, lp.zf),
		VertexColor(lp.xr, lp.yu, lp.zf),
		VertexColor(lp.xr, lp.yu, lp.zr),
		VertexColor(lp.xr, lp.yd, lp.zr),
		VertexColor(lp.xr, lp.yd, lp.zf),
		VertexColor(lp.xr, lp.yu, lp.zr),
		// третья сторона
		VertexColor(lp.xr, lp.yd, lp.zr),
		VertexColor(lp.xr, lp.yu, lp.zr),
		VertexColor(lp.xl, lp.yu, lp.zr),
		VertexColor(lp.xl, lp.yd, lp.zr),
		VertexColor(lp.xr, lp.yd, lp.zr),
		VertexColor(lp.xl, lp.yu, lp.zr),
		// четвертая сторона
		VertexColor(lp.xl, lp.yd, lp.zr),
		VertexColor(lp.xl, lp.yu, lp.zr),
		VertexColor(lp.xl, lp.yu, lp.zf),
		VertexColor(lp.xl, lp.yu, lp.zf),
		VertexColor(lp.xl, lp.yd, lp.zf),
		VertexColor(lp.xl, lp.yd, lp.zr),
		// пятая сторона
		VertexColor(lp.xl, lp.yu, lp.zf),
		VertexColor(lp.xl, lp.yu, lp.zr),
		VertexColor(lp.xr, lp.yu, lp.zr),
		VertexColor(lp.xr, lp.yu, lp.zr),
		VertexColor(lp.xr, lp.yu, lp.zf),
		VertexColor(lp.xl, lp.yu, lp.zf),
		// шестая сторона
		VertexColor(lp.xl, lp.yd, lp.zr),
		VertexColor(lp.xl, lp.yd, lp.zf),
		VertexColor(lp.xr, lp.yd, lp.zf),
		VertexColor(lp.xr, lp.yd, lp.zf),
		VertexColor(lp.xr, lp.yd, lp.zr),
		VertexColor(lp.xl, lp.yd, lp.zr)
	}; // массив вершин для обработки
	return vector<VertexColor>(vc, vc + 36);
}

void BoundingBox::move_data(BoundingBox && bb)
{
	center = bb.center;
	arrayPointsBox = std::move(bb.arrayPointsBox);
	arrayForDraw = std::move(bb.arrayForDraw);
	vertexShader = bb.vertexShader; bb.vertexShader = nullptr;
	pixelShader = bb.pixelShader; bb.pixelShader = nullptr;
	vertexFormat = bb.vertexFormat; bb.vertexFormat = nullptr;
	vertexBuffer = bb.vertexBuffer; bb.vertexBuffer = nullptr;
	constantsBufferVS = bb.constantsBufferVS; bb.constantsBufferVS = nullptr;
	constantsBufferPS = bb.constantsBufferPS; bb.constantsBufferPS = nullptr;
	arrayPointsFrustum = std::move(bb.arrayPointsFrustum);
	visible = bb.visible;
}

bool BoundingBox::_init(ID3D11Device* device, CollectionShaders& collShaders, const wstring& vertexShader)
{
	D3D11_INPUT_ELEMENT_DESC ied[2] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,0,D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0,12,D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};
	auto tmpBuf = collShaders.getBlobVertexShader(vertexShader);
	HRESULT hr = device->CreateInputLayout(ied, 2, tmpBuf->GetBufferPointer(), tmpBuf->GetBufferSize(), &vertexFormat);
	if (hr != S_OK)
	{
		//LOGMESSAGE("Error create input layout!");
		return false;
	}

	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof D3D11_BUFFER_DESC);
	bd.ByteWidth = sizeof(ConstantBufferNoLight);
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	hr = device->CreateBuffer(&bd, NULL, &constantsBufferVS); 
	if (hr != S_OK)
	{
		//LOGMESSAGE("Error create constant buffer!");
		return false;
	}

	ZeroMemory(&bd, sizeof D3D11_BUFFER_DESC);
	bd.ByteWidth = UINT(sizeof(VertexColor) * arrayForDraw.size());
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	D3D11_SUBRESOURCE_DATA subd;
	ZeroMemory(&subd, sizeof(subd));
	subd.pSysMem = this->arrayForDraw.data();
	hr = device->CreateBuffer(&bd, &subd, &vertexBuffer);
	if (hr != S_OK)
	{
		//LOGMESSAGE("Error create vertex buffer!");
		return false;
	}

	ZeroMemory(&bd, sizeof D3D11_BUFFER_DESC);
	bd.ByteWidth = sizeof(ConstantBufferModePS);
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	hr = device->CreateBuffer(&bd, NULL, &constantsBufferPS);
	if (hr != S_OK)
	{
		//LOGMESSAGE("Error create constant buffer!");
		return false;
	}

	return true;
}

Vector & BoundingBox::getCenterBoundingBox()
{
	return center;
}

vector<VertexColor>& BoundingBox::getVertexForRayPick()
{
	return arrayPointsBox;
}

vector<VertexColor>& BoundingBox::getVertexForDraw()
{
	return arrayForDraw;
}

vector<Vertex>& BoundingBox::getVertexForFrustumCulling()
{
	return this->arrayPointsFrustum;
}

BoundingBox::~BoundingBox()
{
	if(vertexShader)
		vertexShader->Release();
	if(pixelShader)
		pixelShader->Release();
	if (vertexFormat)
		vertexFormat->Release();
	if (vertexBuffer)
		vertexBuffer->Release();
	if (constantsBufferVS)
		constantsBufferVS->Release();
	if (constantsBufferPS)
		constantsBufferPS->Release();
}

// BBox для инстансинга
BoundingBoxInstansing::BoundingBoxInstansing(float xl, float yd, float zf, float xr, float yu, float zr):physicVisible(new PhysicBuffer<int>())
{
	LastPoints lp = { xl, yd, zf, xr, yu, zr };
	this->shiftCoordinateProcent(lp, 5);
	center = BoundingBox::calculateCenterBoundingBox(lp); // запоминаем изначальный центр модели
	arrayPointsBox = BoundingBox::generateVertexForRayPick(lp); // сохраняем главную модель для ray pick
	arrayForDraw = this->generateVertexForDraw(lp); // массив вершин для отрисовки
	physicVisible->allocate(LogicBuffer<int>(0)); // добавляем в физический буфер первый флаг
}

BoundingBoxInstansing::BoundingBoxInstansing(float xl, float yd, float zf, float xr, float yu, float zr, const Vector & v) :physicVisible(new PhysicBuffer<int>())
{
	LastPoints lp = { xl, yd, zf, xr, yu, zr };
	this->shiftCoordinateProcent(lp, 5);
	center = v;
	arrayForDraw = this->generateVertexForDraw(lp); // массив вершин для отрисовки
	arrayPointsBox = BoundingBox::generateVertexForRayPick(lp); // сохраняем главную модель для ray pick
	physicVisible->allocate(LogicBuffer<int>(0)); // добавляем в физический буфер первый флаг
}

BoundingBoxInstansing::BoundingBoxInstansing(BoundingBoxInstansing && bbi): BoundingBox(std::move(bbi))
{
	geometryShader = bbi.geometryShader; bbi.geometryShader = nullptr;
	visible = std::move(bbi.visible); 
	physicVisible = std::move(bbi.physicVisible); 
}

const BoundingBoxInstansing & BoundingBoxInstansing::operator=(BoundingBoxInstansing && bbi)
{
	if (this != &bbi)
	{
		this->move_data(std::move(bbi));
		geometryShader = bbi.geometryShader; bbi.geometryShader = nullptr;
		visible = std::move(bbi.visible); // LogicBuffer - добавить move operator
		physicVisible = std::move(bbi.physicVisible); 
	}
	return *this;
}

void BoundingBoxInstansing::reVisible(int index)
{
	physicVisible->bufferCPU[index].data = physicVisible->bufferCPU[index].data ? false : true;
	physicVisible->isReCreate = true;
}

void BoundingBoxInstansing::show(int index)
{
	physicVisible->bufferCPU[index].data = 1;
	physicVisible->isReCreate = true;
}

void BoundingBoxInstansing::hide(int index)
{
	physicVisible->bufferCPU[index].data = 0;
	physicVisible->isReCreate = true;
}

bool BoundingBoxInstansing::isVisible(int index)
{
	return physicVisible->bufferCPU[index].data;
}

void BoundingBoxInstansing::reCreateStructuredBuffer(ID3D11Device * device)
{
	this->physicVisible->rebuildPages(device);
}

void BoundingBoxInstansing::addModel()
{
	//visible.push_back(0); // добавляем флаг для новой модели
	//Matrix out = m1 * m2 * m3;
	//Vector centerNewModel = center; centerNewModel.vector4TransformCoord(out);  centers.push_back(centerNewModel); // добавляем центр новой модели
	////auto arr = arrayPointsBox; transformModel(arr, m); buf.push_back(arr); // добавляем новый инстанс в буфер для ray pick
	physicVisible->allocate(LogicBuffer<int>(0)); // обновляем физический буфер флагов
}

bool BoundingBoxInstansing::eraseModel(int index)
{
	if (physicVisible->bufferCPU.size() >= index)
		return false;
	physicVisible->release(index);
	return true;
}

void BoundingBoxInstansing::draw(ID3D11DeviceContext * devcontx, const ConstantBufferNoLightInstansing & buff, ID3D11ShaderResourceView * arrayPosition)
{
	UINT start = 0;
	UINT sizeVertex = sizeof VertexColor;
	devcontx->IASetVertexBuffers(NULL, 1, &vertexBuffer, &sizeVertex, &start);
	devcontx->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP);

	devcontx->IASetInputLayout(vertexFormat);
	devcontx->VSSetShader(vertexShader, NULL, NULL);
	devcontx->GSSetShader(geometryShader, NULL, NULL); // установка геометрического шейдера
	devcontx->PSSetShader(pixelShader, NULL, NULL);
	devcontx->UpdateSubresource(constantsBufferVS, 0, NULL, &buff, 0, 0);
	devcontx->VSSetConstantBuffers(0, 1, &constantsBufferVS);

	devcontx->VSSetShaderResources(0, 1, &arrayPosition); // отправляем в шейдер структурный буфер позиций инстансов
	devcontx->VSSetShaderResources(1, 1, &physicVisible->physicShaderResourceView); // отправляем в шейдер структурный буфер флагов инстансов

	ConstantBufferModePS cbps;
	cbps.mode = 1;
	devcontx->UpdateSubresource(constantsBufferPS, 0, NULL, &cbps, 0, 0);
	devcontx->PSSetConstantBuffers(0, 1, &constantsBufferPS);

	devcontx->DrawInstanced((UINT)this->arrayForDraw.size(), (UINT)physicVisible->bufferCPU.size(), 0, 0); // вызываем отрисовку всех инстансов
}

bool BoundingBoxInstansing::init(ID3D11Device * device, CollectionShaders & collShaders, const array<wstring, 3 >& _shaders)
{
	// создаем вершинный буфер
	vertexShader = collShaders.getShadersVertex(_shaders[0]);
	geometryShader = collShaders.getShadersGeometry(_shaders[1]);
	pixelShader = collShaders.getShadersPixel(_shaders[2]);
	return _init(device, collShaders, _shaders[0]);
}

BoundingBoxInstansing::~BoundingBoxInstansing()
{
	if (geometryShader)
		geometryShader->Release();
}

// класс треугольник
Point3 Triangle::calculate_center(const Point3 & a, const Point3 & b, const Point3 & c)
{
	float _x = (a[x] + b[x] + c[x]) / 3;
	float _y = (a[y] + b[y] + c[y]) / 3;
	float _z = (a[z] + b[z] + c[z]) / 3;
	o = { _x, _y, _z };
	return o;
}

Triangle::Triangle(const Point3 & a, const Point3 & b, const Point3 & c):a(a), b(b), c(c)
{
	o = this->calculate_center(a, b, c);
}

Triangle::Triangle()
{
	this->createEmpty();
}

void Triangle::createEmpty()
{
	a = b = c = 0;
}

bool Triangle::isEmpty()const
{
	return (a == 0 && b == 0 && c == 0);
}

Point3  Triangle::getCenter()const
{
	return o;
}

bool Triangle::operator<(const Triangle & v) const
{
	if (this->o[x] == v.o[x]) 
	{
		if (this->o[y] == v.o[y])
		{
			if (this->o[z] >= v.o[z])
				return false;
			else
				return true;
		}
		else if (this->o[y] > v.o[y])
			return false;
		else
			return true;
	}
	else if (this->o[x] > v.o[x])
		return false;
	else
		return true;
}

// компаратор для сравнения треугольников
TrianglePred::TrianglePred(const Vector & v):cam(v)
{
}

bool TrianglePred::operator()(const pair<int, Triangle> & t1, const pair<int, Triangle> & t2)
{
	const Triangle* _t1 = &t1.second;
	const Triangle* _t2 = &t2.second;

	float length1 = ((Vector)_t1->getCenter() - cam).length(); // получаем длину от камеры до первого треугольника
	float length2 = ((Vector)_t2->getCenter() - cam).length(); // получаем длину от камеры до второго втреугольника
	return length1 < length2;
}

// компаратор для сравнения треугольников инстансинговых моделей
TriangleInstancedPred::TriangleInstancedPred(const Vector & v):cam(v)
{
}

bool TriangleInstancedPred::operator()(const pair<pair<int, int>, Triangle>& t1, const pair<pair<int, int>, Triangle>& t2)
{
	const Triangle* _t1 = &t1.second;
	const Triangle* _t2 = &t2.second;

	float length1 = ((Vector)_t1->getCenter() - cam).length(); // получаем длину от камеры до первого треугольника
	float length2 = ((Vector)_t2->getCenter() - cam).length(); // получаем длину от камеры до второго втреугольника
	return length1 < length2;
}

// базовый класс - модель
void _model::init_data(char* versionShaderModel)
{
	vertexBuffer = indexBuffer = nullptr;
	constantsBufferVS = constantsBufferPS = nullptr;
	vertexFormat = nullptr;
	translation = direction = scale = Matrix::Identity();
	this->versionShaderModel = versionShaderModel;
	sizeByteTypeVertex = -1;
	type = TypePrimitive::NoPrimitive;
}

_model::_model(_model && m)
{
	this->move_data(std::move(m));
}

const _model & _model::operator=(_model && m)
{
	if (this != &m)
		this->move_data(std::move(m));
	return *this;
}

void _model::move_data(_model && m)
{
	this->versionShaderModel = m.versionShaderModel; m.versionShaderModel = nullptr;
	this->index = std::move(m.index);
	this->countVertex = m.countVertex;
	this->countIndex = m.countIndex;
	this->translation = m.translation;
	this->direction = m.direction;
	this->scale = m.scale;
	this->sizeByteTypeVertex = m.sizeByteTypeVertex;
	this->vertexBuffer = m.vertexBuffer; m.vertexBuffer = nullptr;
	this->vertexFormat = m.vertexFormat; m.vertexFormat = nullptr;
	this->indexBuffer = m.indexBuffer;  m.indexBuffer = nullptr;
	this->constantsBufferVS = m.constantsBufferVS; m.constantsBufferVS = nullptr;
	this->constantsBufferPS = m.constantsBufferPS; m.constantsBufferPS = nullptr;
	this->shaderBuf = std::move(m.shaderBuf);
	this->type = m.type;
}

void _model::clear_data()
{
	if (this->vertexBuffer)
		this->vertexBuffer->Release();
	if (this->vertexFormat)
		this->vertexFormat->Release();
	if (this->indexBuffer)
		this->indexBuffer->Release();
	if (this->constantsBufferVS)
		this->constantsBufferVS->Release();
	if (this->constantsBufferPS)
		this->constantsBufferPS->Release();
}

bool _model::loadFromFileObj(const string& path, DataFromFile* data)
{
	std::string temp; // временная строка
	std::vector<WORD> indBuf; // индексы
	std::vector<Point> verBuf; // вершины
	std::vector<Point> normBuf; // нормали
	std::vector<UVCoordinate> uv; // текстуры
	std::map<string, string> pathTextures; // массив: имя материала, текстура этого материала
	std::map<string, int> indexTexture; // массив: текстура, индекс текстуры
	std::ifstream file_model(path); // файл модели
	bool isTexture(false); // есть ли текстуры на моделе
	int currentIndexTexture(-1); // текущий индекс текстуры
	std::map<int, string> convertListTextures; // временная карта индекс текстуры - текстура
	std::vector<wstring> texturesListOut; // выходной список текстур для модели

	if (!file_model)
	{
		//wstring _f(path);
		//string out = "Error open file "; out += string(_f.begin(), _f.end()); out += "!";
		//LOGMESSAGE(out.c_str());
		return false;
	}

	// cначала грузим файл материалов
	string name_file_material(path); name_file_material.resize(name_file_material.length() - 3);
	std::ifstream file_textures(name_file_material + "mtl");
	if (file_textures)
	{
		while (true)
		{
			std::getline(file_textures, temp, '\n');
			if (file_textures.eof())
				break;
			if (!file_textures)
			{
				/*wstring _f(name_file_material + ".mtl");
				string out = "Error open file "; out += string(_f.begin(), _f.end()); out += "!";
				LOGMESSAGE(out.c_str());*/
				return false;
			}
			// тут грузим текстуры
			if (temp.find("newmtl ") != -1)
			{
				string mtl(temp.begin() + 7, temp.end());
				pathTextures.insert({ mtl, "" }); // добавили материал
				for (int i(0); i < 5; i++)
				{
					std::getline(file_textures, temp, '\n');
					if (!file_textures)
					{
						/*wstring _f(name_file_material + ".mtl");
						string out = "Error open file "; out += string(_f.begin(), _f.end()); out += "!";
						LOGMESSAGE(out.c_str());*/
						return false;
					}
				}

				// записываем путь к текстуре
				temp.erase(0, 7);
				std::string _temp("engine_resource/textures/");
				temp.insert(0, _temp);
				if (!fs::exists(fs::path(temp))) // текстуры нет
					temp = "engine_resource/textures/no_texture.dds"; // заменяем заглушкой
				if (indexTexture.find(temp) == indexTexture.end())
					indexTexture.insert(std::make_pair(temp, (int)indexTexture.size()));
				pathTextures[mtl] = temp;

				std::getline(file_textures, temp, '\n'); // читаем ненужную строку
				if (!file_textures)
				{
					/*wstring _f(name_file_material + L".mtl");
					string out = "Error open file "; out += string(_f.begin(), _f.end()); out += "!";
					LOGMESSAGE(out.c_str());*/
					return false;
				}
			}
		}
		isTexture = true;
	}	
	file_textures.close();
	std::for_each(indexTexture.begin(), indexTexture.end(), [&convertListTextures](const auto& el) {// переносим индексы в ключи, а текстуры в значение
		convertListTextures.insert({ el.second, el.first });
	});
	std::for_each(convertListTextures.begin(), convertListTextures.end(), [&texturesListOut](const auto& el) { // конструируем массив имен текстур
		texturesListOut.push_back({el.second.begin(), el.second.end()});
	});

	// считываем данные модели
	while (true)
	{
		std::getline(file_model, temp, '\n');
		if (file_model.eof())
			break;
		if (!file_model)
		{
			/*wstring _f(f);
			string out = "Error read file "; out += string(_f.begin(), _f.end()); out += "!";
			LOGMESSAGE(out.c_str());*/
			return false;
		}

		if (isTexture && temp.find("usemtl ") != -1) // добавляем текстуру и количество соответствующих ей вершин
		{
			auto key = pathTextures[string(temp.begin() + 7, temp.end())]; // получаем имя текстуры по имени материала
			currentIndexTexture = indexTexture[key]; // получаем индекс текстуры по имени текстуры
		}

		std::string number;
		std::vector<float> tmp;
		std::string sel;

		if (temp.length() >= 2)
			sel = temp.substr(0, 2);
		else
			sel = temp;

		if (sel == "v ") // вершины
		{
			temp += " ";
			temp.erase(0, 2);
			std::for_each(temp.begin(), temp.end(), [&number, &tmp](const char& e) { // итератор вылетает за границы
				if (e != ' ')
					number += e;
				else
				{
					tmp.push_back(0.f);
					std::stringstream ostream(number);
					ostream >> tmp.back();
					number.clear();
				}
			});
			// переворачиваем отзеркаленную модель
			/*D3DXMATRIX mirror;
			D3DXMatrixTranslation(&mirror, 0, 0, 0);
			mirror(2, 2) = -1;
			D3DXVec3TransformCoord(&vect, &vect, &mirror);*/
			// добавляем в буфер вершин
			verBuf.push_back({ tmp[0], tmp[1], -tmp[2] });
		}
		else if (sel == "vn") // нормали
		{
			temp += " ";
			temp.erase(0, 3);
			std::for_each(temp.begin(), temp.end(), [&number, &tmp](const char& e) {
				if (e != ' ')
					number += e;
				else
				{
					tmp.push_back(0.f);
					std::stringstream ostream(number);
					ostream >> tmp.back();
					number.clear();
				}
			});
			normBuf.push_back({ tmp[0], tmp[1], tmp[2] });
		}
		else if (sel == "vt") // текстуры
		{
			temp += " ";
			temp.erase(0, 3);
			std::for_each(temp.begin(), temp.end(), [&number, &tmp](const char& e) {
				if (e != ' ')
					number += e;
				else
				{
					tmp.push_back(0.f);
					std::stringstream ostream(number);
					ostream >> tmp.back();
					number.clear();
				}
			});
			uv.push_back({ tmp[0], 0 - tmp[1] });
		}
		else if (sel == "f ") // фейсы(индексы)
		{
			std::vector<int> Tmp;
			temp += " ";
			temp.erase(0, 2);
			std::for_each(temp.begin(), temp.end(), [&number, &Tmp](const char& e) {
				if (e != '/' && e != ' ')
					number += e;
				else
				{
					Tmp.push_back(0);
					std::stringstream ostream(number);
					ostream >> Tmp.back();
					Tmp.back()--;
					number.clear();
				}
			});

			for (int i(0); i < 3; i++)
				std::swap(Tmp[i], Tmp[Tmp.size() - 3 + i]);

			for (int i(0); i < Tmp.size(); i += 3)
			{
				indBuf.push_back(Tmp[i]); // индекс модели
				indBuf.push_back(Tmp[i + 1]); // индекс нормали
				indBuf.push_back(Tmp[i + 2]); // индекст текстурных координат
				indBuf.push_back(currentIndexTexture); // индекс текстуры
			}
		}
	}
	file_model.close();

	IVNBuffers buf;
	for (int j(0), k(0); j < indBuf.size(); j += 4, k++)
	{
		int _0(indBuf[j]); int _1(indBuf[j + 1]); int _2(indBuf[j + 2]); UINT _3(indBuf[j + 3]);
		Point uvAndIndex = { uv[_1][x], uv[_1][y], (float)_3 };
		buf.first.push_back({ verBuf[_0], uvAndIndex, normBuf[_2] });
		buf.second.push_back(k);
	}

	DataFromFile loadingsData(texturesListOut, buf.first, buf.first.size(), buf.second, buf.second.size()); // загруженные из файла данные
	*data = loadingsData; // присваиваем данные

	return true;
}

bool _model::loadFromFileProObject(const string & path, DataFromFile * data)
{
	std::ifstream file(path, std::ios_base::binary);
	if (!file)
	{
		// TODO: print error message
		return false;
	}
	// переменные для модели
	int countMesh(0); // количество мешей в модели
	Vector center; // центр BBox
	BoundingBoxData bBoxData; // данные для конструирования BBox
	DataFromFile _data; // выходной буфер
	_data.isBBox = true; // ббокс будет загружаться из файла

	while (true)
	{
		int block(0);
		file.read((char*)&block, 4);
		if (file.eof())
			break;
		if (!file)
		{
			// TODO: print error message
			return false;
		}
		switch (block)
		{
		case 0x10:// блок служебной информации
		{
			string nameAutor;
			if (!loadLineFromFile(nameAutor, file)) // имя автора
			{
				// TODO: print error message
				return false;
			}
			DateTime dateCreate;
			file.read((char*)&dateCreate, sizeof DateTime); // дата создания
			if (!file)
			{
				// TODO: print error message
				return false;
			}
			DateTime dateModify;
			file.read((char*)&dateModify, sizeof DateTime); // дата модификации
			if (!file)
			{
				// TODO: print error message
				return false;
			}
			file.read((char*)&countMesh, 4); // количество мешей в модели
			if (!file)
			{
				// TODO: print error message
				return false;
			}
			// читаем центр модели
			float arr[6];
			for (int i(0); i < 3; i++)
			{
				file.read((char*)&arr[i], sizeof(float));
				if (!file)
				{
					// TODO: print error message
					return false;
				}
			}
			center = Vector(arr[0], arr[1], arr[2], 1);
			// читаем BBox модели(ограничивающие точки)
			for (int i(0); i < 6; i++)
			{
				file.read((char*)&arr[i], sizeof(float));
				if (!file)
				{
					// TODO: print error message
					return false;
				}
			}
			bBoxData = BoundingBoxData(arr[0], arr[1], arr[2], arr[3], arr[4], arr[5]);
			_data.bboxData = bBoxData;
			_data.bboxData.center = center; // записываем центр
			string unit; 
			if (!loadLineFromFile(unit, file)) // читаем единицы измерения
			{
				// TODO: print error message
				return false;
			}
			break;
		}

		case 0x20: // блок информации о каждом меше в модели
		{
			for (int i(0); i < countMesh; i++) // перебираем все меши
			{
				file.read((char*)&block, 4); // id подблока
				if (!file)
				{
					// TODO: print error message
					return false;
				}
				string nameMesh;
				if (!loadLineFromFile(nameMesh, file))  // читаем имя меша
				{
					// TODO: print error message
					return false;
				}
				int ci(0); file.read((char*)&ci, 4); // читаем количество индексов
				if (!file)
				{
					// TODO: print error message
					return false;
				}
				_data.indexCount += ci; // приращиваем количество индексов
				for (int j(0); j < ci; j++) // перебираем все индексы
				{
					int tmpIndex(-1); // временная переменная
					file.read((char*)&tmpIndex, 4); // читаем индекс
					if (!file)
					{
						// TODO: print error message
						return false;
					}
					_data.index.push_back((WORD)tmpIndex); // пишем считанный индекс в буфер
				}
			}
			break;
		}

		case 0x30: // блок информации о вершинах
		{
			file.read((char*)&_data.vertexCount, 4); // количество вершин
			if (!file)
			{
				// TODO: print error message
				return false;
			}
			for (int i(0); i < _data.vertexCount; i++) // перебираем все вершины
			{
				VertexNormal vn;// временная переменная
				file.read((char*)&vn, sizeof(VertexNormal)); // читаем вершину
				if (!file)
				{
					// TODO: print error message
					return false;
				}
				_data.vertex.push_back(vn); // пишем считанную вершину в буфер
			}
			break;
		}

		case 0x40: // блок информации о текстурах
		{
			int count(0);
			file.read((char*)&count, 4); // количество текстур
			if (!file)
			{
				// TODO: print error message
				return false;
			}
			_data.texturesListOut.resize(count); // создаём буфер текстур
			for (int i(0); i < count; i++)
			{
				int index(0);
				file.read((char*)&index, 4); // индекс текстуры
				if (!file)
				{
					// TODO: print error message
					return false;
				}
				string path;
				if (!loadLineFromFile(path, file)) // читаем путь к текстуре
				{
					// TODO: print error message
					return false;
				}
				_data.texturesListOut[index] = wstring(path.begin(), path.end()); // помещаем текстуру в буфер на позицию
			}
			break;
		}

		case 0x50: // блок настроек(пока только инстансинг или нет)
		{
			file.read((char*)&_data.isInstansing, sizeof(bool)); // количество текстур
			if (!file)
			{
				// TODO: print error message
				return false;
			}
			if (_data.isInstansing)
				_data.shaders = { L"textures_instansing", L"textures_instansing", L"textures" };
			else
				_data.shaders = { L"textures" };
			break;
		}

		default:
		{
			file.close();
			return false;
		}

		}
	}
	file.close();
	*data = _data;
	return true;
}

bool  _model::loadLineFromFile(string& line, std::ifstream & file)
{
	char s(' ');
	int size(0);
	while(true)
	{
		file.read((char*)&s, 1);
		if (!file)
		{
			// TODO: print error message
			return false;
		}
		if (s == '\0')
			break;
		line += s;
	}
	return true;
}

bool _model::loadFileFromExpansion(const string & fileName, DataFromFile * data)
{
	map<string, int> commands = { {".obj", 0}, {".pro_object", 1} };
	int num(0);
	auto iter = commands.find(fs::path(fileName).extension().string());
	if (iter != commands.end())
		num = iter->second;
	switch (num)
	{
	case 0:
		return loadFromFileObj(fileName, data); // загрузка *.obj

	case 1:
		return loadFromFileProObject(fileName, data); // загрузка *.pro_object

	default:
	{
		// TODO: неверное расширения файла, не грузим, пишем сообщение в лог
		return false;
	}
	}
}

void _model::setWorld(const CharactersModel& cm)
{
	translation = cm.position;
	direction = cm.direction;
	scale = cm.scale;
}

void _model::setType(const TypePrimitive & t)
{
	type = t;
}

TypePrimitive & _model::getType()
{
	return type;
}

_model::~_model()
{
	this->clear_data();
}


// абстракный класс - обычная модель
ModelDraw::ModelDraw()
{
}

ModelDraw::ModelDraw(ModelDraw && m):_model(std::move(m))
{
	this->modelBBox = std::move(m.modelBBox);
}

const ModelDraw & ModelDraw::operator=(ModelDraw && m)
{
	if (this != &m)
	{
		this->move_data(std::move(m));
		this->modelBBox = std::move(m.modelBBox);
	}
	return *this;
}

vector<VertexColor> ModelDraw::getBoundingBox()
{
	return modelBBox->getVertexForRayPick();
}

vector<Vertex> ModelDraw::getBoundingBoxFrustumCulling()
{
	return modelBBox->getVertexForFrustumCulling();
}

Vector  ModelDraw::getCenter()
{
	return modelBBox->getCenterBoundingBox();
}

// обычная модель в цвете
void* ModelDrawColor::operator new(size_t i)
{
	return _aligned_malloc(i, 16);
}

void ModelDrawColor::operator delete(void* p)
{
	_aligned_free(p);
}

void ModelDrawColor::initBoundingBox(const vector<VertexColor>& v)
{
	auto xl = std::min_element(v.begin(), v.end(), [](const VertexColor & v1, const VertexColor& v2) {
		return v1.Pos[x] < v2.Pos[x];
	});
	auto yd = std::min_element(v.begin(), v.end(), [](const VertexColor & v1, const VertexColor& v2) {
		return v1.Pos[y] < v2.Pos[y];
	});
	auto zf = std::min_element(v.begin(), v.end(), [](const VertexColor & v1, const VertexColor& v2) {
		return v1.Pos[z] < v2.Pos[z];
	});
	auto xr = std::max_element(v.begin(), v.end(), [](const VertexColor & v1, const VertexColor& v2) {
		return v1.Pos[x] < v2.Pos[x];
	});
	auto yu = std::max_element(v.begin(), v.end(), [](const VertexColor & v1, const VertexColor& v2) {
		return v1.Pos[y] < v2.Pos[y];
	});
	auto zr = std::max_element(v.begin(), v.end(), [](const VertexColor & v1, const VertexColor& v2) {
		return v1.Pos[z] < v2.Pos[z];
	});
	modelBBox.reset(new BoundingBox(xl->Pos[x], yd->Pos[y], zf->Pos[z], xr->Pos[x], yu->Pos[y], zr->Pos[z]));
}

ModelDrawColor::ModelDrawColor(char * versionShaderModel, const vector<VertexColor>& v, const vector<WORD>& ind, const wstring& s):ModelDraw()
{
	init_data(versionShaderModel);
	this->vertex = v;
	this->countVertex = v.size();
	this->index = ind;
	this->countIndex = ind.size();
	shader = s;
	initBoundingBox(v);
}

ModelDrawColor::ModelDrawColor(char * versionShaderModel, const wstring& s)
{
	init_data(versionShaderModel);
	shader = s;
}

ModelDrawColor::ModelDrawColor(ModelDrawColor && m): ModelDraw(std::move(m))
{
	this->vertex = std::move(m.vertex);
}

const ModelDrawColor & ModelDrawColor::operator=(ModelDrawColor && m)
{
	if (this != &m)
	{
		this->move_data(std::move(m));
		this->vertex = std::move(m.vertex);
	}
	return *this;
}

void ModelDrawColor::draw(ID3D11DeviceContext * devcontx, D3D11_FILLMODE mode, Camera camera, const Matrix & Projection)
{
	cb.Translation = Matrix::Transponse(translation);
	cb.Rotation = Matrix::Transponse(direction);
	cb.Scale = Matrix::Transponse(scale);
	cb.View = Matrix::Transponse(camera.getView());
	cb.Projection = Matrix::Transponse(Projection);

	UINT start = 0;
	devcontx->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R16_UINT, 0);
	devcontx->IASetVertexBuffers(NULL, 1, &vertexBuffer, &sizeByteTypeVertex, &start);

	int flag = mode == D3D11_FILLMODE::D3D11_FILLMODE_WIREFRAME ? 0 : 1;

	devcontx->UpdateSubresource(constantsBufferVS, 0, NULL, &cb, 0, 0);
	devcontx->VSSetConstantBuffers(0, 1, &constantsBufferVS);

	ConstantBufferModePS cbps;
	cbps.mode = flag;
	devcontx->UpdateSubresource(constantsBufferPS, 0, NULL, &cbps, 0, 0);
	devcontx->PSSetConstantBuffers(0, 1, &constantsBufferPS);

	devcontx->IASetInputLayout(vertexFormat);
	devcontx->VSSetShader(shaderBuf.vertexShader, NULL, NULL);
	devcontx->GSSetShader(NULL, NULL, NULL); // установка геометрического шейдера
	devcontx->PSSetShader(shaderBuf.pixelShader, NULL, NULL);
	devcontx->DrawIndexed((UINT)this->countIndex, 0, 0);
}

bool ModelDrawColor::init(ID3D11Device* device, CollectionTextures& collTextures, CollectionShaders& collShaders)
{
	modelBBox->init(device, collShaders, L"color");

	shaderBuf = collShaders.getShadersVP(shader);

	D3D11_INPUT_ELEMENT_DESC ied[2] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,0,D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0,12,D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};
	auto tmpBuf = collShaders.getBlobVertexShader(shader);
	HRESULT hr = device->CreateInputLayout(ied, 2, tmpBuf->GetBufferPointer(), tmpBuf->GetBufferSize(), &vertexFormat);
	if (hr != S_OK)
	{
		//LOGMESSAGE("Error create input layout!");
		return false;
	}

	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof D3D11_BUFFER_DESC);
	bd.ByteWidth = sizeof(ConstantBufferNoLight);
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	hr = device->CreateBuffer(&bd, NULL, &constantsBufferVS);
	if (hr != S_OK)
	{
		//LOGMESSAGE("Error create constant buffer!");
		return false;
	}

	ZeroMemory(&bd, sizeof D3D11_BUFFER_DESC);
	bd.ByteWidth = sizeof(ConstantBufferModePS);
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	hr = device->CreateBuffer(&bd, NULL, &constantsBufferPS);
	if (hr != S_OK)
	{
		//LOGMESSAGE("Error create constant buffer!");
		return false;
	}

	ZeroMemory(&bd, sizeof D3D11_BUFFER_DESC);
	bd.ByteWidth = UINT(sizeof(VertexColor) * this->countVertex);
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	D3D11_SUBRESOURCE_DATA subd;
	ZeroMemory(&subd, sizeof(subd));
	subd.pSysMem = this->vertex.data();
	hr = device->CreateBuffer(&bd, &subd, &vertexBuffer);
	if (hr != S_OK)
	{
		//LOGMESSAGE("Error create vertex buffer!");
		return false;
	}

	ZeroMemory(&bd, sizeof D3D11_BUFFER_DESC);
	ZeroMemory(&subd, sizeof(subd));
	bd.ByteWidth = (UINT)(sizeof(WORD) * this->countIndex);
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	subd.pSysMem = this->index.data();
	hr = device->CreateBuffer(&bd, &subd, &indexBuffer);
	if (hr != S_OK)
	{
		//LOGMESSAGE("Error create index buffer!");
		return false;
	}

	this->sizeByteTypeVertex = sizeof(VertexColor); // устанавливаем размер одной вершины
	vertex.clear();
	index.clear();

	return true;
}

void ModelDrawColor::drawSelected(ID3D11DeviceContext * devcontx)
{
	modelBBox->draw(devcontx, cb);
}

std::pair<vector<VertexColor>, vector<WORD>> ModelDrawColor::generateCube(const Color & c)
{
	float value = 5.f;
	vector<VertexColor> v = {
	{ Point(-value, value, -value), c },
	{ Point(value, value, -value), c   },
	{ Point(value, value, value), c  },
	{ Point(-value, value, value), c   },
	{ Point(-value, -value, -value), c   },
	{ Point(value, -value, -value), c },
	{ Point(value, -value, value), c},
	{ Point(-value, -value, value), c  }
	};
	vector<WORD> ind = {
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
	this->countIndex = ind.size();
	this->countVertex = v.size();
	this->vertex = std::move(v);
	this->index = std::move(ind);
	initBoundingBox(vertex);
	return { vertex , index };
}

// обычная модель, текстура + свет

void* ModelDrawLight::operator new(size_t i)
{
	return _aligned_malloc(i, 16);
}

void ModelDrawLight::operator delete(void* p)
{
	_aligned_free(p);
}

void ModelDrawLight::constructor(char * versionShaderModel, const wstring& s, BoundingBoxData* bbd)
{
	init_data(versionShaderModel);
	shader = s;
	if (!bbd)
	{
		auto xl = std::min_element(vertex.begin(), vertex.end(), [](const auto & v1, const auto& v2) {
			return v1.Pos[x] < v2.Pos[x];
		});
		auto yd = std::min_element(vertex.begin(), vertex.end(), [](const auto & v1, const auto& v2) {
			return v1.Pos[y] < v2.Pos[y];
		});
		auto zf = std::min_element(vertex.begin(), vertex.end(), [](const auto & v1, const auto& v2) {
			return v1.Pos[z] < v2.Pos[z];
		});
		auto xr = std::max_element(vertex.begin(), vertex.end(), [](const auto & v1, const auto& v2) {
			return v1.Pos[x] < v2.Pos[x];
		});
		auto yu = std::max_element(vertex.begin(), vertex.end(), [](const auto & v1, const auto& v2) {
			return v1.Pos[y] < v2.Pos[y];
		});
		auto zr = std::max_element(vertex.begin(), vertex.end(), [](const auto & v1, const auto& v2) {
			return v1.Pos[z] < v2.Pos[z];
		});
		modelBBox.reset(new BoundingBox(xl->Pos[x], yd->Pos[y], zf->Pos[z], xr->Pos[x], yu->Pos[y], zr->Pos[z]));
	}
	else
		modelBBox.reset(new BoundingBox(bbd->xl, bbd->yd, bbd->zf, bbd->xr, bbd->yu, bbd->zr, bbd->center));
}

ModelDrawLight::ModelDrawLight(char * versionShaderModel, const vector<VertexNormal>& v, const vector<WORD>& ind, const wstring& s, const vector<wstring>& text, BoundingBoxData* bBoxData):ModelDraw()
{
	this->vertex = v;
	this->countVertex = v.size();
	this->index = ind;
	this->countIndex = ind.size();
	this->listTextures = text;
	constructor(versionShaderModel, s, bBoxData);
}

ModelDrawLight::ModelDrawLight(ModelDrawLight && m) :ModelDraw(std::move(m))
{
	this->vertex = std::move(m.vertex);
	this->listTextures = std::move(m.listTextures);
	this->samplerState = m.samplerState; m.samplerState = nullptr;
	this->psb = std::move(m.psb);
}

const ModelDrawLight & ModelDrawLight::operator=(ModelDrawLight && m)
{
	if (this != &m)
	{
		this->move_data(std::move(m));
		this->vertex = std::move(m.vertex);
		this->listTextures = std::move(m.listTextures);
		this->samplerState = m.samplerState; m.samplerState = nullptr;
		this->psb = std::move(m.psb);
	}
	return *this;
}

void ModelDrawLight::draw(ID3D11DeviceContext * devcontx, D3D11_FILLMODE mode, Camera camera, const Matrix & Projection)
{
	cb.Translation = Matrix::Transponse(translation);
	cb.Rotation = Matrix::Transponse(direction);
	cb.Scale = Matrix::Transponse(scale);
	cb.View = Matrix::Transponse(camera.getView());
	cb.Projection = Matrix::Transponse(Projection);

	UINT start = 0;
	devcontx->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R16_UINT, 0);
	devcontx->IASetVertexBuffers(NULL, 1, &vertexBuffer, &sizeByteTypeVertex, &start);

	int flag = mode == D3D11_FILLMODE::D3D11_FILLMODE_WIREFRAME ? 0 : 1;

	devcontx->UpdateSubresource(constantsBufferVS, 0, NULL, &cb, 0, 0);
	devcontx->VSSetConstantBuffers(0, 1, &constantsBufferVS);

	ConstantBufferModePS cbps;
	cbps.mode = flag;
	devcontx->UpdateSubresource(constantsBufferPS, 0, NULL, &cbps, 0, 0);
	devcontx->PSSetConstantBuffers(0, 1, &constantsBufferPS);

	devcontx->PSSetShaderResources(0, 1, &textures);
	devcontx->PSSetSamplers(0, 1, &samplerState);
	devcontx->IASetInputLayout(vertexFormat);
	devcontx->VSSetShader(shaderBuf.vertexShader, NULL, NULL);
	devcontx->GSSetShader(NULL, NULL, NULL); // установка геометрического шейдера
	devcontx->PSSetShader(shaderBuf.pixelShader, NULL, NULL);
	devcontx->DrawIndexed((UINT)this->countIndex, 0, 0);
}

bool ModelDrawLight::init(ID3D11Device* device, CollectionTextures& collTextures, CollectionShaders& collShaders)
{
	HRESULT hr;

	modelBBox->init(device, collShaders, L"color");

	// получаем шейдеры
	shaderBuf = collShaders.getShadersVP(shader);

	// здесь создаем текстурный массив

	UINT size((UINT)listTextures.size());
	D3D11_TEXTURE2D_DESC texElementDesc;
	auto startTexture = collTextures.addTexture(device, listTextures[0]);
	startTexture->GetDesc(&texElementDesc);
	D3D11_TEXTURE2D_DESC texArrayDesc;
	texArrayDesc.Width = texElementDesc.Width;
	texArrayDesc.Height = texElementDesc.Height;
	texArrayDesc.MipLevels = texElementDesc.MipLevels;
	texArrayDesc.ArraySize = size;
	texArrayDesc.Format = texElementDesc.Format;
	texArrayDesc.SampleDesc.Count = 1;
	texArrayDesc.SampleDesc.Quality = 0;
	texArrayDesc.Usage = D3D11_USAGE_DEFAULT;
	texArrayDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	texArrayDesc.CPUAccessFlags = 0;
	texArrayDesc.MiscFlags = 0;
	ID3D11Texture2D* texArray(nullptr);
	hr = device->CreateTexture2D(&texArrayDesc, 0, &texArray);
	if (hr != S_OK)
		return false;
	ID3D11DeviceContext* context(nullptr);
	device->GetImmediateContext(&context);
	// копируем каждую текстуру в массив
	for (UINT texElement = 0; texElement < size; ++texElement)
	{
		// копируем каждый мип - мап текстуры в массив
		for (UINT mipLevel = 0; mipLevel < texElementDesc.MipLevels; ++mipLevel)
		{
			D3D11_MAPPED_SUBRESOURCE mappedTex2D;
			auto currentTexture = collTextures.addTexture(device, listTextures[texElement]);
			hr = context->Map(currentTexture, mipLevel, D3D11_MAP_READ, 0, &mappedTex2D);
			if (hr != S_OK)
				return false;

			context->UpdateSubresource(texArray,
				D3D11CalcSubresource(mipLevel, texElement, texElementDesc.MipLevels),
				0, mappedTex2D.pData, mappedTex2D.RowPitch, mappedTex2D.DepthPitch);

			context->Unmap(currentTexture, mipLevel);
		}
	}
	D3D11_SHADER_RESOURCE_VIEW_DESC viewDesc;
	viewDesc.Format = texArrayDesc.Format;
	viewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
	viewDesc.Texture2DArray.MostDetailedMip = 0;
	viewDesc.Texture2DArray.MipLevels = texArrayDesc.MipLevels;
	viewDesc.Texture2DArray.FirstArraySlice = 0;
	viewDesc.Texture2DArray.ArraySize = size;
	hr = device->CreateShaderResourceView(texArray, &viewDesc, &textures);
	if (hr != S_OK)
		return false;

	D3D11_INPUT_ELEMENT_DESC ied[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,0,D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,12,D3D11_INPUT_PER_VERTEX_DATA, 0 }, // координаты текстуры + индекс текстуры
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,24,D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};
	auto tmpBuf = collShaders.getBlobVertexShader(shader);
	hr = device->CreateInputLayout(ied, sizeof ied / sizeof ied[0], tmpBuf->GetBufferPointer(), tmpBuf->GetBufferSize(), &vertexFormat);
	if (hr != S_OK)
	{
		//LOGMESSAGE("Error create input layout!");
		return false;
	}

	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof D3D11_BUFFER_DESC);
	bd.ByteWidth = sizeof(ConstantBufferVS);
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	hr = device->CreateBuffer(&bd, NULL, &constantsBufferVS);
	if (hr != S_OK)
	{
		//LOGMESSAGE("Error create constant buffer!");
		return false;
	}

	ZeroMemory(&bd, sizeof D3D11_BUFFER_DESC);
	bd.ByteWidth = sizeof(ConstantBufferModePS);
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	hr = device->CreateBuffer(&bd, NULL, &constantsBufferPS);
	if (hr != S_OK)
	{
		//LOGMESSAGE("Error create constant buffer!");
		return false;
	}

	//ZeroMemory(&bd, sizeof D3D11_BUFFER_DESC);
	//bd.ByteWidth = sizeof(ConstantBufferDirectionalLightPS);
	//bd.Usage = D3D11_USAGE_DEFAULT;
	//bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	//hr = device->CreateBuffer(&bd, NULL, &constantsBufferPS);
	//if (hr != S_OK)
	//{
	//	//LOGMESSAGE("Error create constant buffer!");
	//	return false;
	//}

	ZeroMemory(&bd, sizeof D3D11_BUFFER_DESC);
	bd.ByteWidth = (UINT)(sizeof(VertexNormal) * this->countVertex);
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	D3D11_SUBRESOURCE_DATA subd;
	ZeroMemory(&subd, sizeof(subd));
	subd.pSysMem = vertex.data();
	hr = device->CreateBuffer(&bd, &subd, &vertexBuffer);
	if (hr != S_OK)
	{
		//LOGMESSAGE("Error create vertex buffer!");
		return false;
	}

	ZeroMemory(&bd, sizeof D3D11_BUFFER_DESC);
	ZeroMemory(&subd, sizeof(subd));
	bd.ByteWidth = (UINT)(sizeof(WORD) * this->countIndex);
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	subd.pSysMem = index.data();
	hr = device->CreateBuffer(&bd, &subd, &indexBuffer);
	if (hr != S_OK)
	{
		//LOGMESSAGE("Error create index buffer!");
		return false;
	}

	// Создание сэмпла (описания) текстуры
	D3D11_SAMPLER_DESC sampDesc;
	ZeroMemory(&sampDesc, sizeof(sampDesc));
	sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;      // Тип фильтрации
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;         // Задаем координаты
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	sampDesc.MinLOD = 0;
	sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
	// Создаем интерфейс сэмпла текстурирования
	hr = device->CreateSamplerState(&sampDesc, &samplerState);
	if (hr != S_OK)
	{
		//LOGMESSAGE("Error create sampler state!");
		return false;
	}


	this->sizeByteTypeVertex = sizeof(VertexNormal);
	vertex.clear();
	index.clear();

	return true;
}

void ModelDrawLight::drawSelected(ID3D11DeviceContext * devcontx)
{
	modelBBox->draw(devcontx, (ConstantBufferNoLight)cb);
}

ModelDrawLight::~ModelDrawLight()
{
	if (samplerState)
		samplerState->Release();
}

// базовый класс для инстансинга
vector<VertexColor> ModelInstansing::getBoundingBox()
{
	return modelBBox->getVertexForRayPick();
}

ModelInstansing::ModelInstansing()
{
}

ModelInstansing::ModelInstansing(ModelInstansing && m):_model(std::move(m))
{
	this->characters = std::move(m.characters);
	this->visible = std::move(m.visible);
	shaderGeometry = std::move(m.shaderGeometry);
	this->shaderPixel = std::move(m.shaderPixel);
	this->physicPosition = std::move(m.physicPosition);
	this->physicVisible = std::move(m.physicVisible);
	modelBBox = std::move(m.modelBBox);
}

const ModelInstansing & ModelInstansing::operator=(ModelInstansing && m)
{
	if (this != &m)
	{
		this->characters = std::move(m.characters);
		this->visible = std::move(m.visible);
		modelBBox = std::move(m.modelBBox);
		shaderGeometry = std::move(m.shaderGeometry);
		this->shaderPixel = std::move(m.shaderPixel);
		this->physicPosition = std::move(m.physicPosition);
		this->physicVisible = std::move(m.physicVisible);
	}
	return *this;
}

// модель инстансинга, в цвете
void ModelInstansingColor::add_model(const CharactersModel & m)
{
	physicPosition->allocate(AllTransformations(Matrix::Transponse(m.position), Matrix::Transponse(m.direction), Matrix::Transponse(m.scale)));
	// создаем физический буфер флагов вывода и добавляем в него новую позицию
	physicVisible->allocate(1);
}

ModelInstansingColor::ModelInstansingColor(char * versionShaderModel, const vector<VertexColor>& v, const vector<WORD>& ind, const array<wstring, 3>& s, const CharactersModel& m)
{
	init_data(versionShaderModel);
	shader = s[0];
	shaderGeometry = s[1];
	shaderPixel = s[2];
	auto xl = std::min_element(v.begin(), v.end(), [](const VertexColor & v1, const VertexColor& v2) {
		return v1.Pos[x] < v2.Pos[x];
	});
	auto yd = std::min_element(v.begin(), v.end(), [](const VertexColor & v1, const VertexColor& v2) {
		return v1.Pos[y] < v2.Pos[y];
	});
	auto zf = std::min_element(v.begin(), v.end(), [](const VertexColor & v1, const VertexColor& v2) {
		return v1.Pos[z] < v2.Pos[z];
	});
	auto xr = std::max_element(v.begin(), v.end(), [](const VertexColor & v1, const VertexColor& v2) {
		return v1.Pos[x] < v2.Pos[x];
	});
	auto yu = std::max_element(v.begin(), v.end(), [](const VertexColor & v1, const VertexColor& v2) {
		return v1.Pos[y] < v2.Pos[y];
	});
	auto zr = std::max_element(v.begin(), v.end(), [](const VertexColor & v1, const VertexColor& v2) {
		return v1.Pos[z] < v2.Pos[z];
	});
	modelBBox.reset(new BoundingBoxInstansing(xl->Pos[x], yd->Pos[y], zf->Pos[z], xr->Pos[x], yu->Pos[y], zr->Pos[z])); // создаём BBox

	this->vertex = v; 	// устанавливаем буфер вершин для отправки на GPU
	this->countVertex = v.size();
	// буфер индексов
	this->index = ind;
	this->countIndex = ind.size();
	physicPosition.reset(new PhysicBuffer<AllTransformations>());
	physicVisible.reset(new PhysicBuffer<int>());
	characters.push_back(m);
	this->add_model(m); // добавляем остальные части модели
}

ModelInstansingColor::ModelInstansingColor(ModelInstansingColor && m): ModelInstansing(std::move(m))
{
	vertex = std::move(m.vertex);
	shaderGeometry = std::move(m.shaderGeometry);
	shaderPixel = std::move(m.shaderPixel);
	physicPosition = std::move(m.physicPosition);
	physicVisible = std::move(m.physicVisible);
}

const ModelInstansingColor & ModelInstansingColor::operator=(ModelInstansingColor && m)
{
	if (this != &m)
	{
		this->move_data(std::move(m));
		vertex = std::move(m.vertex);
		shaderGeometry = std::move(m.shaderGeometry);
		shaderPixel = std::move(m.shaderPixel);
		physicPosition = std::move(m.physicPosition);
		physicVisible = std::move(m.physicVisible);	
	}
	return *this;
}

void ModelInstansingColor::reCreateStructuredBuffer(ID3D11Device * device)
{
	this->physicPosition->rebuildPages(device);
	this->physicVisible->rebuildPages(device);
	this->modelBBox->reCreateStructuredBuffer(device);
}

void ModelInstansingColor::addModel(const CharactersModel& m)
{
	modelBBox->addModel(); // добавляем инстанс BBox'а
	this->add_model(m);
}

bool ModelInstansingColor::eraseModel(int index)
{
	if (characters.size() >= index || visible.size() >= index || physicPosition->bufferCPU.size() >= index ||
		physicVisible->bufferCPU.size() >= index)
		return false;
	if (!modelBBox->eraseModel(index))
		return false;
	vector<CharactersModel>::iterator iterPosition = characters.begin(); std::advance(iterPosition, index);
	characters.erase(iterPosition);
	vector<bool>::iterator iterVisible = visible.begin(); std::advance(iterVisible, index);
	visible.erase(iterVisible);
	vector<LogicBuffer<AllTransformations>>::iterator  iterPhysicPosition = physicPosition->bufferCPU.begin(); std::advance(iterPhysicPosition, index);
	physicPosition->bufferCPU.erase(iterPhysicPosition);
	vector<LogicBuffer<int>>::iterator  iterPhysicBool = physicVisible->bufferCPU.begin(); std::advance(iterPhysicBool, index);
	physicVisible->bufferCPU.erase(iterPhysicBool);
	return true;
}

void ModelInstansingColor::reVisible(int index)
{
	visible[index] = visible[index] ? false : true;
	physicVisible->bufferCPU[index].data = physicVisible->bufferCPU[index].data ? false : true;
}

void ModelInstansingColor::show(int index)
{
	visible[index] = true;
	physicVisible->bufferCPU[index].data = true;
}

void ModelInstansingColor::hide(int index)
{
	visible[index] = false;
	physicVisible->bufferCPU[index].data = false;
}

bool ModelInstansingColor::isVisible(int index)
{
	return physicVisible->bufferCPU[index].data;
}

void ModelInstansingColor::reVisibleBBox(int index)
{
	modelBBox->reVisible(index);
}

void ModelInstansingColor::showBBox(int index)
{
	modelBBox->show(index);
}

void ModelInstansingColor::hideBBox(int index)
{
	modelBBox->hide(index);
}

bool ModelInstansingColor::isVisibleBBox(int index)
{
	return modelBBox->isVisible(index);
}

CharactersModel ModelInstansingColor::getCharactersInstanse(int index)
{
	if (index < 0 || index >= characters.size())
	{
		// описываем ошибку
		return CharactersModel();
	}
	return characters[index];
}

void ModelInstansingColor::draw(ID3D11DeviceContext * devcontx, D3D11_FILLMODE mode, Camera camera, const Matrix & Projection)
{
	//cb.Translation = Matrix::Transponse(translation);
	cb.View = Matrix::Transponse(camera.getView());
	cb.Projection = Matrix::Transponse(Projection);
	cb.center = modelBBox->getCenterBoundingBox();

	UINT start = 0;
	devcontx->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R16_UINT, 0);
	devcontx->IASetVertexBuffers(NULL, 1, &vertexBuffer, &sizeByteTypeVertex, &start);

	int flag = mode == D3D11_FILLMODE::D3D11_FILLMODE_WIREFRAME ? 0 : 1;

	devcontx->UpdateSubresource(constantsBufferVS, 0, NULL, &cb, 0, 0);
	devcontx->VSSetConstantBuffers(0, 1, &constantsBufferVS);

	ConstantBufferModePS cbps;
	cbps.mode = flag;
	devcontx->UpdateSubresource(constantsBufferPS, 0, NULL, &cbps, 0, 0);
	devcontx->PSSetConstantBuffers(0, 1, &constantsBufferPS);

	devcontx->IASetInputLayout(vertexFormat);
	devcontx->VSSetShader(shaderBuf.vertexShader, NULL, NULL);
	devcontx->GSSetShader(shaderBuf.geometryShader, NULL, NULL); // установка геометрического шейдера
	devcontx->PSSetShader(shaderBuf.pixelShader, NULL, NULL);
	devcontx->VSSetShaderResources(0, 1, &physicPosition->physicShaderResourceView); // отправляем в шейдер структурный буфер позиций инстансов
	devcontx->VSSetShaderResources(1, 1, &physicVisible->physicShaderResourceView); // отправляем в шейдер структурный буфер флагов инстансов

	devcontx->DrawIndexedInstanced((UINT)this->countIndex, (UINT)physicPosition->bufferCPU.size(), 0, 0, 0);
}

bool ModelInstansingColor::init(ID3D11Device * device, CollectionTextures & collTextures, CollectionShaders & collShaders)
{
	modelBBox->init(device, collShaders, { L"color_instansing", L"bbox_color_instansing", L"color" });

	shaderBuf.vertexShader = collShaders.getShadersVertex(shader);
	shaderBuf.geometryShader = collShaders.getShadersGeometry(shaderGeometry);
	shaderBuf.pixelShader = collShaders.getShadersPixel(shaderPixel);

	D3D11_INPUT_ELEMENT_DESC ied[2] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,0,D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0,12,D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};
	auto tmpBuf = collShaders.getBlobVertexShader(shader);
	HRESULT hr = device->CreateInputLayout(ied, 2, tmpBuf->GetBufferPointer(), tmpBuf->GetBufferSize(), &vertexFormat);
	if (hr != S_OK)
	{
		//LOGMESSAGE("Error create input layout!");
		return false;
	}

	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof D3D11_BUFFER_DESC);
	bd.ByteWidth = sizeof(ConstantBufferNoLight);
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	hr = device->CreateBuffer(&bd, NULL, &constantsBufferVS);
	if (hr != S_OK)
	{
		//LOGMESSAGE("Error create constant buffer!");
		return false;
	}

	ZeroMemory(&bd, sizeof D3D11_BUFFER_DESC);
	bd.ByteWidth = sizeof(ConstantBufferModePS);
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	hr = device->CreateBuffer(&bd, NULL, &constantsBufferPS);
	if (hr != S_OK)
	{
		//LOGMESSAGE("Error create constant buffer!");
		return false;
	}

	ZeroMemory(&bd, sizeof D3D11_BUFFER_DESC);
	bd.ByteWidth = UINT(sizeof(VertexColor) * this->countVertex);
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	D3D11_SUBRESOURCE_DATA subd;
	ZeroMemory(&subd, sizeof(subd));
	subd.pSysMem = this->vertex.data();
	hr = device->CreateBuffer(&bd, &subd, &vertexBuffer);
	if (hr != S_OK)
	{
		//LOGMESSAGE("Error create vertex buffer!");
		return false;
	}

	ZeroMemory(&bd, sizeof D3D11_BUFFER_DESC);
	ZeroMemory(&subd, sizeof(subd));
	bd.ByteWidth = (UINT)(sizeof(WORD) * this->countIndex);
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	subd.pSysMem = this->index.data();
	hr = device->CreateBuffer(&bd, &subd, &indexBuffer);
	if (hr != S_OK)
	{
		//LOGMESSAGE("Error create index buffer!");
		return false;
	}

	this->sizeByteTypeVertex = sizeof(VertexColor); // устанавливаем размер одной вершины
	vertex.clear();
	index.clear();

	return true;
}

void ModelInstansingColor::drawSelected(ID3D11DeviceContext * devcontx)
{
	modelBBox->draw(devcontx, cb, physicPosition->physicShaderResourceView);
}

// модель инстансинга, с текстурой и светом

void ModelInstansingLight::add_model(const CharactersModel & m)
{
	physicPosition->allocate(AllTransformations(Matrix::Transponse(m.position), Matrix::Transponse(m.direction), Matrix::Transponse(m.scale)));
	// создаем физический буфер флагов вывода и добавляем в него новую позицию
	physicVisible->allocate(1);
}

void ModelInstansingLight::constructor(char * versionShaderModel, const vector<wstring>& s, const CharactersModel& m, BoundingBoxData* bbd)
{
	init_data(versionShaderModel);
	shader = s[0];
	shaderGeometry = s[1];
	shaderPixel = s[2];

	if (!bbd)
	{
		auto xl = std::min_element(vertex.begin(), vertex.end(), [](const auto & v1, const auto& v2) {
			return v1.Pos[x] < v2.Pos[x];
		});
		auto yd = std::min_element(vertex.begin(), vertex.end(), [](const auto & v1, const auto& v2) {
			return v1.Pos[y] < v2.Pos[y];
		});
		auto zf = std::min_element(vertex.begin(), vertex.end(), [](const auto & v1, const auto& v2) {
			return v1.Pos[z] < v2.Pos[z];
		});
		auto xr = std::max_element(vertex.begin(), vertex.end(), [](const auto & v1, const auto& v2) {
			return v1.Pos[x] < v2.Pos[x];
		});
		auto yu = std::max_element(vertex.begin(), vertex.end(), [](const auto & v1, const auto& v2) {
			return v1.Pos[y] < v2.Pos[y];
		});
		auto zr = std::max_element(vertex.begin(), vertex.end(), [](const auto & v1, const auto& v2) {
			return v1.Pos[z] < v2.Pos[z];
		});
		modelBBox.reset(new BoundingBoxInstansing(xl->Pos[x], yd->Pos[y], zf->Pos[z], xr->Pos[x], yu->Pos[y], zr->Pos[z])); // создаём BBox
		// создаем структурные буферы
	}
	else
		modelBBox.reset(new BoundingBoxInstansing(bbd->xl, bbd->yd, bbd->zf, bbd->xr, bbd->yu, bbd->zr, bbd->center));

	physicPosition.reset(new PhysicBuffer<AllTransformations>());
	physicVisible.reset(new PhysicBuffer<int>());
	characters.push_back(m);
	this->add_model(m); // добавляем остальные части модели
}

ModelInstansingLight::ModelInstansingLight(char * versionShaderModel, const vector<VertexNormal>& v, const vector<WORD>& ind, const vector<wstring>& s, const vector<wstring>& text, const CharactersModel & m, BoundingBoxData* bBoxData)
{
	this->vertex = v; 	// устанавливаем буфер вершин для отправки на GPU
	this->countVertex = v.size();
	// буфер индексов
	this->index = ind;
	this->countIndex = ind.size();
	// лист текстур
	this->listTextures = text;
	// создаем модель
	this->constructor(versionShaderModel, s, m, bBoxData);
}

ModelInstansingLight::ModelInstansingLight(ModelInstansingLight && m): ModelInstansing(std::move(m))
{
	textures = m.textures; m.textures = nullptr;
	listTextures = std::move(m.listTextures);
	samplerState = m.samplerState; m.samplerState = nullptr;
	psb = std::move(m.psb);
	vertex = std::move(m.vertex);
	cb = std::move(m.cb);
}

const ModelInstansingLight & ModelInstansingLight::operator=(ModelInstansingLight && m)
{
	if (this != &m)
	{
		textures = m.textures; m.textures = nullptr;
		listTextures = std::move(m.listTextures);
		samplerState = m.samplerState; m.samplerState = nullptr;
		psb = std::move(m.psb);
		vertex = std::move(m.vertex);
		cb = std::move(m.cb);
	}
	return *this;
}

void ModelInstansingLight::reCreateStructuredBuffer(ID3D11Device * device)
{
	this->physicPosition->rebuildPages(device);
	this->physicVisible->rebuildPages(device);
	this->modelBBox->reCreateStructuredBuffer(device);
}

void ModelInstansingLight::addModel(const CharactersModel & m)
{
	modelBBox->addModel(); // добавляем инстанс BBox'а
	this->add_model(m);
}

bool ModelInstansingLight::eraseModel(int index)
{
	if (characters.size() >= index || visible.size() >= index || physicPosition->bufferCPU.size() >= index ||
		physicVisible->bufferCPU.size() >= index)
		return false;
	if (!modelBBox->eraseModel(index))
		return false;
	vector<CharactersModel>::iterator iterPosition = characters.begin(); std::advance(iterPosition, index);
	characters.erase(iterPosition);
	vector<bool>::iterator iterVisible = visible.begin(); std::advance(iterVisible, index);
	visible.erase(iterVisible);
	vector<LogicBuffer<AllTransformations>>::iterator  iterPhysicPosition = physicPosition->bufferCPU.begin(); std::advance(iterPhysicPosition, index);
	physicPosition->bufferCPU.erase(iterPhysicPosition);
	vector<LogicBuffer<int>>::iterator  iterPhysicBool = physicVisible->bufferCPU.begin(); std::advance(iterPhysicBool, index);
	physicVisible->bufferCPU.erase(iterPhysicBool);
	return true;
}

void ModelInstansingLight::reVisible(int index)
{
	visible[index] = visible[index] ? false : true;
	physicVisible->bufferCPU[index].data = physicVisible->bufferCPU[index].data ? false : true;

}

void ModelInstansingLight::show(int index)
{
	visible[index] = true;
	physicVisible->bufferCPU[index].data = true;
}

void ModelInstansingLight::hide(int index)
{
	visible[index] = false;
	physicVisible->bufferCPU[index].data = false;
}

bool ModelInstansingLight::isVisible(int index)
{
	return	physicVisible->bufferCPU[index].data;

}

void ModelInstansingLight::reVisibleBBox(int index)
{
	modelBBox->reVisible(index);
}

void ModelInstansingLight::showBBox(int index)
{
	modelBBox->show(index);
}

void ModelInstansingLight::hideBBox(int index)
{
	modelBBox->hide(index);
}

bool ModelInstansingLight::isVisibleBBox(int index)
{
	return modelBBox->isVisible(index);
}

CharactersModel ModelInstansingLight::getCharactersInstanse(int index)
{
	if (index < 0 || index >= characters.size())
	{
		// описываем ошибку
		return CharactersModel();
	}
	return characters[index];
}

void ModelInstansingLight::draw(ID3D11DeviceContext * devcontx, D3D11_FILLMODE mode, Camera camera, const Matrix & Projection)
{
	cb.View = Matrix::Transponse(camera.getView());
	cb.Projection = Matrix::Transponse(Projection);
	cb.center = modelBBox->getCenterBoundingBox();

	UINT start = 0;
	devcontx->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R16_UINT, 0);
	devcontx->IASetVertexBuffers(NULL, 1, &vertexBuffer, &sizeByteTypeVertex, &start);

	int flag = mode == D3D11_FILLMODE::D3D11_FILLMODE_WIREFRAME ? 0 : 1;

	devcontx->UpdateSubresource(constantsBufferVS, 0, NULL, &cb, 0, 0);
	devcontx->VSSetConstantBuffers(0, 1, &constantsBufferVS);

	ConstantBufferModePS cbps;
	cbps.mode = flag;
	devcontx->UpdateSubresource(constantsBufferPS, 0, NULL, &cbps, 0, 0);
	devcontx->PSSetConstantBuffers(0, 1, &constantsBufferPS);

	devcontx->PSSetShaderResources(0, 1, &textures);
	devcontx->PSSetSamplers(0, 1, &samplerState);
	devcontx->IASetInputLayout(vertexFormat);
	devcontx->VSSetShader(shaderBuf.vertexShader, NULL, NULL);
	devcontx->GSSetShader(shaderBuf.geometryShader, NULL, NULL); // установка геометрического шейдера
	devcontx->PSSetShader(shaderBuf.pixelShader, NULL, NULL);
	devcontx->VSSetShaderResources(0, 1, &physicPosition->physicShaderResourceView); // отправляем в шейдер структурный буфер позиций инстансов
	devcontx->VSSetShaderResources(1, 1, &physicVisible->physicShaderResourceView); // отправляем в шейдер структурный буфер флагов инстансов

	devcontx->DrawIndexedInstanced((UINT)this->countIndex, (UINT)physicPosition->bufferCPU.size(), 0, 0, 0);
}

bool ModelInstansingLight::init(ID3D11Device * device, CollectionTextures & collTextures, CollectionShaders & collShaders)
{
	HRESULT hr;
	modelBBox->init(device, collShaders, { L"color_instansing", L"bbox_color_instansing", L"color" });

	// получаем шейдеры
	shaderBuf.vertexShader = collShaders.getShadersVertex(shader);
	shaderBuf.geometryShader = collShaders.getShadersGeometry(shaderGeometry);
	shaderBuf.pixelShader = collShaders.getShadersPixel(shaderPixel);

	// здесь создаем текстурный массив
	UINT size((UINT)listTextures.size());
	D3D11_TEXTURE2D_DESC texElementDesc;
	auto startTexture = collTextures.addTexture(device, listTextures[0]);
	if (!startTexture)
	{
		// ошибка загрузки текстуры
		// выход...
	}
	startTexture->GetDesc(&texElementDesc);
	D3D11_TEXTURE2D_DESC texArrayDesc;
	texArrayDesc.Width = texElementDesc.Width;
	texArrayDesc.Height = texElementDesc.Height;
	texArrayDesc.MipLevels = texElementDesc.MipLevels;
	texArrayDesc.ArraySize = size;
	texArrayDesc.Format = texElementDesc.Format;
	texArrayDesc.SampleDesc.Count = 1;
	texArrayDesc.SampleDesc.Quality = 0;
	texArrayDesc.Usage = D3D11_USAGE_DEFAULT;
	texArrayDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	texArrayDesc.CPUAccessFlags = 0;
	texArrayDesc.MiscFlags = 0;
	ID3D11Texture2D* texArray(nullptr);
	hr = device->CreateTexture2D(&texArrayDesc, 0, &texArray);
	if (hr != S_OK)
		return false;
	ID3D11DeviceContext* context(nullptr);
	device->GetImmediateContext(&context);
	// копируем каждую текстуру в массив
	for (UINT texElement = 0; texElement < size; ++texElement)
	{
		// копируем каждый мип - мап текстуры в массив
		for (UINT mipLevel = 0; mipLevel < texElementDesc.MipLevels; ++mipLevel)
		{
			D3D11_MAPPED_SUBRESOURCE mappedTex2D;
			auto currentTexture = collTextures.addTexture(device, listTextures[texElement]);
			if (!currentTexture)
			{
				// ошибка загрузки текстуры
				// выход...
			}
			hr = context->Map(currentTexture, mipLevel, D3D11_MAP_READ, 0, &mappedTex2D);
			if (hr != S_OK)
				return false;

			context->UpdateSubresource(texArray,
				D3D11CalcSubresource(mipLevel, texElement, texElementDesc.MipLevels),
				0, mappedTex2D.pData, mappedTex2D.RowPitch, mappedTex2D.DepthPitch);

			context->Unmap(currentTexture, mipLevel);
		}
	}
	D3D11_SHADER_RESOURCE_VIEW_DESC viewDesc;
	viewDesc.Format = texArrayDesc.Format;
	viewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
	viewDesc.Texture2DArray.MostDetailedMip = 0;
	viewDesc.Texture2DArray.MipLevels = texArrayDesc.MipLevels;
	viewDesc.Texture2DArray.FirstArraySlice = 0;
	viewDesc.Texture2DArray.ArraySize = size;
	hr = device->CreateShaderResourceView(texArray, &viewDesc, &textures);
	if (hr != S_OK)
		return false;

	D3D11_INPUT_ELEMENT_DESC ied[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,0,D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,12,D3D11_INPUT_PER_VERTEX_DATA, 0 }, // координаты текстуры + индекс текстуры
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,24,D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};
	auto tmpBuf = collShaders.getBlobVertexShader(shader);
	hr = device->CreateInputLayout(ied, sizeof ied / sizeof ied[0], tmpBuf->GetBufferPointer(), tmpBuf->GetBufferSize(), &vertexFormat);
	if (hr != S_OK)
	{
		//LOGMESSAGE("Error create input layout!");
		return false;
	}

	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof D3D11_BUFFER_DESC);
	bd.ByteWidth = sizeof(ConstantBufferVS);
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	hr = device->CreateBuffer(&bd, NULL, &constantsBufferVS);
	if (hr != S_OK)
	{
		//LOGMESSAGE("Error create constant buffer!");
		return false;
	}

	ZeroMemory(&bd, sizeof D3D11_BUFFER_DESC);
	bd.ByteWidth = sizeof(ConstantBufferModePS);
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	hr = device->CreateBuffer(&bd, NULL, &constantsBufferPS);
	if (hr != S_OK)
	{
		//LOGMESSAGE("Error create constant buffer!");
		return false;
	}

	//ZeroMemory(&bd, sizeof D3D11_BUFFER_DESC);
	//bd.ByteWidth = sizeof(ConstantBufferDirectionalLightPS);
	//bd.Usage = D3D11_USAGE_DEFAULT;
	//bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	//hr = device->CreateBuffer(&bd, NULL, &constantsBufferPS);
	//if (hr != S_OK)
	//{
	//	//LOGMESSAGE("Error create constant buffer!");
	//	return false;
	//}

	ZeroMemory(&bd, sizeof D3D11_BUFFER_DESC);
	bd.ByteWidth = (UINT)(sizeof(VertexNormal) * this->countVertex);
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	D3D11_SUBRESOURCE_DATA subd;
	ZeroMemory(&subd, sizeof(subd));
	subd.pSysMem = vertex.data();
	hr = device->CreateBuffer(&bd, &subd, &vertexBuffer);
	if (hr != S_OK)
	{
		//LOGMESSAGE("Error create vertex buffer!");
		return false;
	}

	ZeroMemory(&bd, sizeof D3D11_BUFFER_DESC);
	ZeroMemory(&subd, sizeof(subd));
	bd.ByteWidth = (UINT)(sizeof(WORD) * this->countIndex);
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	subd.pSysMem = index.data();
	hr = device->CreateBuffer(&bd, &subd, &indexBuffer);
	if (hr != S_OK)
	{
		//LOGMESSAGE("Error create index buffer!");
		return false;
	}

	// Создание сэмпла (описания) текстуры
	D3D11_SAMPLER_DESC sampDesc;
	ZeroMemory(&sampDesc, sizeof(sampDesc));
	sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;      // Тип фильтрации
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;         // Задаем координаты
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.MipLODBias = 0.0f;
	sampDesc.MaxAnisotropy = 1;
	sampDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	sampDesc.BorderColor[0] = 0;
	sampDesc.BorderColor[1] = 0;
	sampDesc.BorderColor[2] = 0;
	sampDesc.BorderColor[3] = 0;
	sampDesc.MinLOD = 0;
	sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
	// Создаем интерфейс сэмпла текстурирования
	hr = device->CreateSamplerState(&sampDesc, &samplerState);
	if (hr != S_OK)
	{
		//LOGMESSAGE("Error create sampler state!");
		return false;
	}


	this->sizeByteTypeVertex = sizeof(VertexNormal);
	vertex.clear();
	index.clear();
	return true;
}

void ModelInstansingLight::drawSelected(ID3D11DeviceContext * devcontx)
{
	modelBBox->draw(devcontx, cb, physicPosition->physicShaderResourceView);
}

ModelInstansingLight::~ModelInstansingLight()
{
	if (textures)
		textures->Release();
	if (samplerState)
		samplerState->Release();
}
