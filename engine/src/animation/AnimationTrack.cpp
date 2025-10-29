#include "AnimationTrack.h"

#include <rapidjson/document.h>
#include <rapidjson/utils.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/prettywriter.h>
#include <algorithm>

#include "utils/file_abstractions.h"
#include "logger/Logger.h"
#include "core/DataStream.h"
#include "resources/SrcData.h"

#define ANIMATION_TRACK_FRAME_COUNT_VAR "frameCount"
#define ANIMATION_TRACK_LOOP_VAR "loop"
#define ANIMATION_TRACK_KEY_FRAMES_VAR "keyFrames"

namespace gallus
{
	namespace animation
	{
		//void AnimationTrack::Update(gameplay::EntityID& a_EntityID, float a_fDeltaTime)
		//{
		//	m_fAccumulatedTime += a_fDeltaTime;
		//	for (auto& keyFrame : m_aKeyFrames)
		//	{
		//		if (m_fAccumulatedTime >= FRAME_TIME * keyFrame.first)
		//		{
		//			keyFrame.second.Activate(a_EntityID);
		//		}
		//		else
		//		{
		//			break;
		//		}
		//	}
		//}

        AnimationTrack::~AnimationTrack()
        {
            for (AnimationKeyFrame* keyFrame : m_aKeyFrames)
            {
                delete keyFrame;
            }
            m_aKeyFrames.clear();
        }

        bool AnimationTrack::LoadByPath(const fs::path& a_Path)
		{
			core::Data data;
			if (!file::LoadFile(a_Path, data))
			{
				return false;
			}

			rapidjson::Document document;
			document.Parse(reinterpret_cast<char*>(data.data()), data.size());

			if (document.HasParseError())
			{
				LOGF(LOGSEVERITY_ERROR, LOG_CATEGORY_ANIMATION, "Failed loading data in animation file \"%s\".", a_Path.generic_string().c_str());
				return false;
			}

            rapidjson::GetInt(document, ANIMATION_TRACK_FRAME_COUNT_VAR, m_iFrameCount);
            rapidjson::GetBool(document, ANIMATION_TRACK_LOOP_VAR, m_bIsLooping);

            m_aKeyFrames.clear();

            if (document.HasMember(ANIMATION_TRACK_KEY_FRAMES_VAR) && document[ANIMATION_TRACK_KEY_FRAMES_VAR].IsObject())
            {
                const rapidjson::Value& keyFrames = document[ANIMATION_TRACK_KEY_FRAMES_VAR];

                for (auto it = keyFrames.MemberBegin(); it != keyFrames.MemberEnd(); ++it)
                {
                    int frame = std::stoi(it->name.GetString());
                    const rapidjson::Value& keyframeObj = it->value;

                    AnimationKeyFrame* keyFrame = new AnimationKeyFrame(frame);
                
                    if (keyframeObj.HasMember("components") && keyframeObj["components"].IsObject())
                    {
                        const rapidjson::Value& components = keyframeObj["components"];

                        // Sprite component
                        if (components.HasMember("spriteComponent") && components["spriteComponent"].IsObject())
                        {
                            AnimationKeyFrameSpriteComponent* spriteComp = keyFrame->AddComponent<AnimationKeyFrameSpriteComponent>();

                            const rapidjson::Value& sprite = components["spriteComponent"];
                            spriteComp->Deserialize(resources::SrcData(sprite));
                        }
                    }

                    m_aKeyFrames.emplace_back(keyFrame);
                }
            }

			return true;
		}

        //---------------------------------------------------------------------
        void AnimationTrack::Update(gameplay::EntityID& a_EntityID, float a_fDeltaTime)
        {
            m_fAccumulatedTime += a_fDeltaTime;

            // Loop over keyframes that are due
            while (m_iNextKeyFrameIndex < m_aKeyFrames.size())
            {
                int keyFrameNumber = m_aKeyFrames[m_iNextKeyFrameIndex]->GetFrame(); // actual frame number
                float keyFrameTime = keyFrameNumber * FRAME_TIME;

                if (m_fAccumulatedTime >= keyFrameTime)
                {
                    m_aKeyFrames[m_iNextKeyFrameIndex]->Activate(a_EntityID, *this);
                    m_iNextKeyFrameIndex++;  // only increment after successfully activating a keyframe
                }
                else
                {
                    break; // next keyframe is not yet due
                }
            }

            // Reset if animation is done
            if (m_bIsLooping && !m_aKeyFrames.empty() && m_iNextKeyFrameIndex >= m_aKeyFrames.size())
            {
                m_fAccumulatedTime = 0.0f;
                m_iNextKeyFrameIndex = 0;
            }
        }


        //---------------------------------------------------------------------
        std::vector<AnimationKeyFrame*>& AnimationTrack::GetKeyFrames()
        {
            return m_aKeyFrames;
        }

        //---------------------------------------------------------------------
#ifdef _EDITOR
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
            m_aKeyFrames.emplace_back(new AnimationKeyFrame(a_iFrame));
            Sort();
        }

        //---------------------------------------------------------------------
        void AnimationTrack::RemoveKeyFrame(int a_iIndex)
        {
            m_aKeyFrames.erase(m_aKeyFrames.begin() + a_iIndex);
            Sort();
        }

        //---------------------------------------------------------------------
        void AnimationTrack::Sort()
        {
            std::sort(m_aKeyFrames.begin(), m_aKeyFrames.end());
        }
#endif
	}
}