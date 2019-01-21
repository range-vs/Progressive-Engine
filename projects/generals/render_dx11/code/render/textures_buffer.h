#pragma once

#include "../../../libraries/DXUT-master/Core/DDSTextureLoader.h"
#include "..\utilites\types.h"

class CollectionTextures
{
	//ID3D11ShaderResourceView*
	map<wstring, ID3D11Texture2D*> textures; // карта текстур: ключ - имя текстуры + путь к файлу

public:
	// конструкторы
	CollectionTextures(){} // пустой конструктор

	// методы
	ID3D11Texture2D* addTexture(ID3D11Device* device, const wstring& path); // добавляем текстуру
	ID3D11Texture2D* getTexture(const wstring& path); // получаем текстуру

	// деструкторы
	void clearTextures(); // удаляем все текстуры
	~CollectionTextures(); // деструктор
};