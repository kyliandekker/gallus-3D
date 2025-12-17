#pragma once

#include "gameplay/systems/components/Component.h"

// external
#include <DirectXMath.h>
#include <array>
#include <set>

#ifdef _EDITOR
// editor
#include "editor/EditorExpose.h"
#endif // _EDITOR

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

			BEGIN_EXPOSE_FIELDS_PARENT(ColliderComponent, Component)
				EXPOSE_FIELD(ColliderComponent, m_vOffset, "Offset", (FieldOptions{ .type = EditorFieldWidgetType::Vector2Field, .description = "The local offset of the collider relative to the object�s pivot or position. Adjusts where the collider is positioned without moving the object itself." }))
				EXPOSE_FIELD(ColliderComponent, m_vSize, "Size", (FieldOptions{ .type = EditorFieldWidgetType::Vector2Field, .description = "The size of the collider in local space. Determines the width and height of the collision area." }))
			END_EXPOSE_FIELDS(ColliderComponent)
			BEGIN_EXPOSE_GIZMOS(ColliderComponent)
			END_EXPOSE_GIZMOS(ColliderComponent)
			END_EXPOSE_TO_EDITOR(ColliderComponent)
		};
	}
}