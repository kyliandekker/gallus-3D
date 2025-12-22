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
			/// Sets the health.
			/// </summary>
			/// <param name="a_fDamage">The health.</param>
			void TakeDamage(float a_fDamage)
			{
				m_fHealth -= a_fDamage;
				if (m_fHealth < 0)
				{
					m_fHealth = 0;
				}
			}
		protected:
			/// <summary>
			/// Updates the components.
			/// </summary>
			/// <param name="a_fDeltaTime">Delta time.</param>
			void UpdateRealtime(float a_fDeltaTime, UpdateTime a_UpdateTime) override;

			float m_fHealth = 100;
			float m_fMaxHealth = 100;

			BEGIN_EXPOSE_FIELDS_PARENT(HealthComponent, Component)
				EXPOSE_FIELD(HealthComponent, m_fHealth, "Health", "The current health of the entity. Represents how much damage it can take before being destroyed or dying.",
					(FieldOptions{ 
						.type = EditorFieldWidgetType::EditorFieldWidgetType_Float, 
						.min = "0", 
						.max = "1000", 
					}))
				EXPOSE_FIELD(HealthComponent, m_fMaxHealth, "Max Health", "The maximum health the entity can have.",
					(FieldOptions{ 
						.type = EditorFieldWidgetType::EditorFieldWidgetType_Float, 
						.min = "0", 
						.max = "1000", 
					}))
			END_EXPOSE_FIELDS(HealthComponent)
			BEGIN_EXPOSE_GIZMOS(HealthComponent)
			END_EXPOSE_GIZMOS(HealthComponent)
			END_EXPOSE_TO_EDITOR(HealthComponent)
		};
	}
}