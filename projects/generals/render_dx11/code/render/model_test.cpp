#include "model_test.h"

LogicBuffer::LogicBuffer(Matrix v)
{
	this->position = v;
}




void PhysicBuffer::allocate(LogicBuffer lb)
{
	lb.position.transponse();
	this->bufferCPU.push_back(lb);
	this->size += sizeof(lb.position);
}

void PhysicBuffer::release(int index)
{
	if (!this->bufferCPU.size()) return;
	this->size -= sizeof(bufferCPU[0].position);
	this->bufferCPU.erase(this->bufferCPU.begin() + index);
}

void PhysicBuffer::rebuildPages(ID3D11Device* device)
{
	size_t vfStride = sizeof(bufferCPU[0].position);

	if (physicBuffer != nullptr)     physicBuffer->Release();
	if (physicShaderResourceView != nullptr) physicShaderResourceView->Release();

	D3D11_BUFFER_DESC bufferDesc;
	ZeroMemory(&bufferDesc, sizeof(bufferDesc));
	bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	bufferDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	bufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	bufferDesc.StructureByteStride = vfStride;
	bufferDesc.ByteWidth = this->bufferCPU.size() * vfStride;
	D3D11_SUBRESOURCE_DATA bufferData;
	ZeroMemory(&bufferData, sizeof(bufferData));
	bufferData.pSysMem = this->bufferCPU.data();
	HRESULT hr = device->CreateBuffer(&bufferDesc, &bufferData, &physicBuffer);
	
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	ZeroMemory(&srvDesc, sizeof(srvDesc));
	srvDesc.Format = DXGI_FORMAT_UNKNOWN;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
	srvDesc.Buffer.ElementWidth = this->bufferCPU.size();
	hr = device->CreateShaderResourceView(physicBuffer, &srvDesc, &physicShaderResourceView);

}









bool Model::Init(ID3D11Device* device, const vector<VertexColor>& v, const vector<WORD>& ind)
{
	ID3DBlob* pVSBlob = NULL; // Вспомогательный объект - просто место в оперативной памяти
	HRESULT hr = CompileShaderFromFile(LR"(..\..\..\engine_resource\shaders\shader.vs)", "main", "vs_5_0", &pVSBlob);
	if (FAILED(hr))
	{
		return hr;
	}
	hr = device->CreateVertexShader(pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), NULL, &VertexShader);
	if (FAILED(hr))
	{
		pVSBlob->Release();
		return hr;
	}

	ID3DBlob* pPSBlob = NULL;
	hr = CompileShaderFromFile(LR"(..\..\..\engine_resource\shaders\shader.ps)", "main", "ps_5_0", &pPSBlob);
	if (FAILED(hr))
	{
		return hr;
	}
	hr = device->CreatePixelShader(pPSBlob->GetBufferPointer(), pPSBlob->GetBufferSize(), NULL, &PixelShader);
	pPSBlob->Release();
	if (FAILED(hr)) return hr;

	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof D3D11_BUFFER_DESC);
	bd.ByteWidth = sizeof(ConstantBufferNoLight);
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	hr = device->CreateBuffer(&bd, NULL, &ConstantsBufferVS);
	if (hr != S_OK)
	{
		return false;
	}

	ZeroMemory(&bd, sizeof D3D11_BUFFER_DESC);
	bd.ByteWidth = sizeof(VertexColor) * v.size();
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	D3D11_SUBRESOURCE_DATA subd;
	ZeroMemory(&subd, sizeof(subd));
	subd.pSysMem = v.data();
	hr = device->CreateBuffer(&bd, &subd, &VertexBuffer);
	if (hr != S_OK)
	{
		return false;
	}

	ZeroMemory(&bd, sizeof D3D11_BUFFER_DESC);
	ZeroMemory(&subd, sizeof(subd));
	bd.ByteWidth = sizeof(WORD) * ind.size();
	this->count_index = ind.size();
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	subd.pSysMem = ind.data();
	hr = device->CreateBuffer(&bd, &subd, &IndexBuffer);
	if (hr != S_OK)
	{
		return false;
	}

	for (int i(0), z(0); i < 100; i++, z+= 5)
		for (int j(0), x(-250); j < 100; j++, x += 5)
			pBuffer.allocate(Matrix::CreateTranslationMatrixXYZ(x, 0, z));

	pBuffer.rebuildPages(device);


	this->size_byte_type_vertex = sizeof(VertexColor); // устанавливаем размер одной вершины

	D3D11_INPUT_ELEMENT_DESC ied[2] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,0,D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0,12,D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};
	hr = device->CreateInputLayout(ied, 2, pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), &VertexFormat);
	if (hr != S_OK)
	{
		return false;
	}

	return true;
}

bool Model::Draw(ID3D11DeviceContext * devcontx, Camera camera, const Matrix & Projection)
{
	ConstantBufferNoLight cb;
	Matrix _final; _final.identity();

	cb.World = Matrix::Transponse(_final);
	cb.View = Matrix::Transponse(camera.getView());
	cb.Projection = Matrix::Transponse(Projection);

	devcontx->UpdateSubresource(ConstantsBufferVS, 0, NULL, &cb, 0, 0);
	devcontx->VSSetConstantBuffers(0, 1, &ConstantsBufferVS);

	devcontx->VSSetShaderResources(0, 1, &pBuffer.physicShaderResourceView);

	devcontx->IASetInputLayout(VertexFormat);
	devcontx->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	devcontx->VSSetShader(VertexShader, NULL, NULL);
	devcontx->PSSetShader(PixelShader, NULL, NULL);

	UINT start = 0;
	devcontx->IASetIndexBuffer(IndexBuffer, DXGI_FORMAT_R16_UINT, 0);
	devcontx->IASetVertexBuffers(NULL, 1, &VertexBuffer, &size_byte_type_vertex, &start);

	devcontx->DrawIndexedInstanced(this->count_index, pBuffer.bufferCPU.size(), 0, 0, 0);
	// devcontx->DrawIndexed(this->count_index, 0, 0);

	return true;
}

HRESULT Model::CompileShaderFromFile(WCHAR* szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut)
{
	HRESULT hr = S_OK;
	DWORD dwShaderFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
	ID3DBlob* pErrorBlob;
	hr = D3DCompileFromFile(szFileName, NULL, NULL, szEntryPoint, szShaderModel,
		dwShaderFlags, 0, ppBlobOut, &pErrorBlob);
	if (FAILED(hr))
	{
		if (pErrorBlob != NULL)
			OutputDebugStringA((char*)pErrorBlob->GetBufferPointer());
		if (pErrorBlob) pErrorBlob->Release();
		return hr;
	}
	if (pErrorBlob) pErrorBlob->Release();
	return S_OK;
}
