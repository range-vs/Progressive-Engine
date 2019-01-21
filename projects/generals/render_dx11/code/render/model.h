#pragma once

// c++
#include <sstream>
#include <fstream>
#include <vector>
#include <array>
#include <functional>
#include <map>
#include <algorithm>
#include <iostream>
#include <set>
#include <string>
#include <list>

using std::function;
using std::vector;
using std::array;
using std::pair;
using std::map;
using std::set;
using std::string;
using std::wstring;
using std::list;

// my
#include "Camera.h"
#include "Light.h"
#include "textures_buffer.h"
#include "shaders_buffer.h"
#include "model_buffers.h"
#include "primitive.h"

// наименования:
// члены - с маленькой буквы, слова слитно, с большой буквы
// скрытые методы  - с маленькой буквы, слова через "_"
// открытые методы - с маленькой буквы, слова слитно, с большой буквы

// объявления
class BoundingBox;
class BoundingBoxInstansing;
class Triangle;
class TrianglePred;
class TriangleInstancedPred;
class _model;
class ModelDraw;
class ModelDrawColor;
class ModelDrawLight;
class ModelInstansing;
class ModelInstansingColor;
class ModelInstansingLight;

// типы данных упрощённые
using BBox = brain_memory<BoundingBox>;
using Model = brain_memory<_model>;
using ModelDrawPtr = brain_memory< ModelDraw>;
using ModelInstansingPtr = brain_memory< ModelInstansing>;
using ModelDrawColorPtr = brain_memory< ModelDrawColor>;
using ModelDrawLightPtr = brain_memory< ModelDrawLight>;
using BBoxInst = brain_memory<BoundingBoxInstansing>;
using ModelInstansingColorPtr = brain_memory<ModelInstansingColor>;
using ModelInstansingLightPtr = brain_memory<ModelInstansingLight>;
using PhysicPositions = brain_memory< PhysicBuffer<AllTransformations>>;

// классы

class BoundingBox // ограничивающий объём
{
protected:
	struct LastPoints
	{
		float xl, yd, zf; // лево, низ и перед
		float xr, yu, zr; // право, верх и зад
	};
	Vector center; // центр огранич. бокса
	vector<VertexColor> arrayPointsBox; // массив вершин бокса для ray pick
	vector<Vertex> arrayPointsFrustum; // массив вершин бокса для frustum culling
	vector<VertexColor> arrayForDraw; // массив вершин бокса для отрисовки
	ID3D11VertexShader* vertexShader; // вершинный шейдер
	ID3D11PixelShader* pixelShader; // пиксельный шейдер
	ID3D11InputLayout* vertexFormat; // формат вершин
	ID3D11Buffer* vertexBuffer; // буфер вершин
	ID3D11Buffer* constantsBufferVS; // константный буфер вершинный
	bool visible; // видно ли этот буфер
	ID3D11Buffer* constantsBufferPS; // константный буфер режема отрисовки

	void shiftCoordinate(LastPoints& lp, float c); // сдвиг всех координат
	void shiftCoordinateProcent(LastPoints& lp, float pr); // сдвиг всех координат на процент
	vector<VertexColor> generateVertexForDraw(const LastPoints& lp); // генерация массива вершин для отрисовки
	vector<Vertex> generateVertexForFrustumCulling(const LastPoints& lp); // генерация массива вершин для frustum culling
	Vector calculateCenterBoundingBox(const LastPoints& lp); // рассчёт центра
	vector<VertexColor> generateVertexForRayPick(const LastPoints& lp); // генерация массива вершин для ray pick
	void move_data(BoundingBox&& bb); // перемещение данных
	bool _init(ID3D11Device* device, CollectionShaders& collShaders, const wstring& vertexShader); // скрытый метод инициализации, обобщенный

	BoundingBox(){}
public:
	// конструкторы
	BoundingBox(float xl, float yd, float zf, float xr, float yu, float zr);
	BoundingBox(float xl, float yd, float zf, float xr, float yu, float zr, const Vector& c);
	BoundingBox(BoundingBox&& bb);
	const BoundingBox& operator=(BoundingBox&& bb);

	// переключатели и геттеры
	Vector& getCenterBoundingBox(); // получить центр модели
	vector<VertexColor>& getVertexForRayPick(); // получить буфер для проверки попадания луча
	vector<VertexColor>& getVertexForDraw(); // получить буфер для отрисовки
	vector<Vertex>& getVertexForFrustumCulling(); // получить буфер для frustum culling


	// методы
	bool init(ID3D11Device* device, CollectionShaders& collShaders, const wstring& shader); // инициализация BBox
	void draw(ID3D11DeviceContext * devcontx, const ConstantBufferNoLight& buff); // отрисовка BBox

