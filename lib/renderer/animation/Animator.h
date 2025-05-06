#pragma once

#include <glm/glm.hpp>
#include <map>
#include <vector>
#include "Animation.h"
#include "Bone.h"
#include "../model/Model.h"



class Animator
{
private:
	Animation* m_CurrentAnimation;
	float m_CurrentTime;
	float m_DeltaTime;


public:
	Animator(Animation* animation)
	{
		m_CurrentTime = 0.0;
		m_CurrentAnimation = animation;
	}

	void UpdateAnimation(float dt)
	{
		m_DeltaTime = dt;
		if (m_CurrentAnimation)
		{
			m_CurrentTime += m_CurrentAnimation->m_TicksPerSecond * dt;
			m_CurrentTime = fmod(m_CurrentTime, m_CurrentAnimation->m_Duration);
			glm::mat4 identity = mat4(1.0f);
			CalculateBoneTransform(m_CurrentAnimation->m_RootNode, identity);
		}

	}

	void PlayAnimation(Animation* pAnimation)
	{
		m_CurrentAnimation = pAnimation;
		m_CurrentTime = 0.0f;
	}

	void CalculateBoneTransform(const Node* node, glm::mat4 &parentTransform)
	{
		std::string nodeName = node->name;
		glm::mat4 nodeTransform = node->transformation;

		Bone* Bone = m_CurrentAnimation->FindBone(nodeName);

		if (Bone)
		{
			Bone->Update(m_CurrentTime);
			nodeTransform = Bone->GetLocalTransform();
		}

		glm::mat4 globalTransformation = parentTransform * nodeTransform;

		std::map<std::string, BoneInfo>& boneInfoMap = m_CurrentAnimation->m_BoneInfoMap;
		if (boneInfoMap.find(nodeName) != boneInfoMap.end())
		{
			BoneInfo& boneInfo = boneInfoMap[nodeName];
			boneInfo.finalTransformMatrix = globalTransformation * boneInfo.offset;
		}

		for (int i = 0; i < node->children.size(); i++)
			CalculateBoneTransform(node->children[i], globalTransformation);
	}

};