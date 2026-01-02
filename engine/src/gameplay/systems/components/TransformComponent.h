#pragma once

#include "gameplay/systems/components/Component.h"

// graphics
#include "graphics/dx12/Transform.h"

namespace gallus
{
	namespace gameplay
	{
		//---------------------------------------------------------------------
		// TransformComponent
		//---------------------------------------------------------------------
		class TransformComponent : public Component
		{
		public:
			/// <summary>
			/// Retrieves the transform.
			/// </summary>
			/// <returns>Reference to the transform used in the transform component.</returns>
			graphics::dx12::Transform& GetTransform();

			/// <summary>
			/// Translates the component.
			/// </summary>
			/// <param name="a_vTranslation">The movement.</param>
			void Translate(const DirectX::XMFLOAT3& a_vTranslation);
		private:
			/// <summary>
			/// Updates the components.
			/// </summary>
			/// <param name="a_fDeltaTime">Delta time.</param>
			void UpdateRealtime(float a_fDeltaTime, UpdateTime a_UpdateTime);

			graphics::dx12::Transform m_Transform;
			DirectX::XMFLOAT3 m_vTranslation = {};

			BEGIN_EXPOSABLE_PARENT(TransformComponent, Component)
				EXPOSE_FIELD(m_Transform, "Transform", "",
					.type = EditorFieldWidgetType::EditorFieldWidgetType_Object)
				// EXPOSE_GIZMO(m_Transform, EditorGizmoType::EditorGizmoType_Transform)
			END_EXPOSABLE(TransformComponent)
		};
	}
}