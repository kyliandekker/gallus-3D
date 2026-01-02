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

				Material Instantiate() const;

				/// <summary>
				/// Loads and compiles Materials using logical resource names, resolving them through the engine�s resource system.
				/// </summary>
				/// <param name="a_sName">The resource name of the Material.</param>
				/// <returns>True if loading and compilation were successful, false otherwise.</returns>
				bool LoadByName(const std::string& a_sName) override;

				/// <summary>
				/// Loads and compiles Materials using logical resource names, resolving them through the engine�s resource system.
				/// </summary>
				/// <param name="a_sName">The resource name of the Material.</param>
				/// <returns>True if loading and compilation were successful, false otherwise.</returns>
				bool LoadByNameEmpty(const std::string& a_sName);
#ifdef _LOAD_BY_PATH
				/// <summary>
				/// Loads and compiles Materials directly from file paths.
				/// </summary>
				/// <param name="a_MaterialPath">The file path to the Material source.</param>
				/// <returns>True if loading and compilation were successful, false otherwise.</returns>
				bool LoadByPath(const fs::path& a_MaterialPath) override;
#endif // _LOAD_BY_PATH

				void LoadMetaData();

				void SetColor(DirectX::XMFLOAT4 a_vColor)
				{
					m_MaterialData.DiffuseColor = a_vColor;
				}

				void SetEnableLighting(bool a_bEnableLighting)
				{
					m_MaterialData.EnableLighting = static_cast<uint32_t>(a_bEnableLighting);
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
protected:
				MaterialData m_MaterialData;

				BEGIN_SERIALIZE_PARENT(Material, resources::EngineResource)
				 	SERIALIZE_FIELD(m_MaterialData.EnableLighting, "Enable Lighting", "",
				 		.type = gallus::FieldSerializationType::FieldSerializationType_LongSwitch)
				 	SERIALIZE_FIELD(m_MaterialData.DiffuseColor, "Diffuse Color", "",
				 		.type = gallus::FieldSerializationType::FieldSerializationType_Color)
				 	SERIALIZE_FIELD(m_MaterialData.Metallic, "Metallic", "",
				 		.type = gallus::FieldSerializationType::FieldSerializationType_Float)
				 	SERIALIZE_FIELD(m_MaterialData.Smoothness, "Smoothness", "",
				 		.type = gallus::FieldSerializationType::FieldSerializationType_Float)
				END_SERIALIZE(Material)
			};
		}
	}
}