	// деструктор
	virtual ~BoundingBox();
};

class BoundingBoxInstansing : public BoundingBox // ограничивающий объём для инстансинга
{

	PhysicVisibles physicVisible; // физический буфер для флагов
	ID3D11GeometryShader* geometryShader; // геометрический шейдер
public:
	// конструкторы
	BoundingBoxInstansing(float xl, float yd, float zf, float xr, float yu, float zr);
	BoundingBoxInstansing(float xl, float yd, float zf, float xr, float yu, float zr, const Vector& v);
	BoundingBoxInstansing(BoundingBoxInstansing&& bbi);
	const BoundingBoxInstansing& operator=(BoundingBoxInstansing&& bbi);

	// переключатели и геттеры
	void reVisible(int index); // показ/сокрытие инстанса по индексу
	void show(int index); // показать инстанс по индексу
	void hide(int index); // скрыть инстанс по индексу
	bool isVisible(int index); // видно ли инстанс по индексу

	// методы
	void reCreateStructuredBuffer(ID3D11Device* device); // пересоздаем буфер позиций константный
	void addModel(); // добавляем новый инстанс
	bool eraseModel(int index); // удаляем инстанс по индексу
	void draw(ID3D11DeviceContext * devcontx, const ConstantBufferNoLightInstansing& buff, ID3D11ShaderResourceView* arrayPosition); // отрисовка BBoxInst
	bool init(ID3D11Device* device, CollectionShaders& collShaders, const array<wstring,3 >& _shaders); // инициализация BBox
	template<class T> void transformModel(vector<T>& v, const Matrix& m); // умножение всех вершин на матрицу преобразования

	// деструктор
	~BoundingBoxInstansing();
};

class Triangle
{
	Point3 a; // вершины треугольника
	Point3 b;
	Point3 c;
	Point3 o; // центр треугольника

	Point3 calculate_center(const Point3& a, const Point3& b, const Point3&c); // рассчитать центр треугольника
public:
	// конструкторы
	Triangle(const Point3& a, const Point3& b, const Point3&c); // конструктор треугольника из трех вершин
	Triangle(); // конструирование "вырожденного"(несуществующего) треугольника

	// методы
	void createEmpty(); // метод конструирование "вырожденного"(несуществующего) треугольника
	bool isEmpty()const; // является ли треугольник вырожденным
	Point3 getCenter()const; // получить координаты центра трегольника

	// перегруженные операторы
	bool operator<(const Triangle & v) const;
};

class TrianglePred // компаратор для сравнения треугольников
{
	Vector cam;
public:
	TrianglePred(const Vector& v);
	bool operator()(const pair<int, Triangle>& t1, const pair<int, Triangle>& t2);
};

class TriangleInstancedPred // компаратор для сравнения треугольников инстансинговых моделей
{
	Vector cam;
public:
	TriangleInstancedPred(const Vector& v);
	bool operator()(const pair<pair<int, int>, Triangle>& t1, const pair<pair<int, int>, Triangle>& t2);
};

class _model // базовый класс - модель
{
	
protected:
	ID3D11Buffer* vertexBuffer; // буфер вершин
	ID3D11Buffer* indexBuffer; // буфер индексов
	vector<WORD> index;  // буфер индексов в ОЗУ, для raypick

	size_t countVertex; // количество вершин
	size_t countIndex; // количество индексов
	ID3D11Buffer* constantsBufferVS; // константный буфер вершинный
	ID3D11Buffer* constantsBufferPS; // константный буфер пиксельный
	ID3D11InputLayout* vertexFormat; // формат вершин

	Matrix translation; // мировая матрица позиции
	Matrix direction; // мировая матрица позиции
	Matrix scale; // мировая матрица позиции

	string versionShaderModel; // версия модели шейдеров
	wstring shader; // имя шейдера для этой модели
	shaders shaderBuf; // массив шейдеров

	// иницализация
	UINT sizeByteTypeVertex; // размер в байтах одной вершины

	// тип
	TypePrimitive type;
	
	// технические методы(инициализация переменных, копирование данных)
	void init_data(char* versionShaderModel); // инициализация переменных
	void move_data(_model&& m); // перемещение данных
	void clear_data(); // удаление данных

	_model(){} // конструктор - чтобы нельзя было создать экземпляр класса
public:
	// конструкторы
	_model(_model&& m);
	const _model& operator=(_model&& m);

