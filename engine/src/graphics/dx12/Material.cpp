#include "Material.h"

// core
#include "core/Data.h"

// graphics
#include "graphics/dx12/CommandList.h"
#include "graphics/dx12/DX12System.h"

// resources
#include "resources/SrcData.h"

#include "core/DataStream.h"

// logger
#include "logger/Logger.h"

namespace gallus::graphics::dx12
{
	//---------------------------------------------------------------------
	// Material
	//---------------------------------------------------------------------
	Material Material::Instantiate() const
	{
		Material mat;
		mat.m_MaterialData = mat.m_MaterialData;
		return mat;
	}

	//---------------------------------------------------------------------
	bool Material::LoadData(const core::Data& a_Data)
	{
		CD3DX12_HEAP_PROPERTIES heapProps = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);

		D3D12_RESOURCE_DESC bufferDesc = {};
		bufferDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		bufferDesc.Width = sizeof(MaterialData);  // Size of the Material struct
		bufferDesc.Height = 1;
		bufferDesc.DepthOrArraySize = 1;
		bufferDesc.MipLevels = 1;
		bufferDesc.Format = DXGI_FORMAT_UNKNOWN;
		bufferDesc.SampleDesc.Count = 1;
		bufferDesc.SampleDesc.Quality = 0;
		bufferDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
		bufferDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

		if (!SetResourceData("", bufferDesc, heapProps, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr))
		{
			LOGF(LOGSEVERITY_ERROR, LOG_CATEGORY_DX12, "Failed to load material \"%s\": Could not create DX12 resource.", m_sName.c_str());
			return false;
		}

		DeserializeFields(this, resources::SrcData(a_Data), SerializationMethod::SerializationMethod_File);

		// Map the constant buffer to update it with material data
		D3D12_RANGE readRange = { 0, 0 };  // We don't need to read, so set to zero
		void* mappedData;
		m_pResource->Map(0, &readRange, &mappedData);
		memcpy(mappedData, &m_MaterialData, sizeof(m_MaterialData));  // Copy data into the buffer
		m_pResource->Unmap(0, nullptr);

		m_AssetType = resources::AssetType::Material;
		
		LOGF(LOGSEVERITY_INFO_SUCCESS, LOG_CATEGORY_DX12, "Successfully loaded material \"%s\".", m_sName.c_str());
		return true;
	}

	//---------------------------------------------------------------------
	void Material::Bind(std::shared_ptr<CommandList> a_pCommandList)
	{
		if (!m_pResource)
		{
			return;
		}

		// Map the constant buffer to update it with material data
		D3D12_RANGE readRange = { 0, 0 };  // We don't need to read, so set to zero
		void* mappedData;
		m_pResource->Map(0, &readRange, &mappedData);
		memcpy(mappedData, &m_MaterialData, sizeof(m_MaterialData));
		m_pResource->Unmap(0, nullptr);

		a_pCommandList->GetCommandList()->SetGraphicsRootConstantBufferView(RootParameters::MATERIAL, m_pResource->GetGPUVirtualAddress());
	}
}