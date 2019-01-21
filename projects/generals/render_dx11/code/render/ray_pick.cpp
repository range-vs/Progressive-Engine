#include "ray_pick.h"

void RayPick::addModelTransformation(int index, AllTransformations& m)
{
	m.position.transponse();
	m.rotation.transponse();
	m.scale.transponse();
	transformations[index].push_back(m);
	++offsets[index].countTransformations;
	countTransformationAll++;
	isReCreate = true;
}

bool RayPick::eraseModel(int index, int indexTransformation)
{
	if ((index < 0 || index > points.size()) || offsets[index].countTransformations == 0)
		return false;
	/*if (indexTransformation == -1) // обдумать, есть ли смысл удалять модели
	{
		countVertexAll -= offsets[index].countVertex;
		countIndecedAll -= offsets[index].countIndeceds;
		points.erase(points.begin() + offsets[index].countVertexAllPred, points.begin() + offsets[index].countVertex);
		indeceds.erase(indeceds.begin() + offsets[index].countIndecedsAllPred, indeceds.begin() + offsets[index].countIndeceds);
		offsets.erase(offsets.begin() + index);
	}
	else*/
	transformations[index].erase(transformations[index].begin() + indexTransformation);
	--offsets[index].countTransformations;
	countTransformationAll--;
	isReCreate = true;
	return true;
}

bool RayPick::reCreateStructuredBuffers(ID3D11Device* device)
{
	if (!recreateStructuredBuffer(device, points, &physicBufferPoints, &physicShaderResourceViewPoints))
		return false;
	if (!recreateStructuredBuffer(device, bbox, &physicBufferBBoxPoints, &physicShaderResourceViewBBoxPoints))
		return false;
	if (!recreateStructuredBuffer(device, indeceds, &physicBufferIndeceds, &physicShaderResourceViewIndeceds))
		return false;
	if (!recreateStructuredBuffer(device, offsets, &physicBufferOffsets, &physicShaderResourceViewOffsets))
		return false;
	if (!reCreateStructuredBufferTransformations(device))
		return false;

	isReCreate = false;
	return true;
}

bool RayPick::reCreateStructuredBufferTransformations(ID3D11Device* device)
{
	size_t vfStride = sizeof(AllTransformations);
	size_t numElements = countTransformationAll;

	if (physicBufferTransformation != nullptr)     physicBufferTransformation->Release();
	if (physicShaderResourceViewTransformation != nullptr) physicShaderResourceViewTransformation->Release();

	D3D11_BUFFER_DESC bufferDesc;
	ZeroMemory(&bufferDesc, sizeof(bufferDesc));
	bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	bufferDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	bufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	bufferDesc.StructureByteStride = (UINT)vfStride;
	bufferDesc.ByteWidth = (UINT)(numElements * vfStride);
	HRESULT hr = device->CreateBuffer(&bufferDesc, nullptr, &physicBufferTransformation);
	if (hr != S_OK)
		return false;

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	ZeroMemory(&srvDesc, sizeof(srvDesc));
	srvDesc.Format = DXGI_FORMAT_UNKNOWN;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
	srvDesc.Buffer.ElementWidth = (UINT)numElements;
	hr = device->CreateShaderResourceView(physicBufferTransformation, &srvDesc, &physicShaderResourceViewTransformation);
	if (hr != S_OK)
		return false;


	D3D11_MAPPED_SUBRESOURCE mappedData;
	ZeroMemory(&mappedData, sizeof(mappedData));
	ID3D11DeviceContext* g_pImmediateContext;
	device->GetImmediateContext(&g_pImmediateContext);
	hr = g_pImmediateContext->Map(physicBufferTransformation, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedData);
	if (hr != S_OK)
		return false;

	uint8_t* dataPtr = reinterpret_cast<uint8_t*>(mappedData.pData);
	size_t pageOffset = 0;
	for (size_t i(0); i < transformations.size(); ++i) 
	{
		for (size_t j(0); j < transformations[i].size(); j++)
		{
			std::memcpy(dataPtr + pageOffset, (void*)&transformations[i][j], vfStride);
			pageOffset += vfStride;
		}
	}
	g_pImmediateContext->Unmap(physicBufferTransformation, 0);

	return true;

}

void RayPick::update(const Point & cameraPosition, const Point & rayPosition, const Point & rayDirection)
{
	BufferData.cameraPosition = (Vector)cameraPosition;
	BufferData.countModel[x] = (float)offsets.size();
	BufferData.rayDirection = (Vector)rayDirection;
	BufferData.rayPosition = (Vector)rayPosition;
}

