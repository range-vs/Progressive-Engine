#include "fustrum_culling.h"

bool FrustumCulling::reCreateStructuredBuffers(ID3D11Device * device)
{
	if (!recreateStructuredBuffer(device, matrix, &physicBufferMatrix, &physicShaderResourceViewMatrix))
		return false;
	if (!recreateStructuredBuffer(device, offset, &physicBufferOffset, &physicShaderResourceViewOffset))
		return false;
	if (!recreateStructuredBuffer(device, vertex, &physicBufferModels, &physicShaderResourceViewModels))
		return false;
	if (!recreateStructuredBuffer(device, centers, &physicBufferCenters, &physicShaderResourceViewCenters))
		return false;

	isReCreate = false;
	return true;
}


bool FrustumCulling::updateBuffers(ID3D11Device* device)
{
	// clear buff
	if (outFlags)
		outFlags->Release();
	if (physicBufferFlags)
		physicBufferFlags->Release();
	if (physicShaderResourceViewFlags)
		physicShaderResourceViewFlags->Release();

	// пересоздаем выходной буфер
	// buffer for copy data
	D3D11_BUFFER_DESC outputDesc;
	outputDesc.Usage = D3D11_USAGE_STAGING;
	outputDesc.BindFlags = 0;
	outputDesc.ByteWidth = sizeof(int) * countMatrix;
	outputDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
	outputDesc.StructureByteStride = sizeof(int);
	outputDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	HRESULT hr = device->CreateBuffer(&outputDesc, 0, &outFlags);
	if (hr != S_OK)
		return false;

	// create rw structured buffer
	D3D11_BUFFER_DESC descGPUBuffer;
	ZeroMemory(&descGPUBuffer, sizeof(descGPUBuffer));
	descGPUBuffer.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
	descGPUBuffer.ByteWidth = sizeof(int) * countMatrix;
	descGPUBuffer.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	descGPUBuffer.StructureByteStride = sizeof(int);
	hr = device->CreateBuffer(&descGPUBuffer, NULL, &physicBufferFlags);
	if (hr != S_OK)
		return false;

	// create rw structured buffer UAV 
	D3D11_BUFFER_DESC descBuf;
	ZeroMemory(&descBuf, sizeof(descBuf));
	physicBufferFlags->GetDesc(&descBuf);
	D3D11_UNORDERED_ACCESS_VIEW_DESC descView;
	ZeroMemory(&descView, sizeof(descView));
	descView.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
	descView.Buffer.FirstElement = 0;
	descView.Format = DXGI_FORMAT_UNKNOWN;
	descView.Buffer.NumElements = countMatrix;
	hr = device->CreateUnorderedAccessView(physicBufferFlags, &descView, &physicShaderResourceViewFlags);
	if (hr != S_OK)
		return false;
	return true;
}

void FrustumCulling::addModelInstansingFake()
{
	data.countModels++;
	offsetCurrent.push_back(0);
	offset.push_back(0);
	isReCreate = true;
}

void FrustumCulling::addModel(const vector<Vertex>& bbox, AllTransformations & m, const Point& cen)
{
	vector<Point> * p = &this->vertex;
	for_each(bbox.begin(), bbox.end(), [p](const Vertex& vert) {
		p->push_back(vert.Pos);
	});
	m = allTransponse(m);
	matrix.push_back(m);
	if (!data.countModels)
		offsetCurrent.push_back(0);
	else
		offsetCurrent.push_back(*(offsetCurrent.end()-1));
	++data.countModels;
	++countMatrix;
	offset.push_back(1);
	centers.push_back(cen);
	isReCreate = true;
}

bool FrustumCulling::addModelTransformation(int indexModel, AllTransformations & m)
{
	if (indexModel < 0 || indexModel >= data.countModels)
		return false;
	offset[indexModel]++;
	++countMatrix;
	m = allTransponse(m);
	if (offsetCurrent[indexModel] + offset[indexModel] >= matrix.size())
		matrix.push_back(m);
	else
		matrix.insert(matrix.begin() + offsetCurrent[indexModel], m);
	isReCreate = true;
	return true;
}

bool FrustumCulling::eraseModel(int indexModel, int indexTransformation)
{
	if ((indexModel < 0 || indexModel >= data.countModels) || offset[indexModel] == 0 || (indexTransformation < 0 || indexTransformation >= offset[indexModel]))
		return false;
	matrix.erase(matrix.begin() + offsetCurrent[indexModel] + indexTransformation);
	offset[indexModel]--;
	--countMatrix;
	isReCreate = true;
	return true;
}

