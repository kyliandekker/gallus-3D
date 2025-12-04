#include "AnimationTrack.h"

// external
#include <rapidjson/document.h>
#include <rapidjson/utils.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/prettywriter.h>
#include <algorithm>

// core
#include "core/Engine.h"
#include "core/DataStream.h"

// animation
#include "animation/AnimationKeyFrameSpriteComponent.h"
#include "animation/AnimationKeyFrameEventComponent.h"
#include "animation/AnimationKeyFrameSoundComponent.h"

// logger
#include "logger/Logger.h"

// utils
#include "utils/file_abstractions.h"

// resources
#include "resources/SrcData.h"

#define ANIMATION_TRACK_FRAME_COUNT_VAR "frameCount"
#define ANIMATION_TRACK_LOOP_VAR "loop"
#define ANIMATION_TRACK_KEY_FRAMES_VAR "keyFrames"

namespace gallus
{
	namespace animation
	{
        //---------------------------------------------------------------------
        AnimationTrack::~AnimationTrack()
        {
            for (AnimationKeyFrame* keyFrame : m_aKeyFrames)
            {
                delete keyFrame;
            }
            m_aKeyFrames.clear();
        }

        //---------------------------------------------------------------------
        bool AnimationTrack::LoadByPath(const fs::path& a_Path)
		{
			core::Data data;
			if (!file::LoadFile(a_Path, data))
			{
				return false;
			}

            m_Path = a_Path;
            m_sName = a_Path.filename().generic_string();

			rapidjson::Document document;
			document.Parse(reinterpret_cast<char*>(data.data()), data.size());

			if (document.HasParseError())
			{
				LOGF(LOGSEVERITY_ERROR, LOG_CATEGORY_ANIMATION, "Failed loading data in animation file \"%s\".", a_Path.generic_string().c_str());
				return false;
			}

            rapidjson::GetInt(document, ANIMATION_TRACK_FRAME_COUNT_VAR, m_iFrameCount);
            rapidjson::GetBool(document, ANIMATION_TRACK_LOOP_VAR, m_bIsLooping);

            for (AnimationKeyFrame* keyFrame : m_aKeyFrames)
            {
                delete keyFrame;
            }
            m_aKeyFrames.clear();

            if (document.HasMember(ANIMATION_TRACK_KEY_FRAMES_VAR) && document[ANIMATION_TRACK_KEY_FRAMES_VAR].IsObject())
            {
                const rapidjson::Value& keyFrames = document[ANIMATION_TRACK_KEY_FRAMES_VAR];

                for (auto it = keyFrames.MemberBegin(); it != keyFrames.MemberEnd(); ++it)
                {
                    int frame = std::stoi(it->name.GetString());
                    const rapidjson::Value& keyframeObj = it->value;

                    AnimationKeyFrame* keyFrame = new AnimationKeyFrame(frame, *this);

                    // TODO: This is retarded.
                    if (keyframeObj.HasMember("components") && keyframeObj["components"].IsObject())
                    {
                        const rapidjson::Value& components = keyframeObj["components"];

                        // Sprite component
                        if (components.HasMember("sprite") && components["sprite"].IsObject())
                        {
                            AnimationKeyFrameSpriteComponent* spriteComp = keyFrame->AddComponent<AnimationKeyFrameSpriteComponent>();

                            const rapidjson::Value& sprite = components["sprite"];
                            spriteComp->Deserialize(resources::SrcData(sprite));
                        }
                        // Event component
                        if (components.HasMember("event") && components["event"].IsObject())
                        {
                            AnimationKeyFrameEventComponent* eventComp = keyFrame->AddComponent<AnimationKeyFrameEventComponent>();

                            const rapidjson::Value& sprite = components["event"];
                            eventComp->Deserialize(resources::SrcData(sprite));
                        }
                        // Sound component
                        if (components.HasMember("sound") && components["sound"].IsObject())
                        {
                            AnimationKeyFrameSoundComponent* eventComp = keyFrame->AddComponent<AnimationKeyFrameSoundComponent>();

                            const rapidjson::Value& sprite = components["sound"];
                            eventComp->Deserialize(resources::SrcData(sprite));
                        }
                    }

                    m_aKeyFrames.push_back(keyFrame);
                }
            }

			return true;
		}

        //---------------------------------------------------------------------
        void AnimationTrack::Update(gameplay::EntityID& a_EntityID, float a_fDeltaTime)
        {
        }

        //---------------------------------------------------------------------
        void AnimationTrack::ActivateEvent(gameplay::EntityID& a_EntityID, AnimationEvent a_Event)
        {
            switch (a_Event)
            {
                case AnimationEvent::AnimationEvent_Delete:
                {
                    core::ENGINE->GetECS().GetEntity(a_EntityID)->Destroy();
                }
                default:
                case AnimationEvent::AnimationEvent_None:
                {
                    break;
                }
            }
        }

        //---------------------------------------------------------------------
        std::vector<AnimationKeyFrame*>& AnimationTrack::GetKeyFrames()
        {
            return m_aKeyFrames;
        }

#ifdef _EDITOR

        //---------------------------------------------------------------------
        bool AnimationTrack::Save(const fs::path& a_Path) const
        {
            rapidjson::Document doc;
            doc.SetObject();

            doc.AddMember(ANIMATION_TRACK_FRAME_COUNT_VAR, m_iFrameCount, doc.GetAllocator());
            doc.AddMember(ANIMATION_TRACK_LOOP_VAR, m_bIsLooping, doc.GetAllocator());
            
            // Keyframes object
            rapidjson::Value keyframesObj(rapidjson::kObjectType);

            for (const AnimationKeyFrame* keyFrame : m_aKeyFrames)
            {
                rapidjson::Value keyframeObj(rapidjson::kObjectType);
                rapidjson::Value componentsObj(rapidjson::kObjectType);

                keyFrame->Serialize(componentsObj, doc.GetAllocator());

                if (!componentsObj.ObjectEmpty())
                {
                    keyframeObj.AddMember("components", componentsObj, doc.GetAllocator());
                }

                // Convert frame index to string key
                std::string frameStr = std::to_string(keyFrame->GetFrame());
                keyframesObj.AddMember(rapidjson::Value(frameStr.c_str(), doc.GetAllocator()), keyframeObj, doc.GetAllocator());
            }

            // Add keyframes to document
            doc.AddMember(ANIMATION_TRACK_KEY_FRAMES_VAR, keyframesObj, doc.GetAllocator());

            rapidjson::StringBuffer buffer;
            rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(buffer);
            doc.Accept(writer);

            core::DataStream data = core::DataStream(buffer.GetString(), buffer.GetSize());
            return file::SaveFile(a_Path, data);
        }

        //---------------------------------------------------------------------
        void AnimationTrack::AddKeyFrame(int a_iFrame)
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
        void AnimationTrack::RemoveKeyFrame(int a_iIndex)
        {
            m_aKeyFrames.erase(m_aKeyFrames.begin() + a_iIndex);
            Sort();
        }

        //---------------------------------------------------------------------
        void AnimationTrack::RemoveKeyFrame(AnimationKeyFrame& a_KeyFrame)
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
        void AnimationTrack::Sort()
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
#endif
	}
}