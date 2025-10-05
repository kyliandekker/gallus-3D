#pragma once

#include "gameplay/systems/components/Component.h"

namespace gallus
{
	namespace gameplay
	{
		//---------------------------------------------------------------------
		// HealthComponent
		//---------------------------------------------------------------------
		class HealthComponent : public Component
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
			/// Deserializes data from a json document and loads it into the component.
			/// </summary>
			/// <param name="a_Document">The json document that contains the data.</param>
			/// <param name="a_Allocator">The allocator used by the json document.</param>
			void Deserialize(const rapidjson::Value& a_Document, rapidjson::Document::AllocatorType& a_Allocator) override;

			/// <summary>
			/// Retrieves the health.
			/// </summary>
			/// <returns>Float representing the health.</returns>
			float GetHealth() const
			{
				return m_fHealth;
			}

			/// <summary>
			/// Sets the health.
			/// </summary>
			/// <param name="a_fHealth">The health.</param>
			void SetHealth(float a_fHealth)
			{
				m_fHealth = a_fHealth;
			}

			/// <summary>
			/// Retrieves the max health.
			/// </summary>
			/// <returns>Float representing the max health.</returns>
			float GetMaxHealth() const
			{
				return m_fMaxHealth;
			}

			/// <summary>
			/// Sets the max health.
			/// </summary>
			/// <param name="a_fMaxHealth">The max health.</param>
			void SetMaxHealth(float a_fMaxHealth)
			{
				m_fMaxHealth = a_fMaxHealth;
			}

			/// <summary>
			/// Updates the components.
			/// </summary>
			/// <param name="a_fDeltaTime">Delta time.</param>
			void UpdateRealtime(float a_fDeltaTime) override;
		protected:
			float m_fHealth = 100;
			float m_fMaxHealth = 100;
		};
	}
}