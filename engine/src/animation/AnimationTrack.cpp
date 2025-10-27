#include "AnimationTrack.h"

#include <rapidjson/document.h>
#include <rapidjson/utils.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/prettywriter.h>
#include <algorithm>

#include "utils/file_abstractions.h"
#include "logger/Logger.h"
#include "core/DataStream.h"

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

            // keyframes
            if (document.HasMember(ANIMATION_TRACK_KEY_FRAMES_VAR) && document[ANIMATION_TRACK_KEY_FRAMES_VAR].IsObject())
            {
                const rapidjson::Value& keyframes = document[ANIMATION_TRACK_KEY_FRAMES_VAR];

                for (auto it = keyframes.MemberBegin(); it != keyframes.MemberEnd(); ++it)
                {
                    int frame = std::stoi(it->name.GetString());
                    const rapidjson::Value& keyframeObj = it->value;

                    AnimationKeyFrame keyframe(frame);

                    //if (keyframeObj.HasMember("components") && keyframeObj["components"].IsObject())
                    //{
                    //    const rapidjson::Value& components = keyframeObj["components"];

                    //    // Sprite component
                    //    if (components.HasMember("sprite") && components["sprite"].IsObject())
                    //    {
                    //        const rapidjson::Value& sprite = components["sprite"];
                    //        SpriteComponent s;
                    //        if (sprite.HasMember("spriteIndex") && sprite["spriteIndex"].IsInt())
                    //        {
                    //            s.spriteIndex = sprite["spriteIndex"].GetInt();
                    //        }
                    //        if (sprite.HasMember("texture") && sprite["texture"].IsString())
                    //        {
                    //            s.texture = sprite["texture"].GetString();
                    //        }
                    //        keyframe.sprite = s;
                    //    }

                    //    // Event component
                    //    if (components.HasMember("event") && components["event"].IsObject())
                    //    {
                    //        const rapidjson::Value& eventObj = components["event"];
                    //        EventComponent e;

                    //        if (eventObj.HasMember("name") && eventObj["name"].IsString())
                    //        {
                    //            e.name = eventObj["name"].GetString();
                    //        }

                    //        if (eventObj.HasMember("parameters") && eventObj["parameters"].IsObject())
                    //        {
                    //            for (auto p = eventObj["parameters"].MemberBegin(); p != eventObj["parameters"].MemberEnd(); ++p)
                    //            {
                    //                if (p->value.IsNumber())
                    //                {
                    //                    e.parameters[p->name.GetString()] = static_cast<float>(p->value.GetDouble());
                    //                }
                    //            }
                    //        }

                    //        keyframe.event = e;
                    //    }
                    //}

                    m_aKeyFrames.emplace_back(keyframe);
                }
            }

			return true;
		}

        //---------------------------------------------------------------------
        std::vector<AnimationKeyFrame>& AnimationTrack::GetKeyFrames()
        {
            return m_aKeyFrames;
        }

        //---------------------------------------------------------------------
        bool AnimationTrack::Save(const fs::path& a_Path) const
        {
            rapidjson::Document doc;
            doc.SetObject();

            doc.AddMember(ANIMATION_TRACK_FRAME_COUNT_VAR, m_iFrameCount, doc.GetAllocator());
            doc.AddMember(ANIMATION_TRACK_LOOP_VAR, m_bIsLooping, doc.GetAllocator());
            
            // Keyframes object
            rapidjson::Value keyframesObj(rapidjson::kObjectType);

            for (const auto& keyFrame : m_aKeyFrames)
            {
                rapidjson::Value keyframeObj(rapidjson::kObjectType);
                rapidjson::Value componentsObj(rapidjson::kObjectType);

                if (!componentsObj.ObjectEmpty())
                {
                    keyframeObj.AddMember("components", componentsObj, doc.GetAllocator());
                }

                // Convert frame index to string key
                std::string frameStr = std::to_string(keyFrame.GetFrame());
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
            for (auto& keyFrame : m_aKeyFrames)
            {
                if (keyFrame.GetFrame() == a_iFrame)
                {
                    return;
                }
            }
            m_aKeyFrames.emplace_back(a_iFrame);
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
	}
}