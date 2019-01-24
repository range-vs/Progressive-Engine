#include "textures_buffer.h"

ID3D11Texture2D* CollectionTextures::addTexture(ID3D11Device* device, const wstring & path)
{
	wstring p = LR"(engine_resource\textures\)" + path;
	auto iter = textures.find(p);
	if (iter != textures.end())
		return iter->second;
	ID3D11Texture2D* r(nullptr);
	HRESULT hr = DirectX::CreateDDSTextureFromFileEx(device, p.c_str(), 0u, D3D11_USAGE_STAGING, 0,
		D3D11_CPU_ACCESS_WRITE | D3D11_CPU_ACCESS_READ, 0, false, (ID3D11Resource**)&r, nullptr, nullptr);
	if (hr != S_OK) // не смогли загрузить текстуру(её нет, или она повреждена)
	{
		// ищем заглушку
		if (textures.find(LR"(engine_resource\textures\no_texture.dds)") == textures.end())
			return nullptr; // заглушка не найдена, программу нельзя запустить
		// возвращаем заглушку
		return textures[LR"(engine_resource\textures\no_texture.dds)"];
	}
	textures.insert({ p, r });
	return r;
}

ID3D11Texture2D * CollectionTextures::getTexture(const wstring & path)
{
	return textures[path];
}

void CollectionTextures::clearTextures()
{
	for (auto& t : textures)
		if (t.second)
			t.second->Release();
}

CollectionTextures::~CollectionTextures()
{
	clearTextures();
}