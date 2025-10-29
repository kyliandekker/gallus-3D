#pragma once

#include "AnimationKeyFrameComponentBase.h"

namespace gallus
{
	namespace graphics
	{
		namespace dx12
		{
			class Texture;
		}
	}
	namespace resources
	{
		class SrcData;
	}
	namespace animation
	{
		class AnimationKeyFrameTagComponent : public AnimationKeyFrameComponentBase
		{
		public:
			AnimationKeyFrameTagComponent(AnimationKeyFrame& a_KeyFrame) : AnimationKeyFrameComponentBase(a_KeyFrame)
			{
			}

			void Activate(gameplay::EntityID& a_EntityID) override;

			std::string GetName() const override
			{
				return "Tag";
			}
#ifdef _EDITOR
			void Serialize(rapidjson::Value& a_Value, rapidjson::Document::AllocatorType& a_Allocator) const override;
			std::string GetPropertyName() const override
			{
				return "tagComponent";
			}
#endif
			void Deserialize(const resources::SrcData& a_SrcData) override;

			const std::string& GetTag() const
			{
				return m_sTag;
			}

			void SetTag(const std::string& a_sTag)
			{
				m_sTag = a_sTag;
			}
		private:
			std::string m_sTag;
		};
	}
}