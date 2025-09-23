#pragma once

#include "gameplay/systems/components/Component.h"

namespace gallus
{
	namespace gameplay
	{
		//---------------------------------------------------------------------
		// PlayerComponent
		//---------------------------------------------------------------------
		class PlayerComponent : public Component
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
			/// Retrieves the movement speed.
			/// </summary>
			/// <returns>Float representing the movement speed.</returns>
			float GetSpeed() const
			{
				return m_fSpeed;
			}

			/// <summary>
			/// Sets the movement speed.
			/// </summary>
			/// <param name="a_fSpeed">The movement speed.</param>
			void SetSpeed(float a_fSpeed)
			{
				m_fSpeed = a_fSpeed;
			}

			/// <summary>
			/// Updates the components.
			/// </summary>
			/// <param name="a_fDeltaTime">Delta time.</param>
			void Update(float a_fDeltaTime) override;
		protected:
			float m_fSpeed = 200;
		};
	}
}