bool RayPick::init(ID3D11Device* device, const wstring& path, CollectionShaders& collShaders)
{
	// получаем шейдер
	pathToShader = path;
	shader = collShaders.getShaderCompute(pathToShader);
	if (!shader)
		return false;

	// buffer for copy data
	D3D11_BUFFER_DESC outputDesc;
	outputDesc.Usage = D3D11_USAGE_STAGING;
	outputDesc.BindFlags = 0;
	outputDesc.ByteWidth = sizeof(DataOutput);
	outputDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
	outputDesc.StructureByteStride = sizeof(DataOutput);
	outputDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	HRESULT hr = device->CreateBuffer(&outputDesc, 0, &outBuffer);
	if (hr != S_OK)
		return false;

	// create rw structured buffer
	D3D11_BUFFER_DESC descGPUBuffer;
	ZeroMemory(&descGPUBuffer, sizeof(descGPUBuffer));
	descGPUBuffer.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
	descGPUBuffer.ByteWidth = sizeof(DataOutput);
	descGPUBuffer.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	descGPUBuffer.StructureByteStride = sizeof(DataOutput);
	hr = device->CreateBuffer(&descGPUBuffer, NULL, &physicBufferOut);
	if(hr != S_OK)
		return false;

	// create rw structured buffer UAV 
	D3D11_BUFFER_DESC descBuf;
	ZeroMemory(&descBuf, sizeof(descBuf));
	physicBufferOut->GetDesc(&descBuf);
	D3D11_UNORDERED_ACCESS_VIEW_DESC descView;
	ZeroMemory(&descView, sizeof(descView));
	descView.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
	descView.Buffer.FirstElement = 0;
	descView.Format = DXGI_FORMAT_UNKNOWN;
	descView.Buffer.NumElements = 1;
	hr = device->CreateUnorderedAccessView(physicBufferOut, &descView, &physicShaderResourceViewOut);
	if(hr != S_OK)
		return false;

	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof D3D11_BUFFER_DESC);
	bd.ByteWidth = sizeof(CommonData);
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	hr = device->CreateBuffer(&bd, NULL, &constantBufferData);
	if (hr != S_OK)
	{
		//LOGMESSAGE("Error create constant buffer!");
		return false;
	}

	return true;
}

DataOutput RayPick::run(ID3D11Device* device, ID3D11DeviceContext * deviceContext)
{
	// заглушки
	ID3D11UnorderedAccessView* nullOutBuffer(nullptr);
	ID3D11ShaderResourceView* nullResourceBuffer(nullptr);
	ID3D11Buffer* nullConstantBuffer(nullptr);
	// выходной файл
	DataOutput out; out.indexModel = out.indexTransformation = -1;
	// устанавливаем вычислительный шейдер в контекст
	deviceContext->CSSetShader(shader, NULL, 0);
	// устанавливаем константные буферы
	deviceContext->CSSetConstantBuffers(0, 1, &constantBufferData); // устанавливаем константный буфер
	deviceContext->UpdateSubresource(constantBufferData, 0, NULL, &BufferData, 0, 0); // обновляем константный буфер
	// устанавливаем структурные буферы
	deviceContext->CSSetShaderResources(0, 1, &physicShaderResourceViewPoints);
	deviceContext->CSSetShaderResources(1, 1, &physicShaderResourceViewIndeceds);
	deviceContext->CSSetShaderResources(2, 1, &physicShaderResourceViewTransformation);
	deviceContext->CSSetShaderResources(3, 1, &physicShaderResourceViewOffsets);
	deviceContext->CSSetShaderResources(4, 1, &physicShaderResourceViewBBoxPoints);
	// устанавливаем выходной буфер
	deviceContext->CSSetUnorderedAccessViews(0, 1, &physicShaderResourceViewOut, NULL);

	// вызов вычислительного шейдера
	deviceContext->Dispatch(1, 1, 1);

	// сбрасываем установленные буферы(есть нужно) + шейдер
	deviceContext->CSSetUnorderedAccessViews(0, 1, &nullOutBuffer, NULL);
	deviceContext->CSSetConstantBuffers(0, 1, &nullConstantBuffer);
	deviceContext->CSSetShaderResources(0, 1, &nullResourceBuffer);
	deviceContext->CSSetShaderResources(1, 1, &nullResourceBuffer);
	deviceContext->CSSetShaderResources(2, 1, &nullResourceBuffer);
	deviceContext->CSSetShaderResources(3, 1, &nullResourceBuffer);
	deviceContext->CSSetShaderResources(4, 1, &nullResourceBuffer);
	deviceContext->CSSetShader(NULL, NULL, 0);

	// скопировать данные из видеокарты на CPU
	deviceContext->CopyResource(outBuffer, physicBufferOut);
	D3D11_MAPPED_SUBRESOURCE mappedData;
	ZeroMemory(&mappedData, sizeof(mappedData));
	HRESULT hr = deviceContext->Map(outBuffer, 0, D3D11_MAP_READ, 0, &mappedData); // утройство удаляется - при огромных данных надо делить на потоки
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
		return out;
	}
	DataOutput* dataPtr = reinterpret_cast<DataOutput*>(mappedData.pData);
	out = *dataPtr;
	deviceContext->Unmap(outBuffer, 0);
	//MessageBox(NULL, (std::to_wstring(out.indexModel) + L" " + std::to_wstring(out.indexTransformation)).c_str(), L"", MB_OK);

	return out;
}

RayPick::~RayPick()
{
	if (physicBufferPoints != nullptr)     physicBufferPoints->Release();
	if (physicShaderResourceViewPoints != nullptr) physicShaderResourceViewPoints->Release();
	if (physicBufferBBoxPoints != nullptr) physicBufferBBoxPoints->Release();
	if (physicShaderResourceViewBBoxPoints != nullptr) physicShaderResourceViewBBoxPoints->Release();
	if (physicBufferIndeceds != nullptr)     physicBufferIndeceds->Release();
	if (physicShaderResourceViewIndeceds != nullptr) physicShaderResourceViewIndeceds->Release();
	if (physicBufferTransformation != nullptr)     physicBufferTransformation->Release();
	if (physicShaderResourceViewTransformation != nullptr) physicShaderResourceViewTransformation->Release();
	if (physicBufferOffsets != nullptr)     physicBufferOffsets->Release();
	if (physicShaderResourceViewOffsets != nullptr) physicShaderResourceViewOffsets->Release();
	if (physicBufferOut != nullptr)     physicBufferOut->Release();
	if (physicShaderResourceViewOut != nullptr) physicShaderResourceViewOut->Release();
	if (shader != nullptr) shader->Release();
	if (constantBufferData != nullptr) constantBufferData->Release();
}