	// методы
	static bool loadLineFromFile(string& line, std::ifstream& file); // загрузка строки из файла
	static bool loadFileFromExpansion(const string& fileName, DataFromFile* data); // выбор нужного метода загрузки исходя из расширения файла
	static bool loadFromFileObj(const string& path, DataFromFile* data); // загрузка *.obj - модели
	static bool loadFromFileProObject(const string& path, DataFromFile* data); // загрузка *.pro_object - модели
	void setWorld(const CharactersModel& cm); // устанавливаем мировую матрицу
	void setType(const TypePrimitive& t); // устанавливаем тип
	TypePrimitive& getType(); // получаем тип

	// виртуальные
	virtual void draw(ID3D11DeviceContext * devcontx, D3D11_FILLMODE mode, Camera camera, const Matrix & Projection) = 0; // метод отрисовки
	virtual void drawSelected(ID3D11DeviceContext * devcontx) = 0; // метод отрисовки bbox
	virtual vector<VertexColor> getBoundingBox() = 0; // получить bbox модели

	// деструктор
	virtual ~_model();
};

class ModelDraw : public _model // базовый для обычных моделей
{
protected:
	BBox modelBBox; // ограничивающий объем модели(параллепипед вокруг модели, для выделения)

public:
	// конструкторы
	ModelDraw(); // по - умолчанию
	ModelDraw(ModelDraw&& m); // перемещения
	const ModelDraw& operator=(ModelDraw&& m); // присвоение перемещения

	// абстрактные методы
	virtual void draw(ID3D11DeviceContext * devcontx, D3D11_FILLMODE mode, Camera camera, const Matrix & Projection) {}; // метод отрисовки
	virtual bool init(ID3D11Device* device, CollectionTextures& collTextures, CollectionShaders& collShaders) = 0; // метод инициализации
	virtual void drawSelected(ID3D11DeviceContext * devcontx) {}; // отрисовка выделителя модели

	// методы
	vector<VertexColor> getBoundingBox();
	vector<Vertex> getBoundingBoxFrustumCulling();
	Vector getCenter();

	// деструктор
	virtual ~ModelDraw() {}
};

class ModelDrawColor : public ModelDraw // модель без света, в цвете
{
	vector<VertexColor> vertex; // массив вершин в ОЗУ, для raypick
	ConstantBufferNoLight cb; // константный буфер на GPU

	void initBoundingBox(const vector<VertexColor>& v); // создание BBox
public:
	// выравнивание памяти
	void* operator new(size_t i); // выравнивае памяти
	void operator delete(void* p); // удаление класса из памяти

	// конструкторы
	ModelDrawColor(char* versionShaderModel,  // конструктор, версия шейдеров
									const vector<VertexColor>& v,  // вершины
									const vector<WORD>& ind,  // индексы
									const wstring& s
									); // список путей к шейдерам
	ModelDrawColor(char* versionShaderModel,  // конструктор, версия шейдеров
									const wstring& s // имя шейдера
								    ); // мировая матрица
	ModelDrawColor(ModelDrawColor&& m); // конструктор перемещения
	const ModelDrawColor& operator=(ModelDrawColor&& m); // конструктор присвоения перемещения

	// реализация чистых виртуальных методов
	void draw(ID3D11DeviceContext * devcontx, D3D11_FILLMODE mode, Camera camera, const Matrix & Projection);
	bool init(ID3D11Device* device, CollectionTextures& collTextures, CollectionShaders& collShaders);
	void drawSelected(ID3D11DeviceContext * devcontx);

	void setVertex(const vector< VertexColor>& v) { this->vertex = v; }
	void setIndex(const vector< WORD>& i) { this->index = i; }


	// уникальные методы
	std::pair<vector<VertexColor>, vector<WORD>> generateCube(const Color& c = Color(1.f, 1.f, 1.f, 1.f)); // генерируем куб

	// деструктор
	virtual ~ModelDrawColor() {}
};

class ModelDrawLight : public ModelDraw // модель освещённая(пока только directional light)
{
	vector<VertexNormal> vertex; // массив вершин в ОЗУ, для raypick
	ID3D11ShaderResourceView* textures; // массив текстур для шейдера(одинаковый размер!!!)
	vector<wstring> listTextures; // список путей к текстурам
	ID3D11SamplerState* samplerState; // формат текстур
	ConstantBufferDirectionalLightPS psb; // константный буфер для пиксельного шейдера(данные свет, как - то по другому сделать)
	ConstantBufferVS cb;

	void constructor(char * versionShaderModel, const wstring& s, BoundingBoxData* bbd = nullptr); // общий метод инициализации
public:
	// выравнивание памяти
	void* operator new(size_t i); // выравнивае памяти
	void operator delete(void* p); // удаление класса из памяти

