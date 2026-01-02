#pragma once

#include "graphics/dx12/DX12PCH.h"
#include "graphics/dx12/ShaderDefs.h"
#include "graphics/dx12/DX12Resource.h"

namespace gallus
{
	namespace graphics
	{
		namespace dx12
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
			protected:
				DirectionalLightData m_DirectionalLightData;

				BEGIN_EXPOSABLE_PARENT(DirectionalLight, resources::EngineResource)
				 	EXPOSE_FIELD(m_DirectionalLightData.DirectionalLightEnabled, "Enabled", ".",
				 		.type = EditorFieldWidgetType::EditorFieldWidgetType_LongSwitch)
				 	EXPOSE_FIELD(m_DirectionalLightData.LightDirection, "Light Direction", ".",
				 		.type = EditorFieldWidgetType::EditorFieldWidgetType_Vector3)
				 	EXPOSE_FIELD(m_DirectionalLightData.LightColor, "Light Color", "",
				 		.type = gallus::EditorFieldWidgetType::EditorFieldWidgetType_Color)
				 	EXPOSE_FIELD(m_DirectionalLightData.AmbientIntensity, "Ambient Intensity", "",
				 		.type = EditorFieldWidgetType::EditorFieldWidgetType_Float,
				 		.step = 0.1f)
				END_EXPOSABLE(DirectionalLight)
			};
		}
	}
}