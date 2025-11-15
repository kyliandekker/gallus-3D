#pragma once

#include "gameplay/systems/components/Component.h"

#include "editor/EditorExpose.h"

// graphics includes
#include "graphics/dx12/DX12Transform.h"

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
			graphics::dx12::DX12Transform& Transform();

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
			/// Retrieves the translation.
			/// </summary>
			/// <returns>Reference to the translation vector.</returns>
			const DirectX::XMFLOAT2& GetTranslation() const
			{
				return m_vTranslation;
			}

			/// <summary>
			/// Translates the component.
			/// </summary>
			/// <param name="a_vTranslation">The movement.</param>
			void SetTranslation(const DirectX::XMFLOAT2& a_vTranslation)
			{
				m_vTranslation = a_vTranslation;
			}

			/// <summary>
			/// Translates the component.
			/// </summary>
			/// <param name="a_vTranslation">The movement.</param>
			void Translate(const DirectX::XMFLOAT2& a_vTranslation);
		private:
			/// <summary>
			/// Updates the components.
			/// </summary>
			/// <param name="a_fDeltaTime">Delta time.</param>
			void UpdateRealtime(float a_fDeltaTime, UpdateTime a_UpdateTime);

			graphics::dx12::DX12Transform m_Transform;
			DirectX::XMFLOAT2 m_vTranslation = {};

			void ShowTransformGizmo();
#ifdef _EDITOR
			BEGIN_EXPOSE_FIELDS(TransformComponent)
				EXPOSE_FIELD(TransformComponent, m_Transform, "Transform", FieldOptions{ .type = EditorFieldWidgetType::Object })
			END_EXPOSE_FIELDS(TransformComponent)
			BEGIN_EXPOSE_GIZMOS(TransformComponent)
				EXPOSE_GIZMO(TransformComponent, m_Transform, GizmoOptions{ EditorGizmoType::Transform })
			END_EXPOSE_GIZMOS(TransformComponent)
			END_EXPOSE_TO_EDITOR(TransformComponent)
#endif
		};
	}
}