bool FrustumCulling::updateModel(int indexModel, int indexTransformation,const vector<Vertex>& bbox, const Point& cen)
{
	if ((indexModel < 0 || indexModel >= data.countModels) || offset[indexModel] == 0 || (indexTransformation < 0 || indexTransformation >= offset[indexModel]))
		return false;
	for (int i(0); i < bbox.size(); i++)
		vertex[8 * indexModel + i] = bbox[i].Pos;
	centers[indexModel] = cen;
	isReCreate = true;
	return true;
}

bool FrustumCulling::updateModel(int indexModel, int indexTransformation, AllTransformations& m)
{
	if ((indexModel < 0 || indexModel >= data.countModels) || offset[indexModel] == 0 || (indexTransformation < 0 || indexTransformation >= offset[indexModel]))
		return false;
	m = allTransponse(m);
	matrix[offsetCurrent[indexModel] + indexTransformation] = m;
	isReCreate = true;
	return true;
}

vector<int> FrustumCulling::getIndexForDrawing()
{
	return this->flags;
}

FrustumCulling::~FrustumCulling()
{
	if (physicBufferMatrix)
		physicBufferMatrix->Release();
	if (physicShaderResourceViewMatrix)
		physicShaderResourceViewMatrix->Release();
	if (physicBufferFlags)
		physicBufferFlags->Release();
	if (physicShaderResourceViewFlags)
		physicShaderResourceViewFlags->Release();
	if (outFlags)
		outFlags->Release();
	if (physicBufferOffset)
		physicBufferOffset->Release();
	if (physicShaderResourceViewOffset)
		physicShaderResourceViewOffset->Release();
	if (physicBufferModels)
		physicBufferModels->Release();
	if (physicShaderResourceViewModels)
		physicShaderResourceViewModels->Release();
	if (shader)
		shader->Release();
	if (constantBufferData)
		constantBufferData->Release();
	if (physicShaderResourceViewCenters)
		physicShaderResourceViewCenters->Release();
	if (physicBufferCenters)
		physicBufferCenters->Release();
}

