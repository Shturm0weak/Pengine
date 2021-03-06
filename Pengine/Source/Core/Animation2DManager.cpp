#include "Animation2DManager.h"

#include "Serializer.h"

using namespace Pengine;

Animation2DManager& Animation2DManager::GetInstance()
{
    static Animation2DManager animation2DManager;
    return animation2DManager;
}

void Animation2DManager::Save(Animation2D* animation)
{
	if (Utils::Erase<Animation2D>(m_Animations, animation))
	{
		m_Animations.insert(std::make_pair(animation->m_Name, animation));
	}
}

Animation2DManager::Animation2D* Animation2DManager::Load(const std::string& filePath)
{
	auto animIter = m_Animations.find(Utils::GetNameFromFilePath(filePath, 4));
	if (animIter != m_Animations.end())
	{
		Animation2D* newAnimation = Serializer::DeSerializeAnimation2D(filePath);
		*animIter->second = *newAnimation;
		delete newAnimation;
		return animIter->second;
	}
	else
	{
		Animation2D* animation = Serializer::DeSerializeAnimation2D(filePath);
		m_Animations.insert(std::make_pair(animation->m_Name, animation));
		return animation;
	}
}

Animation2DManager::Animation2D* Animation2DManager::Create(const std::string& name)
{
	if (name.empty())
	{
		return nullptr;
	}

	Animation2D* animation2D = Get(name);
	if (!animation2D)
	{
		animation2D = new Animation2D("None", name);
		m_Animations.insert(std::make_pair(name, animation2D));
	}

	return animation2D;
}

Animation2DManager::Animation2D* Animation2DManager::Get(const std::string& name) const
{
	auto animIter = m_Animations.find(name);
	if (animIter != m_Animations.end())
	{
		return animIter->second;
	}
	else
	{

#ifdef SHOW_MESH_WARNINGS
		Logger::Warning("has not been found!", "Animation2D", name.c_str());
#endif

		return nullptr;
	}
}

void Animation2DManager::ShutDown()
{
	for (auto& animIter : m_Animations)
	{
		delete animIter.second;
	}
	m_Animations.clear();
}