#pragma once

#include "graphics/dx12/DX12PCH.h"
#include "graphics/dx12/ShaderDefs.h"
#include "graphics/dx12/DX12Resource.h"
#include "graphics/dx12/Transform.h"

namespace gallus::graphics::dx12
{
	class CommandList;

	//---------------------------------------------------------------------
	// DirectionalLight
	//---------------------------------------------------------------------
	/// <summary>
	/// Represents a directional light in the engine.
	/// </summary>
	class DirectionalLight : public DX12Resource
	{
	public:
		/// <summary>
		/// Constructs an empty DirectionalLight resource.
		/// </summary>
		DirectionalLight() : DX12Resource()
		{
			m_DirectionalLightData.LightDirection = { 0, 0, 0 };
			m_DirectionalLightData.LightColor = { 1, 1, 1, 1 };
			m_DirectionalLightData.AmbientIntensity = 1.5f;

			m_AssetType = resources::AssetType::None;
		}

		void Bind(std::shared_ptr<CommandList> a_CommandList);

		/// <summary>
		/// Returns whether the resource is a valid resource.
		/// </summary>
		/// <returns>True if the resource was valid, false otherwise.</returns>
		bool IsValid() const override
		{
			return m_pResource;
		}

		const DirectionalLightData& GetDirectionalLightData() const
		{
			return m_DirectionalLightData;
		}

		const Transform& GetTransform() const
		{
			return m_Transform;
		}
	protected:
		Transform m_Transform;
		DirectionalLightData m_DirectionalLightData;

		BEGIN_SERIALIZE_PARENT(DirectionalLight, resources::EngineResource)
		 	SERIALIZE_FIELD(m_DirectionalLightData.DirectionalLightEnabled, "Enabled", ".",
		 		.type = FieldSerializationType::FieldSerializationType_LongSwitch)
		 	SERIALIZE_FIELD(m_Transform.m_vRotation, "Light Direction", ".",
		 		.type = FieldSerializationType::FieldSerializationType_Quaternion)
		 	SERIALIZE_FIELD(m_DirectionalLightData.LightColor, "Light Color", "",
		 		.type = gallus::FieldSerializationType::FieldSerializationType_Color)
		 	SERIALIZE_FIELD(m_DirectionalLightData.AmbientIntensity, "Ambient Intensity", "",
		 		.type = FieldSerializationType::FieldSerializationType_Float,
		 		.step = 0.1f)
			EXPOSE_GIZMO(m_Transform, EditorGizmoType::EditorGizmoType_Transform)
		END_SERIALIZE(DirectionalLight)
	};
}