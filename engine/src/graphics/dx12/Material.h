#pragma once

#include "graphics/dx12/DX12PCH.h"
#include "graphics/dx12/DX12Resource.h"

// graphics
#include "graphics/dx12/ShaderDefs.h"

namespace gallus::graphics::dx12
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
		Material Instantiate() const;

		/// <summary>
		/// Loads the mesh data.
		/// </summary>
		/// <param name="a_Data">A vector containing data in bytes.</param>
		/// <returns>True when loading was successful, false otherwise.</returns>
		bool LoadData(const core::Data& a_Data) override;

		/// <summary>
		/// Sets the color of the material.
		/// </summary>
		/// <param name="a_vColor">A vector containing the color.</param>
		void SetColor(DirectX::XMFLOAT4 a_vColor)
		{
			m_MaterialData.DiffuseColor = a_vColor;
		}

		/// <summary>
		/// Enables or disables global illumination.
		/// </summary>
		/// <param name="a_bEnableLighting">True to enable, false to disable.</param>
		void SetEnableLighting(bool a_bEnableLighting)
		{
			m_MaterialData.EnableLighting = static_cast<uint32_t>(a_bEnableLighting);
		}

		/// <summary>
		/// Binds the material to the current mesh.
		/// </summary>
		/// <param name="a_pCommandList">The command list used to issue GPU draw calls.</param>
		void Bind(std::shared_ptr<CommandList> a_pCommandList);

		/// <summary>
		/// Returns whether the resource is a valid resource.
		/// </summary>
		/// <returns>True if the resource was valid, false otherwise.</returns>
		bool IsValid() const override
		{
			return m_pResource;
		}
	protected:
		MaterialData m_MaterialData = {};

		BEGIN_SERIALIZE_PARENT(Material, resources::EngineResource)
		 	SERIALIZE_FIELD(m_MaterialData.EnableLighting, "Enable Lighting", "",
		 		.type = FieldSerializationType::FieldSerializationType_LongSwitch,
				.serializationMethod = SerializationMethod::SerializationMethod_File)
		 	SERIALIZE_FIELD(m_MaterialData.DiffuseColor, "Diffuse Color", "",
		 		.type = FieldSerializationType::FieldSerializationType_Color,
				.serializationMethod = SerializationMethod::SerializationMethod_File)
		 	SERIALIZE_FIELD(m_MaterialData.Metallic, "Metallic", "",
		 		.type = FieldSerializationType::FieldSerializationType_Float,
				.serializationMethod = SerializationMethod::SerializationMethod_File)
		 	SERIALIZE_FIELD(m_MaterialData.Smoothness, "Smoothness", "",
		 		.type = FieldSerializationType::FieldSerializationType_Float,
				.serializationMethod = SerializationMethod::SerializationMethod_File)
		END_SERIALIZE(Material)
	};
}