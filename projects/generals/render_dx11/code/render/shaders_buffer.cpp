#include "shaders_buffer.h"

// �������������

void CollectionShaders::init(ID3D11Device * device, const wstring& catalogue, const char* shader_version)
{
	recursive_directory_iterator begin(catalogue);
	recursive_directory_iterator end;

	for(;begin!=end;begin++) // ���������� ��� ����� � ��������
	{
		auto p = begin->path();
		ID3DBlob* buffer = nullptr; // ��������� ����� ������� ������
		ID3D11VertexShader* tmp_vertex_shader; // ������ ������ ����������������
		ID3D11PixelShader* tmp_pixel_shader; // ������ �������� ����������������
		ID3D11GeometryShader* tmp_geometry_shader; // ������ �������������� ����������������
		ID3D11ComputeShader* tmp_compute_shader; // ������ �������������� ����������������

		if (p.extension().wstring() == L".vs" || p.extension().wstring() == L".ps" || p.extension().wstring() == L".gs" || p.extension().wstring() == L".cs") // ��� ������, ���������� ���������
		{
			// �������� ��� ������� ��� ����������
			wstring _path = p.c_str(); _path.resize(_path.length() - 3);

			string sh = p.extension().string().substr(1, p.extension().string().length() - 1) + "_" + shader_version;
			buffer = this->_compile_shader_from_file((WCHAR*)(_path + p.extension().wstring()).c_str(), "main", sh.c_str());
			if (buffer)
			{
				if (p.extension().wstring() == L".vs")
				{
					device->CreateVertexShader(buffer->GetBufferPointer(), buffer->GetBufferSize(), NULL, &tmp_vertex_shader); // ���������� ������(� ���� ����)
					vertex_shader_buffer.insert({ _path.substr(_path.find_last_of(L"\\") + 1), tmp_vertex_shader });
					vertex_shader_blob.insert({ _path.substr(_path.find_last_of(L"\\") + 1), buffer }); // ��������� � �����
				}
				else if (p.extension().wstring() == L".ps")
				{
					device->CreatePixelShader(buffer->GetBufferPointer(), buffer->GetBufferSize(), NULL, &tmp_pixel_shader);
					pixel_shader_buffer.insert({ _path.substr(_path.find_last_of(L"\\") + 1), tmp_pixel_shader });
				}
				else if (p.extension().wstring() == L".gs")
				{
					device->CreateGeometryShader(buffer->GetBufferPointer(), buffer->GetBufferSize(), NULL, &tmp_geometry_shader);
					geometry_shader_buffer.insert({ _path.substr(_path.find_last_of(L"\\") + 1), tmp_geometry_shader });
				}
				else if (p.extension().wstring() == L".cs")
				{
					device->CreateComputeShader(buffer->GetBufferPointer(), buffer->GetBufferSize(), NULL, &tmp_compute_shader);
					computer_shader_buffer.insert({ _path.substr(_path.find_last_of(L"\\") + 1), tmp_compute_shader });
				}
			}
			else
			{
				// ����� ��� ������
			}
		}
	}
}

ID3DBlob* CollectionShaders::_compile_shader_from_file(WCHAR * FileName, LPCSTR EntryPoint, LPCSTR ShaderModel)
{
	// args: ��� �����, ������� ������� �������, ������ �������
	// � ����������������� �������� �������: ������ �� �������, ����� ��������
	ID3DBlob* error(nullptr);
	ID3DBlob* ppBlobOut; // �������� ������
	UINT flags(NULL);
#ifdef DEBUG
	flags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

	HRESULT hr(D3DCompileFromFile(FileName, NULL, NULL,
		EntryPoint, ShaderModel, /*D3DCOMPILE_PACK_MATRIX_ROW_MAJOR*/flags,
		0, &ppBlobOut, &error));
	std::string err;
	if (error)
	{
		for (int i(0); i < error->GetBufferSize(); i++)
			err += ((char*)(error->GetBufferPointer()))[i];
		MessageBoxA(NULL, err.c_str(), "Error", MB_OK);
	}
	if (hr != S_OK)
		return nullptr;
	return ppBlobOut;
}

// �������

