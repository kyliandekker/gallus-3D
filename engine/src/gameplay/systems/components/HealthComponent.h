#pragma once

#include "gameplay/systems/components/Component.h"

#include "editor/EditorExpose.h"

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

#ifdef _EDITOR
			/// <summary>
			/// Serialized the component to a json document.
			/// </summary>
			/// <param name="a_Document">The json document that the data will be put into.</param>
			/// <param name="a_Allocator">The allocator used by the json document.</param>
			void Serialize(rapidjson::Value& a_Document, rapidjson::Document::AllocatorType& a_Allocator) const override;
#endif

			/// <summary>
			/// Creates an instance based on source data.
			/// </summary>
			/// <param name="a_SrcData">The source data.</param>
			void Deserialize(const resources::SrcData& a_SrcData) override;

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
		protected:
			/// <summary>
			/// Updates the components.
			/// </summary>
			/// <param name="a_fDeltaTime">Delta time.</param>
			void UpdateRealtime(float a_fDeltaTime, UpdateTime a_UpdateTime) override;

			float m_fHealth = 100;
			float m_fMaxHealth = 100;

#ifdef _EDITOR
			BEGIN_EXPOSE_FIELDS(HealthComponent)
				EXPOSE_FIELD(HealthComponent, m_fHealth, "Health", FieldOptions{ .type = EditorFieldWidgetType::DragFloat, .min = "0", .max = "1000", .description = "The current health of the entity. Represents how much damage it can take before being destroyed or dying." })
				EXPOSE_FIELD(HealthComponent, m_fMaxHealth, "Max Health", FieldOptions{ .type = EditorFieldWidgetType::DragFloat, .min = "0", .max = "1000", .description = "The maximum health the entity can have." })
			END_EXPOSE_FIELDS(HealthComponent)
			BEGIN_EXPOSE_GIZMOS(HealthComponent)
			END_EXPOSE_GIZMOS(HealthComponent)
			END_EXPOSE_TO_EDITOR(HealthComponent)
#endif
		};
	}
}