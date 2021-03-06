#include "TextureManager.h"

#include "ThreadPool.h"
#include "Logger.h"
#include "Utils.h"
#include "../EventSystem/EventSystem.h"
#include "../Events/OnMainThreadCallback.h"

using namespace Pengine;

void TextureManager::DispatchLoadedTextures()
{
	for (auto i = m_WaitingForTextures.begin(); i != m_WaitingForTextures.end();)
	{
		std::function<void(Texture*)> callback = i->second;
		Texture* texture = Get(i->first);
		if (texture != nullptr)
		{
			callback(texture);
			m_WaitingForTextures.erase(i++);
		}
		else
		{
			++i;
		}
	}
}

TextureManager& TextureManager::GetInstance()
{
	static TextureManager textureManager;
	return textureManager;
}

void TextureManager::ShutDown()
{
	std::vector<Texture*> textures;
	for (auto i = m_Textures.begin(); i != m_Textures.end(); i++)
	{
		textures.push_back(i->second);
	}
	m_Textures.clear();

	for (uint32_t i = 0; i < textures.size(); i)
	{
		delete textures[i];
		textures.erase(textures.begin() + i);
	}
}

void TextureManager::Delete(Texture* texture)
{
	auto iter = m_Textures.find(texture->GetName());
	if (iter != m_Textures.end())
	{
		delete iter->second;
		m_Textures.erase(iter);
	}
}

void TextureManager::AsyncCreate(const std::string& filePath)
{
	ThreadPool::GetInstance().Enqueue([=] {
		if (!Utils::MatchType(filePath, { "jpeg", "png", "jpg" })) return;
		Texture* texture = Get(Utils::GetNameFromFilePath(filePath));
		if (texture == nullptr)
		{
			texture = new Texture(filePath);
			m_Textures.insert(std::make_pair(texture->GetName(), texture));

			texture->LoadInRAM();

			std::vector<Texture::TexParameteri> params = m_TexParameters;

			std::function<void()> callback = std::function<void()>([=] {
				texture->LoadInVRAM(params);
				DispatchLoadedTextures();
			});
			EventSystem::GetInstance().SendEvent(new OnMainThreadCallback(callback, EventType::ONMAINTHREADPROCESS));
		}
		else
		{
			std::function<void()> callback = std::function<void()>([=] {
				DispatchLoadedTextures();
			});
			EventSystem::GetInstance().SendEvent(new OnMainThreadCallback(callback, EventType::ONMAINTHREADPROCESS));
		}
	});
}

Texture* TextureManager::Create(const std::string& filePath, bool flip)
{
	if (!Utils::MatchType(filePath, { "jpeg", "png", "jpg" })) return nullptr;
	Texture* texture = Get(Utils::GetNameFromFilePath(filePath));
	if (texture == nullptr)
	{
		texture = new Texture(filePath);
		m_Textures.insert(std::make_pair(texture->GetName(), texture));

		texture->LoadInRAM();
		texture->LoadInVRAM(m_TexParameters);
		DispatchLoadedTextures();
		return texture;
	}
	else
	{
		return texture;
	}
}

Texture* TextureManager::ColoredTexture(const std::string& name, uint32_t color)
{
	Texture* texture = Get(name);
	if (texture)
	{
#ifdef _DEBUG
		Logger::Warning("has already existed!", "Texture", name.c_str());
#endif
		return texture;
	}

	texture = new Texture(name);
	texture->m_Name = name;
	texture->ColoredTexture(m_TexParameters, color);

	m_Textures.insert(std::make_pair(texture->GetName(), texture));

	return texture;
}

Texture* TextureManager::Get(const std::string& name, bool showErrors)
{
	std::string filename = name;
	if (Utils::Contains(name, ".png") || Utils::Contains(name, ".jpg") || Utils::Contains(name, ".jpeg"))
	{
		filename = Utils::RemoveResolution(name);
	}
	auto textureIter = m_Textures.find(filename);
	if (textureIter != m_Textures.end())
	{
		return textureIter->second;
	}
	else
	{
		if (showErrors)
		{
			Logger::Warning("doesn't exist!", "Texture", filename.c_str());
		}

		return nullptr;
	}
}

void TextureManager::AsyncGet(std::function<void(Texture* t)> callback, const std::string& name)
{
	Texture* texture = Get(name);
	if (texture != nullptr)
	{
		callback(texture);
	}
	else
	{
		m_WaitingForTextures.insert(std::make_pair(name, callback));
	}
}

void TextureManager::RemoveFromGetAsync(const std::string& name)
{
	for (auto i = m_WaitingForTextures.begin(); i != m_WaitingForTextures.end();)
	{
		if (i->first == name)
		{
			m_WaitingForTextures.erase(i++);
		}
		else
		{
			++i;
		}
	}
}

void TextureManager::ResetTexParametersi()
{
	m_TexParameters.resize(4);
	m_TexParameters[0] = { GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST };
	m_TexParameters[1] = { GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST };
	m_TexParameters[2] = { GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE };
	m_TexParameters[3] = { GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE };
}

std::vector<Texture*> TextureManager::GetTexturesFromFolder(const std::string& filePath)
{
	std::vector<Texture*> textures;
	std::filesystem::path path(filePath);

	for (auto& directoryIter : std::filesystem::directory_iterator(path))
	{
		std::string filename = directoryIter.path().string();
		if (Utils::Contains(filename, "png") || Utils::Contains(filename, "jpg"))
		{
			textures.push_back(Create(filename));
		}
	}

	return textures;
}
