#pragma once

#include "gameplay/systems/components/Component.h"

// graphics includes
#include "graphics/dx12/DX12Transform.h"

namespace gallus
{
	namespace gameplay
	{
		//---------------------------------------------------------------------
		// TransformComponent
		//---------------------------------------------------------------------
		class TransformComponent : public Component
		{
		public:
			/// <summary>
			/// Retrieves the transform.
			/// </summary>
			/// <returns>Reference to the transform used in the transform component.</returns>
			graphics::dx12::DX12Transform& Transform();

			/// <summary>
			/// Serialized the component to a json document.
			/// </summary>
			/// <param name="a_Document">The json document that the data will be put into.</param>
			/// <param name="a_Allocator">The allocator used by the json document.</param>
			void Serialize(rapidjson::Value& a_Document, rapidjson::Document::AllocatorType& a_Allocator) const override;

			/// <summary>
			/// Creates an instance based on source data.
			/// </summary>
			/// <param name="a_SrcData">The source data.</param>
			void Deserialize(const resources::SrcData& a_SrcData) override;
		private:
			graphics::dx12::DX12Transform m_Transform;
		};
	}
}