#pragma once
#include <vector>
#include <map>
#include <glm/glm.hpp>
#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <functional>
#include "Bone.h"
#include "../model/BoneInfo.h"
#include "../model/Node.h"


class Animation
{
public:
	float m_Duration;
	int m_TicksPerSecond;
	std::vector<Bone> m_Bones;
	std::map<std::string, BoneInfo> m_BoneInfoMap;
	Node* m_RootNode;


	Animation() = default;

	Animation(const aiAnimation* animation , Node* root , std::map<string, BoneInfo>& boneInfoMap, int& boneCount)
	{
		m_Duration = animation->mDuration;
		m_TicksPerSecond = animation->mTicksPerSecond;
		m_RootNode = root;
		ReadBones(animation, boneInfoMap, boneCount);
	}

	~Animation() {}

	Bone* FindBone(const std::string& name)
	{
		auto iter = std::find_if(m_Bones.begin(), m_Bones.end(),
			[&](const Bone& Bone)
			{
				return Bone.GetBoneName() == name;
			}
		);
		if (iter == m_Bones.end()) return nullptr;
		else return &(*iter);
	}


private:
	void ReadBones(const aiAnimation* animation , std::map<string, BoneInfo>& boneInfoMap, int& boneCount)
	{
		int size = animation->mNumChannels;

		//reading channels(bones engaged in an animation and their keyframes)
		for (int i = 0; i < size; i++)
		{
			auto channel = animation->mChannels[i];
			std::string boneName = channel->mNodeName.data;

			if (boneInfoMap.find(boneName) == boneInfoMap.end())
			{
				boneInfoMap[boneName].id = boneCount;
				boneCount++;
			}
			m_Bones.push_back(Bone(channel->mNodeName.data,
				boneInfoMap[channel->mNodeName.data].id, channel));
		}

		m_BoneInfoMap = boneInfoMap;
	}
	
};