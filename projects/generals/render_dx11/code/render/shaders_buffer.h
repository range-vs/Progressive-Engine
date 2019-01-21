#pragma once

// engine
#include "../utilites/types.h"

// c++
#include <map>
#include <string>
#include <filesystem>

using std::map;
using std::wstring;
using namespace std::experimental::filesystem;

// class

class CollectionShaders
{
	map<wstring, ID3D11VertexShader*> vertex_shader_buffer; // ����� ��������� ��������
	map<wstring, ID3DBlob*> vertex_shader_blob; // ����� ��������� ��������, ���� ������
	map<wstring, ID3D11PixelShader*> pixel_shader_buffer; // ����� ���������� ��������
	map<wstring, ID3D11GeometryShader*> geometry_shader_buffer; // ����� �������������� ��������
	map<wstring, ID3D11ComputeShader*> computer_shader_buffer; // ����� �������������� ��������

	// ������� ������
	ID3DBlob* _compile_shader_from_file(WCHAR * FileName, LPCSTR EntryPoint, LPCSTR ShaderModel); // �������� �������
	void copy_data(const CollectionShaders& cs);
public:
	// ������������� ������!!!
	void* operator new(size_t i);
	void operator delete(void* p);

	// ����������� � ����������
	CollectionShaders(){}
	CollectionShaders(const CollectionShaders& sh) { this->copy_data(sh); }
	CollectionShaders& operator=(const CollectionShaders& sh) { if (this != &sh) this->copy_data(sh); return *this; }
	~CollectionShaders();

	// �������������
	void init(ID3D11Device * device, const wstring& catalogue, const char* shader_version); // �������� ���� ��������

	// �������� ������� � ��������
	shaders getShadersVP(const wstring& k);
	ID3D11VertexShader* getShadersVertex(const wstring& k);
	ID3D11PixelShader* getShadersPixel(const wstring& k);
	ID3D11GeometryShader* getShadersGeometry(const wstring& k);
	ID3D11ComputeShader* getShaderCompute(const wstring& k);

	ID3DBlob* getBlobVertexShader(const wstring& k);
	
};