//bool FrustumCulling::update(ID3D11Device* device, const Matrix & view, const Matrix& proj, float screenDepth)
//{
//	if (data.countModels == 0 || countMatrix == 0)
//		return true;
//	// TODO: передать сюда вьюпорт, создать по нему матрицу и сравнить её с получившейся
//	//Matrix v(Matrix::Transponse(view)), projection(Matrix::Transponse(proj));
//	Matrix v(view), projection(proj);
//	float zmin = -projection(3, 2) / projection(2, 2);
//	float q = screenDepth / (screenDepth - zmin);
//	projection(2, 2) = q;
//	projection(3, 2) = -q * zmin;
//	Matrix fMatrix = v * projection;
//
//	// генерируем шесть плоскостей фрустума
//	/*Plane3 pNear;  
//	pNear.setNormal(Vector3(fMatrix(0, 3) + fMatrix(0,2), fMatrix(1,3) + fMatrix(1, 2), fMatrix(2, 3) + fMatrix(2, 2)));
//	pNear.setD(fMatrix(3, 3) + fMatrix(3, 2));
//	pNear.normalize();
//	data.planes[0] = PlanesFrustum(pNear.getNormal(), pNear.getD());
//
//	Plane3 pFar;
//	pFar.setNormal(Vector3(fMatrix(0, 3) - fMatrix(0, 2), fMatrix(1, 3) - fMatrix(1, 2), fMatrix(2, 3) - fMatrix(2, 2)));
//	pFar.setD(fMatrix(3, 3) - fMatrix(3, 2));
//	pFar.normalize();
//	data.planes[1] = PlanesFrustum(pFar.getNormal(), pFar.getD());
//
//	Plane3 pLeft;
//	pLeft.setNormal(Vector3(fMatrix(0, 3) + fMatrix(0, 0), fMatrix(1, 3) + fMatrix(1, 0), fMatrix(2, 3) + fMatrix(2, 0)));
//	pLeft.setD(fMatrix(3, 3) + fMatrix(3, 0));
//	pLeft.normalize();
//	data.planes[2] = PlanesFrustum(pLeft.getNormal(), pLeft.getD());
//
//	Plane3 pRight;
//	pRight.setNormal(Vector3(fMatrix(0, 3) - fMatrix(0, 0), fMatrix(1, 3) - fMatrix(1, 0), fMatrix(2, 3) - fMatrix(2, 0)));
//	pRight.setD(fMatrix(3, 3) - fMatrix(3, 0));
//	pRight.normalize();
//	data.planes[3] = PlanesFrustum(pRight.getNormal(), pRight.getD());
//
//	Plane3 pTop;
//	pTop.setNormal(Vector3(fMatrix(0, 3) - fMatrix(0, 1), fMatrix(1, 3) - fMatrix(1, 1), fMatrix(2, 3) - fMatrix(2, 1)));
//	pTop.setD(fMatrix(3, 3) - fMatrix(3, 1));
//	pTop.normalize();
//	data.planes[4] = PlanesFrustum(pTop.getNormal(), pTop.getD());
//
//	Plane3 pBottom;
//	pBottom.setNormal(Vector3(fMatrix(0, 3) + fMatrix(0, 1), fMatrix(1, 3) + fMatrix(1, 1), fMatrix(2, 3) + fMatrix(2, 1)));
//	pBottom.setD(fMatrix(3, 3) + fMatrix(3, 1));
//	pBottom.normalize();
//	data.planes[5] = PlanesFrustum(pBottom.getNormal(), pBottom.getD());
//	*/
//
//	// debug, другой метод построения плоскостей
//	// перед
//	data.planes[0][x] = fMatrix(0, 3) + fMatrix(0, 2);
//	data.planes[0][y] = fMatrix(1, 3) + fMatrix(1, 2);
//	data.planes[0][z] = fMatrix(2, 3) + fMatrix(2, 2);
//	data.planes[0][w] = fMatrix(3, 3) + fMatrix(3, 2);
//	data.planes[0] = normalizePlane(data.planes[0]);
//	
//	// зад
//	data.planes[1][x] = fMatrix(0, 3) - fMatrix(0, 3);
//	data.planes[1][y] = fMatrix(1, 3) - fMatrix(1, 2);
//	data.planes[1][z] = fMatrix(2, 3) - fMatrix(2, 2);
//	data.planes[1][w] =  fMatrix(3, 3) - fMatrix(3, 2);
//	data.planes[1] = normalizePlane(data.planes[1]);
//
//	// лево
//	data.planes[2][x] = fMatrix(0, 3) + fMatrix(0, 0);
//	data.planes[2][y] = fMatrix(1, 3) + fMatrix(1, 0);
//	data.planes[2][z] = fMatrix(2, 3) + fMatrix(2, 0);
//	data.planes[2][w] = fMatrix(3, 3) + fMatrix(3, 0); 
//	data.planes[2] = normalizePlane(data.planes[2]);
//
//	// право
//	data.planes[3][x] = fMatrix(0, 3) - fMatrix(0, 0);
//	data.planes[3][y] = fMatrix(1, 3) - fMatrix(1, 0);
//	data.planes[3][z] = fMatrix(2, 3) - fMatrix(2, 0);
//	data.planes[3][w] = fMatrix(3, 3) - fMatrix(3, 0); 
//	data.planes[3] = normalizePlane(data.planes[3]);
//
//	// верх
//	data.planes[4][x] = fMatrix(0, 3) - fMatrix(0, 1);
//	data.planes[4][y] = fMatrix(1, 3) - fMatrix(1, 1);
//	data.planes[4][z] = fMatrix(2, 3) - fMatrix(2, 1);
//	data.planes[4][w] = fMatrix(3, 3) - fMatrix(3, 1);
//	data.planes[4] = normalizePlane(data.planes[4]);
//
//	// низ
//	data.planes[5][x] = fMatrix(0, 3) + fMatrix(0, 1);
//	data.planes[5][y] = fMatrix(1, 3) + fMatrix(1, 1);
//	data.planes[5][z] = fMatrix(2, 3) + fMatrix(2, 1);
//	data.planes[5][w] = fMatrix(3, 3) + fMatrix(3, 1);
//	data.planes[5] = normalizePlane(data.planes[5]);
//
//	if (isReCreate)
//		if (!this->reCreateStructuredBuffers(device))
//			return false;
//	return this->updateBuffers(device);
//}

