#pragma once

#include "gameplay/systems/components/Component.h"

#include <DirectXMath.h>

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
			/// Creates an instance based on source data.
			/// </summary>
			/// <param name="a_SrcData">The source data.</param>
			void Deserialize(const resources::SrcData& a_SrcData) override;

			const DirectX::XMFLOAT2& GetTranslation() const
			{
				return m_vTranslation;
			}

			/// <summary>
			/// Translates the component.
			/// </summary>
			/// <param name="a_vTranslation">The movement.</param>
			void SetTranslation(const DirectX::XMFLOAT2& a_vTranslation)
			{
				m_vTranslation = a_vTranslation;
			}

			/// <summary>
			/// Translates the component.
			/// </summary>
			/// <param name="a_vTranslation">The movement.</param>
			void Translate(const DirectX::XMFLOAT2& a_vTranslation);

			bool HasGravity() const
			{
				return m_bHasGravity;
			}

			void SetHasGravity(bool a_bHasGravity)
			{
				m_bHasGravity = a_bHasGravity;
			}
		protected:
			/// <summary>
			/// Updates the components.
			/// </summary>
			/// <param name="a_fDeltaTime">Delta time.</param>
			void UpdateRealtime(float a_fDeltaTime, UpdateTime a_UpdateTime);

			bool m_bHasGravity = false;
			DirectX::XMFLOAT2 m_vTranslation = {};
		};
	}
}