	// конструкторы
	ModelDrawLight(char* versionShaderModel,  // конструктор(из набора вершин и индексов), версия шейдеров
								   const vector<VertexNormal>& v, // вершины
								   const vector<WORD>& ind, // индексы
							       const wstring& s, // список файлов шейдеров
								   const vector<wstring>& text, // список путей к текстурам
							   	   BoundingBoxData* bBoxData = nullptr // данные о bbox'е
								   ); // топология
	ModelDrawLight(ModelDrawLight&& m); // перемещение
	const ModelDrawLight& operator=(ModelDrawLight&& m); // присвоение перемещения

	// реализация чистых виртуальных методов
	void draw(ID3D11DeviceContext * devcontx, D3D11_FILLMODE mode, Camera camera, const Matrix & Projection);
	bool init(ID3D11Device* device, CollectionTextures& collTextures, CollectionShaders& collShaders);
	void drawSelected(ID3D11DeviceContext * devcontx);

	// деструктор
	virtual ~ModelDrawLight();
};

class ModelInstansing : public _model // базовая модель инстансинга
{
protected:
	vector<CharactersModel> characters; // массив характеристик каждого инстанса
	vector<bool> visible; // массив флагов, показывает какая модель видна, а какая нет
	BBoxInst modelBBox; // BBox 

	wstring shaderGeometry; // имя геометрического шейдера для этой модели
	wstring shaderPixel; // имя пиксельного шейдера для этой модели
	PhysicPositions physicPosition; // физический буфер для позиций
	PhysicVisibles physicVisible; // физический буфер для флагов

public:
	// конструкторы
	ModelInstansing(); // по - умолчанию
	ModelInstansing(ModelInstansing&& m); // перемещения
	const ModelInstansing& operator=(ModelInstansing&& m); // оператор перемещения

	// абстрактные методы
	virtual void reCreateStructuredBuffer(ID3D11Device* device) = 0; // пересоздаем все структурные буферы
	virtual void addModel(const CharactersModel& m) = 0; // добавляем инстанс
	virtual bool eraseModel(int index) = 0; // удаляем инстанс по индексу
	virtual void reVisible(int index) = 0; // показ/сокрытие модели
	virtual void show(int index) = 0; // показать модель
	virtual void hide(int index) = 0; // скрыть модель
	virtual bool isVisible(int index) = 0; // видно ли инстанс по индексу
	virtual void reVisibleBBox(int index) = 0; // показ/сокрытие модели
	virtual void showBBox(int index) = 0; // показать модель
	virtual void hideBBox(int index) = 0; // скрыть модель
	virtual bool isVisibleBBox(int index) = 0; // видно ли bbox инстанс'а по индексу
	virtual CharactersModel getCharactersInstanse(int index) = 0; // получаем характеристики заданного инстанса
	virtual void draw(ID3D11DeviceContext * devcontx, D3D11_FILLMODE mode, Camera camera, const Matrix & Projection) {}; // метод отрисовки
	virtual bool init(ID3D11Device* device, CollectionTextures& collTextures, CollectionShaders& collShaders) = 0; // метод инициализации
	virtual void drawSelected(ID3D11DeviceContext * devcontx) {}; // отрисовка выделителя модели

	// методы
	template<class T>  void transformModel(vector<T>& v, const Matrix& m); // умножение всех вершин на матрицу преобразования
	vector<VertexColor> getBoundingBox();

	// деструктор
	virtual ~ModelInstansing() {};
};

class ModelInstansingColor : public ModelInstansing
{
	vector<VertexColor> vertex; // буфер, который будет отправляться на GPU

	ConstantBufferNoLightInstansing cb;

	void add_model(const CharactersModel& m); // общий метод добавления инстанса
public:
	// конструкторы
	ModelInstansingColor(char* versionShaderModel,  // конструктор, версия шейдеров
											 const vector<VertexColor>& v,  // вершины
											 const vector<WORD>& ind,  // индексы
											 const array<wstring, 3>& s, // список путей к шейдерам
											 const CharactersModel& m // характеристика первого инстанса
											 ); // топология
	ModelInstansingColor(ModelInstansingColor&& m); // конструктор перемещения
	const ModelInstansingColor& operator=(ModelInstansingColor&& m); // конструктор присвоения перемещения