bool FrustumCulling::update(ID3D11Device* device, const Camera & view, float angle, float ratio, float _near, float rear)
{
	if (data.countModels == 0 || countMatrix == 0)
		return true;
	
	// заготовки для получения 8 вершин фрустума
	Vector3 pos = (Vector3)view.getViewPosition();
	Vector3 d = pos - (Vector3)view.getViewDirection();
	d.normalize();
	Vector3 right = (Vector3)view.getViewHeight();
	right = right.cross(d);
	right.normalize();
	Vector3 up = Vector3::cross(d, right);
	// получаем высоту и ширину передней и задней плоскостей
	float val = angle * 0.5f;
	float tang = (float)tan(val);
	float nh = _near * tang; // перед высота
	float nw = nh * ratio; // ширина
	float rh = rear * tang; // зад высота
	float rw = rh * ratio; // ширина
	// генерируем 8 вершин фрустума
	Vector3 fc = pos - d * rear; // центр задней
	Vector3 ftl = fc + up * rh - right * rw; // задняя левая верхняя
	Vector3 ftr = fc + up * rh + right * rw; // задняя правая верхняя
	Vector3 fbl = fc - up * rh - right * rw; // задняя левая нижняя
	Vector3 fbr = fc - up * rh + right * rw; // задняя правая нижняя
	Vector3 nc = pos - d * _near; // центр передней
	data.centerNear = (Vector)nc;
	Vector3 ntl = nc + up * nh - right * nw; // передняя левая верхняя
	Vector3 ntr = nc + up * nh + right * nw; // передняя правая верхняя
	Vector3 nbl = nc - up * nh - right * nw; // передняя левая нижняя
	Vector3 nbr = nc - up * nh + right * nw; // передняя правая нижняя

	//debug
	v.clear();
	ind.clear();
	v.push_back(VertexColor(nbl, Color(0,1,0,1))); //0
	v.push_back(VertexColor(ntl, Color(0, 1, 0, 1))); //1
	v.push_back(VertexColor(ntr, Color(0, 1, 0, 1))); //2
	v.push_back(VertexColor(nbr, Color(0, 1, 0, 1))); //3
	v.push_back(VertexColor(fbr, Color(0, 1, 0, 1))); //4
	v.push_back(VertexColor(ftr, Color(0, 1, 0, 1))); //5
	v.push_back(VertexColor(ftl, Color(0, 1, 0, 1))); //6
	v.push_back(VertexColor(fbl, Color(0, 1, 0, 1))); //7
	ind.push_back(5); ind.push_back(6); ind.push_back(1);
	ind.push_back(1); ind.push_back(6); ind.push_back(7);
	ind.push_back(4); ind.push_back(5); ind.push_back(2);
	ind.push_back(7); ind.push_back(4); ind.push_back(0);


	// генерируем по точкам массив из 6 плоскостей
	// верх
	Plane3 p(ntr, ntl, ftl);
	data.planes[TOP] = PlanesFrustum(p.getPlane());
	// низ
	p = Plane3(nbl, nbr, fbr);
	data.planes[BOTTOM] = PlanesFrustum(p.getPlane());
	// лево
	p = Plane3(ntl, nbl, fbl);
	data.planes[LEFT] = PlanesFrustum(p.getPlane());
	// право
	p = Plane3(nbr, ntr, fbr);
	data.planes[RIGHT] = PlanesFrustum(p.getPlane());
	// зад
	p = Plane3(ftr, ftl, fbl);
	data.planes[FARP] = PlanesFrustum(p.getPlane());
	//перед
	p = Plane3(ntl, ntr, nbr);
	data.planes[NEARP] = PlanesFrustum(p.getPlane());

	/*Vector3 h = pos + (up  * 3.f);
	Vector3 r = pos + (right * 3.f);
	p = Plane3(pos, h, r);
	data.planes[NEARP] = PlanesFrustum(p.getPlane());*/

	// TODO: ожидать ответа с stack game developers

	if (isReCreate)
		if (!this->reCreateStructuredBuffers(device))
			return false;
	return this->updateBuffers(device);
}


bool FrustumCulling::init(ID3D11Device * device, const wstring & path, CollectionShaders & collShaders)
{
	// получаем шейдер
	pathToShader = path;
	shader = collShaders.getShaderCompute(pathToShader);
	if (!shader)
		return false;

	// create constant buffer
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof D3D11_BUFFER_DESC);
	bd.ByteWidth = sizeof(FustrumData);
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	HRESULT hr = device->CreateBuffer(&bd, NULL, &constantBufferData);
	if (hr != S_OK)
	{
		//LOGMESSAGE("Error create constant buffer!");
		return false;
	}

	return true;
}

