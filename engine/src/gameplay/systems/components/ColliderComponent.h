#pragma once

#include "gameplay/systems/components/Component.h"

// external
#include <DirectXMath.h>
#include <array>
#include <set>

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
			void IgnoreEntity(const gameplay::EntityID& a_EntityID);
		protected:
			/// <summary>
			/// Updates the system's component.
			/// </summary>
			// <param name="a_fDeltaTime">The time it took since last frame.</param>
			void UpdateRealtime(float a_fDeltatime, UpdateTime a_UpdateTime) override;

			DirectX::XMFLOAT2 m_vOffset = { 0, 0 };
			DirectX::XMFLOAT2 m_vSize = { 1, 1 };

			std::set<gameplay::EntityID> m_aEntitiesToIgnore;

				BEGIN_EXPOSABLE_PARENT(ColliderComponent, Component)
					EXPOSE_FIELD(m_vOffset, "Offset", "The local offset of the collider relative to the object's pivot or position. Adjusts where the collider is positioned without moving the object itself.",
						.type = EditorFieldWidgetType::EditorFieldWidgetType_Vector2)
					EXPOSE_FIELD(m_vSize, "Size", "The size of the collider in local space. Determines the width and height of the collision area.",
						.type = EditorFieldWidgetType::EditorFieldWidgetType_Vector2)
				END_EXPOSABLE(ColliderComponent)
		};
	}
}