	// реализация чистых виртуальных методов
	void reCreateStructuredBuffer(ID3D11Device* device);
	void addModel(const CharactersModel& m);
	bool eraseModel(int index);
	void reVisible(int index); 
	void show(int index); 
	void hide(int index);
	bool isVisible(int index);
	void reVisibleBBox(int index);
	void showBBox(int index);
	void hideBBox(int index);
	bool isVisibleBBox(int index);
	CharactersModel getCharactersInstanse(int index);
	void draw(ID3D11DeviceContext * devcontx, D3D11_FILLMODE mode, Camera camera, const Matrix & Projection);
	bool init(ID3D11Device* device, CollectionTextures& collTextures, CollectionShaders& collShaders);
	void drawSelected(ID3D11DeviceContext * devcontx);

	// деструктор
	virtual ~ModelInstansingColor() {}
};

class ModelInstansingLight: public ModelInstansing
{
	vector<VertexNormal> vertex; // буфер, который будет отправляться на GPU
	ID3D11ShaderResourceView* textures; // массив текстур для шейдера(одинаковый размер!!!)
	vector<wstring> listTextures; // список путей к текстурам
	ID3D11SamplerState* samplerState; // формат текстур
	ConstantBufferDirectionalLightPS psb; // константный буфер для пиксельного шейдера(данные свет, как - то по другому сделать)
	ConstantBufferVSInstansing cb;

	void add_model(const CharactersModel& m); // общий метод добавления инстанса
	void constructor(char * versionShaderModel, const vector<wstring>& s, const CharactersModel & m, BoundingBoxData* bbd = nullptr); // скрытый конструктор
public:
	// конструкторы
	ModelInstansingLight(char* versionShaderModel,  // конструктор(из набора вершин и индексов), версия шейдеров
											const vector<VertexNormal>& v,  // вершины
											const vector<WORD>& ind,  // индексы
											const vector<wstring>& s, // список путей к шейдерам
											const vector<wstring>& text, // список путей к текстурам
											const CharactersModel& m, // позиция первого инстанса
											BoundingBoxData* bBoxData = nullptr // данные о bbox'е
											); // топология
	ModelInstansingLight(ModelInstansingLight&& m); // конструктор перемещения
	const ModelInstansingLight& operator=(ModelInstansingLight&& m); // конструктор присвоения перемещения

	// реализация чистых виртуальных методов
	void reCreateStructuredBuffer(ID3D11Device* device);
	void addModel(const CharactersModel& m);
	bool eraseModel(int index);
	void reVisible(int index);
	void show(int index);
	void hide(int index);
	bool isVisible(int index);
	void reVisibleBBox(int index);
	void showBBox(int index);
	void hideBBox(int index);
	bool isVisibleBBox(int index);
	CharactersModel getCharactersInstanse(int index);
	void draw(ID3D11DeviceContext * devcontx, D3D11_FILLMODE mode, Camera camera, const Matrix & Projection);
	bool init(ID3D11Device* device, CollectionTextures& collTextures, CollectionShaders& collShaders);
	void drawSelected(ID3D11DeviceContext * devcontx);

	// деструктор
	virtual ~ModelInstansingLight();
};


// подумать немного над переносом raypick на шейдера
// добавить список объектов
// добавить меню:
// ПКМ - скрыть, удалить, копировать, вставить, дублировать
// сверху - тоже самое.
// в списке объектов: тоже самое + показать
// проработать добавление объектов - инстансов
// оптимизация: внедрить Frustum Culling для всех типов моделей(на шейдерах)
// ModelDrawLight: в init передавать свет, в draw - устанавливать этот буфер света
// со светом еще подумать, есть много техник освещения
// добавить метод генерации сферы, и плоскости(плоскость для отображения сетки)


// Шаблонные методы

// BoundingBoxInstansing
template<class T>
inline void BoundingBoxInstansing::transformModel(vector<T>& v, const Matrix & m)
{
	Matrix transform = Matrix::CreateTranslationMatrixXYZ(-center[x], -center[y], -center[z]) * m * Matrix::CreateTranslationMatrixXYZ(center[x], center[y], center[z]);
	for (auto& vert : v)
	{
		Vector3 vertex = (Vector3)vert.Pos;
		vertex.vector3TransformCoord(transform);
		vert.Pos = vertex;
	}
}

// ModelInstansing
template<class T>
inline void ModelInstansing::transformModel(vector<T>&  v, const Matrix & m)
{
	auto center = modelBBox->getCenterBoundingBox(-1);
	Matrix transform = Matrix::CreateTranslationMatrixXYZ(-center[x], -center[y], -center[z]) * m * Matrix::CreateTranslationMatrixXYZ(center[x], center[y], center[z]);
	for (auto& vert: v)
	{
		Vector3 vertex = (Vector3)vert.Pos;
		vertex.vector3TransformCoord(transform);
		vert.Pos = vertex;
	}
}



