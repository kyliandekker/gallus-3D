#pragma once

#include "gameplay/systems/components/Component.h"

namespace gallus
{
	namespace gameplay
	{
		//---------------------------------------------------------------------
		// AnimationComponent
		//---------------------------------------------------------------------
		class AnimationComponent : public Component
		{
		public:
			/// <summary>
			/// Initializes the component in runtime.
			/// </summary>
			void InitRealtime() override;

			/// <summary>
			/// Serialized the component to a json document.
			/// </summary>
			/// <param name="a_Document">The json document that the data will be put into.</param>
			/// <param name="a_Allocator">The allocator used by the json document.</param>
			void Serialize(rapidjson::Value& a_Document, rapidjson::Document::AllocatorType& a_Allocator) const override;

			/// <summary>
			/// Deerializes sourcce data and creates an instance based on it.
			/// </summary>
			/// <param name="a_SrcData">The src data used for the component.</param>
			void Deserialize(const resources::SrcData& a_SrcData) override;
		};
	}
}