shaders CollectionShaders::getShadersVP(const wstring & k)
{
	shaders sh = { nullptr, nullptr };
	if (vertex_shader_buffer.find(k) != vertex_shader_buffer.end())
		sh.vertexShader = vertex_shader_buffer[k];
	if (pixel_shader_buffer.find(k) != pixel_shader_buffer.end())
		sh.pixelShader = pixel_shader_buffer[k];
	return sh;
}

ID3D11VertexShader * CollectionShaders::getShadersVertex(const wstring & k)
{
	if(vertex_shader_buffer.find(k) != vertex_shader_buffer.end())
		return vertex_shader_buffer[k];
	return nullptr;
}

ID3D11GeometryShader * CollectionShaders::getShadersGeometry(const wstring & k)
{
	if(geometry_shader_buffer.find(k) != geometry_shader_buffer.end())
		return geometry_shader_buffer[k];
	return nullptr;
}

ID3D11PixelShader * CollectionShaders::getShadersPixel(const wstring & k)
{
	if(pixel_shader_buffer.find(k) != pixel_shader_buffer.end())
		return pixel_shader_buffer[k];
	return nullptr;
}

ID3D11ComputeShader * CollectionShaders::getShaderCompute(const wstring & k)
{
	if(computer_shader_buffer.find(k) != computer_shader_buffer.end())
		return computer_shader_buffer[k];
	return nullptr;
}

// ���������� �������
void* CollectionShaders::operator new(size_t i)
{
	return _aligned_malloc(i, 16);
}

void CollectionShaders::operator delete(void* p)
{
	_aligned_free(p);
}

// �����������
void CollectionShaders::copy_data(const CollectionShaders& cs)
{
	this->pixel_shader_buffer = cs.pixel_shader_buffer;
	this->vertex_shader_buffer = cs.vertex_shader_buffer;
	this->geometry_shader_buffer = cs.geometry_shader_buffer;
	this->computer_shader_buffer = cs.computer_shader_buffer;
	std::for_each(this->pixel_shader_buffer.begin(), this->pixel_shader_buffer.end(), [](const std::pair<wstring, ID3D11PixelShader*>& e) {
		e.second->AddRef();
	});
	std::for_each(this->vertex_shader_buffer.begin(), this->vertex_shader_buffer.end(), [](const std::pair<wstring, ID3D11VertexShader*>& e) {
		e.second->AddRef();
	});
	std::for_each(this->vertex_shader_blob.begin(), this->vertex_shader_blob.end(), [](const std::pair<wstring, ID3DBlob*>& e) {
		e.second->AddRef();
	});
	std::for_each(this->geometry_shader_buffer.begin(), this->geometry_shader_buffer.end(), [](const std::pair<wstring, ID3D11GeometryShader*>& e) {
		e.second->AddRef();
	});
	std::for_each(this->computer_shader_buffer.begin(), this->computer_shader_buffer.end(), [](const std::pair<wstring, ID3D11ComputeShader*>& e) {
		e.second->AddRef();
	});
}

CollectionShaders::~CollectionShaders()
{
	std::for_each(this->pixel_shader_buffer.begin(), this->pixel_shader_buffer.end(), [](const std::pair<wstring, ID3D11PixelShader*>& e) {
		e.second->Release();
	});
	std::for_each(this->vertex_shader_buffer.begin(), this->vertex_shader_buffer.end(), [](const std::pair<wstring, ID3D11VertexShader*>& e) {
		e.second->Release();
	});
	std::for_each(this->vertex_shader_blob.begin(), this->vertex_shader_blob.end(), [](const std::pair<wstring, ID3DBlob*>& e) {
		e.second->Release();
	});
	std::for_each(this->geometry_shader_buffer.begin(), this->geometry_shader_buffer.end(), [](const std::pair<wstring, ID3D11GeometryShader*>& e) {
		e.second->Release();
	});
	std::for_each(this->computer_shader_buffer.begin(), this->computer_shader_buffer.end(), [](const std::pair<wstring, ID3D11ComputeShader*>& e) {
		e.second->Release();
	});
}

ID3DBlob * CollectionShaders::getBlobVertexShader(const wstring & k)
{
	return this->vertex_shader_blob[k];
}