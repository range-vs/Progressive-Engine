#pragma once

#include "../utilites/types.h"

// логический буфер
template<class T> struct LogicBuffer
{
	T data;

	// конструкторы
	LogicBuffer(const T& v);
	LogicBuffer(LogicBuffer&& v);
	const LogicBuffer& operator=(LogicBuffer&& v);
};

template<class T>
inline LogicBuffer<T>::LogicBuffer(const T & d):data(d)
{
}

template<class T>
inline LogicBuffer<T>::LogicBuffer(LogicBuffer && v)
{
	data = std::move(v.data);
}

template<class T>
inline const LogicBuffer<T> & LogicBuffer<T>::operator=(LogicBuffer<T> && v)
{
	if (this != &v)
		data = std::move(v.data);
	return *this;
}

// физический буфер
template<class T> struct PhysicBuffer
{
	ID3D11Buffer* physicBuffer;
	ID3D11ShaderResourceView* physicShaderResourceView;
	size_t size;
	bool isReCreate; // флаг, нужно ли пересоздавать буфер

	std::vector<LogicBuffer<T>> bufferCPU;

	// конструкторы
	PhysicBuffer() = default;
	PhysicBuffer(PhysicBuffer&& pb);
	const PhysicBuffer<T>& operator=(PhysicBuffer<T>&& pb);

	void allocate(LogicBuffer<T>&& lb);
	void release(int index);
	void rebuildPages(ID3D11Device* device);

	// деструкторы
	~PhysicBuffer();
};

template<class T>
inline PhysicBuffer<T>::PhysicBuffer(PhysicBuffer && pb)
{
	physicBuffer = pb.physicBuffer; pb.physicBuffer = nullptr;
	physicShaderResourceView = pb.physicShaderResourceView; pb.physicShaderResourceView = nullptr;
	size = pb.size;
	bufferCPU = std::move(pb.bufferCPU);
}

template<class T>
inline const PhysicBuffer<T> & PhysicBuffer<T>::operator=(PhysicBuffer<T> && pb)
{
	if (this != &pb)
	{
		physicBuffer = pb.physicBuffer; pb.physicBuffer = nullptr;
		physicShaderResourceView = pb.physicShaderResourceView; pb.physicShaderResourceView = nullptr;
		size = pb.size;
		bufferCPU = std::move(pb.bufferCPU);
	}
	return *this;
}

template<class T>
void PhysicBuffer<T>::allocate(LogicBuffer<T>&& lb)
{
	this->bufferCPU.push_back(std::move(lb));
	this->size += sizeof(lb.data);
	this->isReCreate = true;
}

template<class T>
void PhysicBuffer<T>::release(int index)
{
	if (!this->bufferCPU.size()) return;
	this->size -= sizeof((*bufferCPU.begin()).data);
	this->bufferCPU.erase(this->bufferCPU.begin() + index);
	this->isReCreate = true;
}

template<class T>
void PhysicBuffer<T>::rebuildPages(ID3D11Device* device)
{
	if (bufferCPU.empty())
		return;
	if (!this->isReCreate)
		return;
	this->isReCreate = true;
	size_t vfStride = sizeof(bufferCPU[0].data);
	if (vfStride % 4 != 0)
	{
		if (vfStride < 4)
			vfStride = 4;
		else
			vfStride += (vfStride / 4);
	}

	if (physicBuffer != nullptr)     physicBuffer->Release();
	if (physicShaderResourceView != nullptr) physicShaderResourceView->Release();

	D3D11_BUFFER_DESC bufferDesc;
	ZeroMemory(&bufferDesc, sizeof(bufferDesc));
	bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	bufferDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	bufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	bufferDesc.StructureByteStride = (UINT)vfStride;
	bufferDesc.ByteWidth = (UINT)(this->bufferCPU.size() * vfStride);
	D3D11_SUBRESOURCE_DATA bufferData;
	ZeroMemory(&bufferData, sizeof(bufferData));
	bufferData.pSysMem = this->bufferCPU.data();
	HRESULT hr = device->CreateBuffer(&bufferDesc, &bufferData, &physicBuffer);

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	ZeroMemory(&srvDesc, sizeof(srvDesc));
	srvDesc.Format = DXGI_FORMAT_UNKNOWN;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
	srvDesc.Buffer.ElementWidth = (UINT)this->bufferCPU.size();
	hr = device->CreateShaderResourceView(physicBuffer, &srvDesc, &physicShaderResourceView);
}

template<class T>
inline PhysicBuffer<T>::~PhysicBuffer()
{
	if (physicBuffer)
		physicBuffer->Release();
	if (physicShaderResourceView)
		physicShaderResourceView->Release();
}

// сокращённый тип данных
using PhysicVisibles = brain_memory< PhysicBuffer<int>>;
using PhysicPoint = brain_memory< PhysicBuffer<Point>>;
using PhysicWord = brain_memory< PhysicBuffer<WORD>>;
