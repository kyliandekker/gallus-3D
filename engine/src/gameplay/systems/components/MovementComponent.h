#pragma once

#include "gameplay/systems/components/Component.h"

#include <DirectXMath.h>
#include <map>

// gameplay includes
#include "gameplay/systems/components/ColliderComponent.h"

namespace gallus
{
	namespace gameplay
	{
		//---------------------------------------------------------------------
		// MovementComponent
		//---------------------------------------------------------------------
		class MovementComponent : public Component
		{
		public:
			/// <summary>
			/// Serialized the component to a json document.
			/// </summary>
			/// <param name="a_Document">The json document that the data will be put into.</param>
			/// <param name="a_Allocator">The allocator used by the json document.</param>
			void Serialize(rapidjson::Value& a_Document, rapidjson::Document::AllocatorType& a_Allocator) const override;

			/// <summary>
			/// Deserializes data from a json document and loads it into the component.
			/// </summary>
			/// <param name="a_Document">The json document that contains the data.</param>
			/// <param name="a_Allocator">The allocator used by the json document.</param>
			void Deserialize(const rapidjson::Value& a_Document, rapidjson::Document::AllocatorType& a_Allocator) override;

			void Translate(const DirectX::XMFLOAT2& a_vTranslation);

			/// <summary>
			/// Updates the components.
			/// </summary>
			/// <param name="a_fDeltaTime">Delta time.</param>
			void UpdateRealtime(float a_fDeltaTime)
			{}

			/// <summary>
			/// Updates the components.
			/// </summary>
			/// <param name="a_fDeltaTime">Delta time.</param>
			void UpdateRealtime(float a_fDeltaTime, std::map<EntityID, ColliderComponent>& a_aColliders);

			bool HasGravity() const
			{
				return m_bHasGravity;
			}

			void SetHasGravity(bool a_bHasGravity)
			{
				m_bHasGravity = a_bHasGravity;
			}
		protected:
			bool m_bHasGravity = false;
			DirectX::XMFLOAT2 m_vTranslation = {};
		};
	}
}