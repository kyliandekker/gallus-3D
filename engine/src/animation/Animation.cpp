#include "Animation.h"

// external
#include <algorithm>

// core
#include "core/Engine.h"

// logger
#include "logger/Logger.h"

// gameplay
#include "gameplay/EntityComponentSystem.h"
#include "gameplay/systems/AnimationSystem.h"

// resources
#include "resources/SrcData.h"

// animation
#include "animation/AnimationEvents.h"
#include "animation/AnimationKeyFrame.h"

#define ANIMATION_TRACK_FRAME_COUNT_VAR "frameCount"
#define ANIMATION_TRACK_LOOP_VAR "loop"
#define ANIMATION_TRACK_KEY_FRAMES_VAR "keyFrames"
#define ANIMATION_TRACK_COMPONENTS_VAR "components"

namespace gallus::animation
{
	//---------------------------------------------------------------------
	// Animation
	//---------------------------------------------------------------------
	Animation::Animation() : EngineResource()
	{
		m_aKeyFrames.reserve(100);
	}
	
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
	bool Animation::LoadData(const core::Data& a_Data)
	{
		resources::SrcData srcData(a_Data);

		if (!srcData.IsValid())
		{
			LOGF(LOGSEVERITY_ERROR, LOG_CATEGORY_DX12, "Failed loading animation \"%s\": Something wrong with animation data.", m_sName.c_str());
			return false;
		}

		DeserializeFields(this, srcData, SerializationMethod::SerializationMethod_File);

		// Manually extract the key frame information.
		// TODO: This would be possible with a key frame factory if I want automatic serialization and deserialization.
		resources::SrcData keyFramesSrcData;
		keyFramesSrcData.SetObject();

		srcData.GetSrcObject(ANIMATION_TRACK_KEY_FRAMES_VAR, keyFramesSrcData);

		for (AnimationKeyFrame* keyFrame : m_aKeyFrames)
		{
			delete keyFrame;
		}
		m_aKeyFrames.clear();

		gameplay::EntityComponentSystem* ecs = GetEngine().GetECS();
		if (!ecs)
		{
			return false;
		}

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

			for (animation::AnimationKeyFrameBaseSystem* sys : ecs->GetSystem<gameplay::AnimationSystem>()->GetSystems())
			{
				resources::SrcData sysSrcData;
				if (!componentsSrcData.GetSrcObject(sys->GetPropertyName(), sysSrcData))
				{
					continue;
				}

				animation::AnimationKeyFrameComponentBase* keyFrameComp = sys->CreateComponent(*keyFrame);
				DeserializeFields(keyFrameComp, sysSrcData);
				keyFrame->AddComponent(keyFrameComp);
			}
		}

		m_AssetType = resources::AssetType::Animation;
		
		LOGF(LOGSEVERITY_INFO_SUCCESS, LOG_CATEGORY_DX12, "Successfully loaded animation \"%s\".", m_sName.c_str());
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
				gameplay::EntityComponentSystem* ecs = GetEngine().GetECS();
				if (!ecs)
				{
					return;
				}

				ecs->DeleteEntity(a_EntityID);
			}
			case AnimationEvent::AnimationEvent_None:
			{
				break;
			}
			default:
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

	//---------------------------------------------------------------------
	void Animation::AddKeyFrame(size_t a_iFrame)
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
	void Animation::RemoveKeyFrame(size_t a_iIndex)
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

	//---------------------------------------------------------------------
	int32_t Animation::GetKeyFrameAtFrame(size_t a_iIndex)
	{
		size_t i = 0;
		for (AnimationKeyFrame* keyFrame : m_aKeyFrames)
		{
			if (a_iIndex == keyFrame->GetFrame())
			{
				return i;
			}
			i++;
		}
		return -1;
	}
}
