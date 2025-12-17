#pragma once

#include "graphics/dx12/DX12PCH.h"
#include "graphics/dx12/DX12Resource.h"

namespace gallus
{
	namespace graphics
	{
		namespace dx12
		{
			struct MaterialData
			{
				DirectX::XMFLOAT3 DiffuseColor = { 0, 0, 0 };	// Base color of the material
				float Metallic = 0.0f;                   // 0 = non-metallic, 1 = metallic
				float Smoothness = 0.0f;                 // 0 = rough, 1 = smooth
			};

			class CommandList;

			//---------------------------------------------------------------------
			// Material
			//---------------------------------------------------------------------
			/// <summary>
			/// Represents a material in the engine.
			/// </summary>
			class Material : public DX12Resource
			{
			public:
				/// <summary>
				/// Constructs an empty Material resource.
				/// </summary>
				Material() : DX12Resource()
				{
					m_AssetType = resources::AssetType::Material;
				}

				/// <summary>
				/// Loads and compiles Materials using logical resource names, resolving them through the engine�s resource system.
				/// </summary>
				/// <param name="a_sName">The resource name of the Material.</param>
				/// <returns>True if loading and compilation were successful, false otherwise.</returns>
				bool LoadByName(const std::string& a_sName) override;

#ifdef _LOAD_BY_PATH
				/// <summary>
				/// Loads and compiles Materials directly from file paths.
				/// </summary>
				/// <param name="a_MaterialPath">The file path to the Material source.</param>
				/// <returns>True if loading and compilation were successful, false otherwise.</returns>
				bool LoadByPath(const fs::path& a_MaterialPath) override;
#endif

				void Bind(std::shared_ptr<CommandList> a_CommandList);

				/// <summary>
				/// Returns whether the resource is a valid resource.
				/// </summary>
				/// <returns>True if the resource was valid, false otherwise.</returns>
				bool IsValid() const override
				{
					return false;
				}
protected:
				MaterialData m_MaterialData;
#ifdef _EDITOR
				BEGIN_EXPOSE_FIELDS_PARENT(Material, resources::EngineResource)
				EXPOSE_FIELD(Material, m_MaterialData.DiffuseColor, "Diffuse Color",
					{ .type = gallus::EditorFieldWidgetType::Vector3Field })
				EXPOSE_FIELD(Material, m_MaterialData.Metallic, "Metallic",
					{ .type = gallus::EditorFieldWidgetType::DragFloat })
				EXPOSE_FIELD(Material, m_MaterialData.Smoothness, "Smoothness",
					{ .type = gallus::EditorFieldWidgetType::DragFloat })
				END_EXPOSE_FIELDS(Material)
				BEGIN_EXPOSE_GIZMOS(Material)
				END_EXPOSE_GIZMOS(Material)
				END_EXPOSE_TO_EDITOR(Material)
#endif
			};
		}
	}
}