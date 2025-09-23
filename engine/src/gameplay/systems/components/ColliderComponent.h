#pragma once

#include "gameplay/systems/components/Component.h"

#include <DirectXMath.h>

namespace gallus
{
	namespace gameplay
	{
		//---------------------------------------------------------------------
		// ColliderComponent
		//---------------------------------------------------------------------
		class ColliderComponent : public Component
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

			/// <summary>
			/// Sets the offset of the collider.
			/// </summary>
			/// <param name="a_vOffset">A XMFLOAT2 containing the offset.</param>
			void SetOffset(const DirectX::XMFLOAT2& a_vOffset);

			/// <summary>
			/// Sets the size of the collider.
			/// </summary>
			/// <param name="a_vSize">A XMFLOAT2 containing the size.</param>
			void SetSize(const DirectX::XMFLOAT2& a_vSize);

			/// <summary>
			/// Retrieves the offset of the collider.
			/// </summary>
			/// <returns>A XMFLOAT2 containing the offset.</returns>
			const DirectX::XMFLOAT2& GetOffset() const;

			/// <summary>
			/// Retrieves the size of the collider.
			/// </summary>
			/// <returns>A XMFLOAT2 containing the size.</returns>
			const DirectX::XMFLOAT2& GetSize() const;
		protected:
			DirectX::XMFLOAT2 m_vOffset = { 0, 0 };
			DirectX::XMFLOAT2 m_vSize = { 1, 1 };
		};
	}
}