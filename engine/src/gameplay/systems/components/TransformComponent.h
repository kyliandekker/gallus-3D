#pragma once

// external
#include <DirectXMath.h>

#include "gameplay/systems/components/Component.h"

// graphics
#include "graphics/dx12/Transform.h"

namespace gallus::gameplay
{
	class TransformSystem;

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

		~TransformComponent();

		/// <summary>
		/// Initializes the component after deserialization.
		/// </summary>
		void Init() override;

		/// <summary>
		/// Translates the component.
		/// </summary>
		/// <param name="a_vTranslation">The movement.</param>
		void Translate(const DirectX::XMFLOAT3& a_vTranslation);

		int32_t GetTransformIndex() const
		{
			return m_iTransformIndex;
		}
	private:
		/// <summary>
		/// Updates the components.
		/// </summary>
		/// <param name="a_fDeltaTime">Delta time.</param>
		void UpdateRealtime(float a_fDeltaTime, UpdateTime a_UpdateTime);

		graphics::dx12::Transform m_Transform;
		DirectX::XMFLOAT3 m_vTranslation = {};

		int16_t m_iTransformIndex = 0; // Default is 0 because 0 is always allocated.

		BEGIN_SERIALIZE_PARENT(TransformComponent, Component)
			SERIALIZE_FIELD(m_Transform, "Transform", "",
				.type = FieldSerializationType::FieldSerializationType_Object)
			EXPOSE_GIZMO(m_Transform, EditorGizmoType::EditorGizmoType_Transform)
		END_SERIALIZE(TransformComponent)

		// cache
		gameplay::TransformSystem* m_pTransformSystem = nullptr;
	};
}