bool FrustumCulling::run(ID3D11Device * device, ID3D11DeviceContext * deviceContext)
{
	if (data.countModels == 0 || countMatrix == 0)
		return true;
	// заглушки
	ID3D11UnorderedAccessView* nullOutBuffer(nullptr);
	ID3D11ShaderResourceView* nullResourceBuffer(nullptr);
	ID3D11Buffer* nullConstantBuffer(nullptr);

	// устанавливаем вычислительный шейдер в контекст
	deviceContext->CSSetShader(shader, NULL, 0);
	// устанавливаем константные буферы
	deviceContext->UpdateSubresource(constantBufferData, 0, NULL, &data, 0, 0); // обновляем константный буфер
	deviceContext->CSSetConstantBuffers(0, 1, &constantBufferData); // устанавливаем константный буфер
													
    // устанавливаем структурные буферы
	deviceContext->CSSetShaderResources(0, 1, &physicShaderResourceViewMatrix);
	deviceContext->CSSetShaderResources(1, 1, &physicShaderResourceViewOffset);
	deviceContext->CSSetShaderResources(2, 1, &physicShaderResourceViewModels);
	deviceContext->CSSetShaderResources(3, 1, &physicShaderResourceViewCenters);
	// устанавливаем выходной буфер
	deviceContext->CSSetUnorderedAccessViews(0, 1, &physicShaderResourceViewFlags, NULL);

	// вызов вычислительного шейдера
	deviceContext->Dispatch(1, 1, 1);

	// сбрасываем установленные буферы(есть нужно) + шейдер
	deviceContext->CSSetUnorderedAccessViews(0, 1, &nullOutBuffer, NULL);
	deviceContext->CSSetConstantBuffers(0, 1, &nullConstantBuffer);
	deviceContext->CSSetShaderResources(0, 1, &nullResourceBuffer);
	deviceContext->CSSetShaderResources(1, 1, &nullResourceBuffer);
	deviceContext->CSSetShaderResources(2, 1, &nullResourceBuffer);
	deviceContext->CSSetShaderResources(3, 1, &nullResourceBuffer);
	deviceContext->CSSetShader(NULL, NULL, 0);

	// скопировать данные из видеокарты на CPU
	deviceContext->CopyResource(outFlags, physicBufferFlags);
	D3D11_MAPPED_SUBRESOURCE mappedData;
	ZeroMemory(&mappedData, sizeof(mappedData));
	HRESULT hr = deviceContext->Map(outFlags, 0, D3D11_MAP_READ, 0, &mappedData); // утройство удаляется - при огромных данных надо делить на потоки
	hr = device->GetDeviceRemovedReason();
	auto fLevel = device->GetFeatureLevel();
	string message;
	switch (hr)
	{
	case DXGI_ERROR_DEVICE_HUNG:
		message = "DXGI_ERROR_DEVICE_HUNG";
		break;

	case DXGI_ERROR_DEVICE_REMOVED:
		message = "DXGI_ERROR_DEVICE_REMOVED";
		break;

	case DXGI_ERROR_DEVICE_RESET:
		message = "DXGI_ERROR_DEVICE_RESET";
		break;

	case DXGI_ERROR_DRIVER_INTERNAL_ERROR:
		message = "DXGI_ERROR_DRIVER_INTERNAL_ERROR";
		break;

	case DXGI_ERROR_INVALID_CALL:
		message = "DXGI_ERROR_INVALID_CALL";
		break;

	case S_OK:
		message = "S_OK";
		break;

	default:
		message = "error";
		break;
	}


	if (hr != S_OK)
	{
		// описывам еррор!
		return false;
	}

	// TODO: заменить clear() на более производительное решение
	flags.clear();
	uint8_t* dataPtr = reinterpret_cast<uint8_t*>(mappedData.pData);
	size_t s = sizeof(int);
	for (size_t i(0); i < countMatrix; ++i)
	{
		int *tmp(nullptr);
		tmp = (int*)dataPtr;
		dataPtr += s;
		flags.push_back(*tmp);
	}
	deviceContext->Unmap(outFlags, 0);

	return true;
}

AllTransformations FrustumCulling::allTransponse(AllTransformations m)
{
	m.position.transponse();
	m.rotation.transponse();
	m.scale.transponse();
	return m;
}
