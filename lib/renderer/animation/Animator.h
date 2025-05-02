#pragma once

#include <glm/glm.hpp>
#include <map>
#include <vector>
#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include "animation.h"
#include "bone.h"



class Animator
{
public:
    Animator(Animation* animation)
    {
        m_CurrentTime = 0.0f;
        m_CurrentAnimation = animation;
        m_FinalBoneMatrices = animation->finalBonesMatrices;
        m_PlaybackSpeed = 1.0f;
        m_IsPaused = false;
        m_ShouldLoop = true;
    }

    void UpdateAnimation(float dt)
    {
        if (m_IsPaused || !m_CurrentAnimation) return;

        m_DeltaTime = dt * m_PlaybackSpeed;
        m_CurrentTime += m_CurrentAnimation->GetTicksPerSecond() * m_DeltaTime;

        if (m_CurrentTime > m_CurrentAnimation->GetDuration())
        {
            if (m_ShouldLoop)
                m_CurrentTime = fmod(m_CurrentTime, m_CurrentAnimation->GetDuration());
            else
            {
                m_CurrentTime = m_CurrentAnimation->GetDuration();
                m_IsPaused = true; // Auto-pause when reaching end in non-loop mode
            }
        }

        CalculateBoneTransform(&m_CurrentAnimation->GetRootNode(), glm::mat4(1.0f));
    }

    // Playback control
    void SetPlaybackSpeed(float speed) { m_PlaybackSpeed = speed; }
    float GetPlaybackSpeed() const { return m_PlaybackSpeed; }

    // Time control
    void SetNormalizedTime(float t)
    {
        if (m_CurrentAnimation)
            m_CurrentTime = glm::clamp(t, 0.0f, 1.0f) * m_CurrentAnimation->GetDuration();
    }

    float GetNormalizedTime() const
    {
        if (!m_CurrentAnimation) return 0.0f;
        return m_CurrentTime / m_CurrentAnimation->GetDuration();
    }

    void SetTime(float time)
    {
        if (m_CurrentAnimation)
            m_CurrentTime = glm::clamp(time, 0.0f, m_CurrentAnimation->GetDuration());
    }

    float GetTime() const { return m_CurrentTime; }
    float GetDuration() const { return m_CurrentAnimation ? m_CurrentAnimation->GetDuration() : 0.0f; }

    // Playback state
    void Pause() { m_IsPaused = true; }
    void Resume() { m_IsPaused = false; }
    bool IsPaused() const { return m_IsPaused; }

    // Looping control
    void SetLooping(bool loop) { m_ShouldLoop = loop; }
    bool IsLooping() const { return m_ShouldLoop; }

private:
    void CalculateBoneTransform(const AssimpNodeData* node, glm::mat4 parentTransform)
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

        auto boneInfoMap = m_CurrentAnimation->GetBoneIDMap();
        if (boneInfoMap.find(nodeName) != boneInfoMap.end())
        {
            int index = boneInfoMap[nodeName].id;
            glm::mat4 offset = boneInfoMap[nodeName].offset;
            (*m_FinalBoneMatrices)[index] = globalTransformation * offset;
        }

        for (int i = 0; i < node->childrenCount; i++)
            CalculateBoneTransform(&node->children[i], globalTransformation);
    }

    std::vector<glm::mat4>* m_FinalBoneMatrices;
    Animation* m_CurrentAnimation;
    float m_CurrentTime;
    float m_DeltaTime;
    float m_PlaybackSpeed;
    bool m_IsPaused;
    bool m_ShouldLoop;
};