#pragma once

#include "gameplay/systems/components/Component.h"

namespace gallus::gameplay
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
		/// Sets the health.
		/// </summary>
		/// <param name="a_fDamage">The health.</param>
		void TakeDamage(float a_fDamage);
	protected:
		/// <summary>
		/// Updates the components.
		/// </summary>
		/// <param name="a_fDeltaTime">Delta time.</param>
		void UpdateRealtime(float a_fDeltaTime, UpdateTime a_UpdateTime) override;

		float m_fHealth = 100;
		float m_fMaxHealth = 100;

		BEGIN_SERIALIZE_PARENT(HealthComponent, Component)
			SERIALIZE_FIELD(m_fHealth, "Health", "The current health of the entity. Represents how much damage it can take before being destroyed or dying.",
				.type = FieldSerializationType::FieldSerializationType_Float,
				.min = "0",
				.max = "1000")
			SERIALIZE_FIELD(m_fMaxHealth, "Max Health", "The maximum health the entity can have.",
				.type = FieldSerializationType::FieldSerializationType_Float,
				.min = "0",
				.max = "1000")
		END_SERIALIZE(HealthComponent)
	};
}