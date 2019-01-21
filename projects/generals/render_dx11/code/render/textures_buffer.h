#pragma once

#include "../../../libraries/DXUT-master/Core/DDSTextureLoader.h"
#include "..\utilites\types.h"

class CollectionTextures
{
	//ID3D11ShaderResourceView*
	map<wstring, ID3D11Texture2D*> textures; // ����� �������: ���� - ��� �������� + ���� � �����

public:
	// ������������
	CollectionTextures(){} // ������ �����������

	// ������
	ID3D11Texture2D* addTexture(ID3D11Device* device, const wstring& path); // ��������� ��������
	ID3D11Texture2D* getTexture(const wstring& path); // �������� ��������

	// �����������
	void clearTextures(); // ������� ��� ��������
	~CollectionTextures(); // ����������
};