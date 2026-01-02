#include "Animation.h"

// external
#include <algorithm>

// core
#include "core/Engine.h"
#include "core/DataStream.h"

// logger
#include "logger/Logger.h"

#include "gameplay/systems/AnimationSystem.h"

// utils
#include "utils/file_abstractions.h"

// resources
#include "resources/SrcData.h"

#include "animation/AnimationEvents.h"

#define ANIMATION_TRACK_FRAME_COUNT_VAR "frameCount"
#define ANIMATION_TRACK_LOOP_VAR "loop"
#define ANIMATION_TRACK_KEY_FRAMES_VAR "keyFrames"
#define ANIMATION_TRACK_COMPONENTS_VAR "components"

namespace gallus
{
	namespace animation
	{
        //---------------------------------------------------------------------
        Animation::~Animation()
        {
            for (AnimationKeyFrame* keyFrame : m_aKeyFrames)
            {
                delete keyFrame;
            }
            m_aKeyFrames.clear();
        }

        //---------------------------------------------------------------------
        bool Animation::LoadByPath(const fs::path& a_Path)
		{
			core::Data data;
			if (!file::LoadFile(a_Path, data))
			{
                LOGF(LOGSEVERITY_ERROR, LOG_CATEGORY_ANIMATION, "Failed loading data in animation file \"%s\".", a_Path.generic_string().c_str());
                return false;
			}

            m_Path = a_Path;
            m_sName = a_Path.filename().generic_string();

            resources::SrcData srcData(data);
            if (!srcData.IsValid())
            {
                LOGF(LOGSEVERITY_ERROR, LOG_CATEGORY_ANIMATION, "Failed loading data in animation file \"%s\".", a_Path.generic_string().c_str());
                return false;
            }

            srcData.GetInt(ANIMATION_TRACK_FRAME_COUNT_VAR, m_iFrameCount);
            srcData.GetBool(ANIMATION_TRACK_LOOP_VAR, m_bIsLooping);

            resources::SrcData keyFramesSrcData;
            keyFramesSrcData.SetObject();

            srcData.GetSrcObject(ANIMATION_TRACK_KEY_FRAMES_VAR, keyFramesSrcData);

            for (AnimationKeyFrame* keyFrame : m_aKeyFrames)
            {
                delete keyFrame;
            }
            m_aKeyFrames.clear();

            std::vector<std::string> memberNames;
            keyFramesSrcData.GetAllMemberNames(memberNames);
            for (const std::string& name : memberNames)
            {
                int frame = std::stoi(name);

                resources::SrcData keyFrameSrcData;
                if (!keyFramesSrcData.GetSrcObject(name, keyFrameSrcData) || !keyFrameSrcData.IsValid())
                {
                    continue;
                }

                AnimationKeyFrame* keyFrame = new AnimationKeyFrame(frame, *this);
                m_aKeyFrames.push_back(keyFrame);

                resources::SrcData componentsSrcData;
                if (!keyFrameSrcData.GetSrcObject(ANIMATION_TRACK_COMPONENTS_VAR, componentsSrcData))
                {
                    continue;
                }

                for (auto* sys : core::ENGINE->GetECS().GetSystem<gameplay::AnimationSystem>().GetSystems())
                {
                    resources::SrcData sysSrcData;
                    if (!componentsSrcData.GetSrcObject(sys->GetPropertyName(), sysSrcData))
                    {
                        continue;
                    }

                    auto* keyFrameComp = sys->CreateComponent(*keyFrame);
                    DeserializeFields(keyFrameComp, sysSrcData);
                    keyFrame->AddComponent(keyFrameComp);
                }
            }

			return true;
		}

        //---------------------------------------------------------------------
        void Animation::Update(gameplay::EntityID& a_EntityID, float a_fDeltaTime)
        {
        }

        //---------------------------------------------------------------------
        void Animation::ActivateEvent(gameplay::EntityID& a_EntityID, AnimationEvent a_Event)
        {
            switch (a_Event)
            {
                case AnimationEvent::AnimationEvent_Delete:
                {
                    auto ent = core::ENGINE->GetECS().GetEntity(a_EntityID).lock();
                    if (!ent)
                    {
                        return;
                    }
                    ent->Destroy();
                }
                default:
                case AnimationEvent::AnimationEvent_None:
                {
                    break;
                }
            }
        }

        //---------------------------------------------------------------------
        std::vector<AnimationKeyFrame*>& Animation::GetKeyFrames()
        {
            return m_aKeyFrames;
        }

#ifdef _EDITOR

        //---------------------------------------------------------------------
        bool Animation::Save(const fs::path& a_Path) const
        {
            resources::SrcData srcData;
            srcData.SetObject();

            srcData.SetInt(ANIMATION_TRACK_FRAME_COUNT_VAR, m_iFrameCount);
            srcData.SetBool(ANIMATION_TRACK_LOOP_VAR, m_bIsLooping);

            resources::SrcData keyFramesSrcData;
            keyFramesSrcData.SetObject();

            for (AnimationKeyFrame* keyFrame : m_aKeyFrames)
            {
                resources::SrcData keyFrameSrcData;
                keyFrameSrcData.SetObject();
                
                resources::SrcData componentsSrcData;
                componentsSrcData.SetObject();

                for (auto* component : keyFrame->GetComponents())
                {
                    resources::SrcData componentSrcData;
                    componentSrcData.SetObject();

                    SerializeFields(component, componentSrcData);
                    componentsSrcData.SetSrcObject(component->GetPropertyName(), componentSrcData);
                }

                keyFrameSrcData.SetSrcObject(ANIMATION_TRACK_COMPONENTS_VAR, componentsSrcData);

                std::string frameStr = std::to_string(keyFrame->GetFrame());
                keyFramesSrcData.SetSrcObject(frameStr, keyFrameSrcData);
            }

            srcData.SetSrcObject(ANIMATION_TRACK_KEY_FRAMES_VAR, keyFramesSrcData);

            core::Data data;
            srcData.GetData(data);
            return file::SaveFile(a_Path, data);
        }

        //---------------------------------------------------------------------
        void Animation::AddKeyFrame(int a_iFrame)
        {
            for (AnimationKeyFrame* keyFrame : m_aKeyFrames)
            {
                if (keyFrame->GetFrame() == a_iFrame)
                {
                    return;
                }
            }
            AnimationKeyFrame* keyFrame = new AnimationKeyFrame(a_iFrame, *this);
            m_aKeyFrames.push_back(keyFrame);
            Sort();
        }

        //---------------------------------------------------------------------
        void Animation::RemoveKeyFrame(int a_iIndex)
        {
            m_aKeyFrames.erase(m_aKeyFrames.begin() + a_iIndex);
            Sort();
        }

        //---------------------------------------------------------------------
        void Animation::RemoveKeyFrame(AnimationKeyFrame& a_KeyFrame)
        {
            size_t i = 0;
            for (AnimationKeyFrame* keyFrame : m_aKeyFrames)
            {
                if (&a_KeyFrame == keyFrame)
                {
                    RemoveKeyFrame(i);
                    break;
                }
                i++;
            }
        }

        //---------------------------------------------------------------------
        void Animation::Sort()
        {
            std::sort(
                m_aKeyFrames.begin(),
                m_aKeyFrames.end(),
                [](const AnimationKeyFrame* a, const AnimationKeyFrame* b)
                {
                    return a->GetFrame() < b->GetFrame();
                }
            );
        }
#endif // _EDITOR
	}
}