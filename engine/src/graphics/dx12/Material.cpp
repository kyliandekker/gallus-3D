#include "Material.h"

// core
#include "core/Data.h"

// graphics
#include "graphics/dx12/CommandList.h"
#include "graphics/dx12/DX12System.h"

// utils
#include "utils/file_abstractions.h"

// resources
#include "resources/SrcData.h"

#include "core/DataStream.h"

namespace gallus
{
	namespace graphics
	{
		namespace dx12
		{
			//---------------------------------------------------------------------
			// Material
			//---------------------------------------------------------------------
			Material Material::Instantiate() const
			{
				Material mat;
				mat.LoadByName(m_sName + "_copy");
				mat.m_MaterialData = mat.m_MaterialData;
				return mat;
			}

			//---------------------------------------------------------------------
			bool Material::LoadByName(const std::string& a_sName)
			{
				if (!DX12Resource::LoadByName(a_sName))
				{
					return false;
				}

				m_AssetType = resources::AssetType::Material;

				return true;
			}

			//---------------------------------------------------------------------
			bool Material::LoadByNameEmpty(const std::string& a_sName)
			{
				if (!DX12Resource::LoadByName(a_sName))
				{
					return false;
				}

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

				if (!CreateResource(bufferDesc, a_sName, heapProps, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr))
				{
					return false;
				}

				LoadMetaData();

				// Map the constant buffer to update it with material data
				D3D12_RANGE readRange = { 0, 0 };  // We don't need to read, so set to zero
				void* mappedData;
				m_pResource->Map(0, &readRange, &mappedData);
				memcpy(mappedData, &m_MaterialData, sizeof(m_MaterialData));  // Copy data into the buffer
				m_pResource->Unmap(0, nullptr);

				m_AssetType = resources::AssetType::Material;

				return true;
			}

#ifdef _LOAD_BY_PATH
			//---------------------------------------------------------------------
			bool Material::LoadByPath(const fs::path& a_MaterialPath)
			{
				if (!DX12Resource::LoadByPath(a_MaterialPath))
				{
					return false;
				}

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

				if (!CreateResource(bufferDesc, m_sName, heapProps, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr))
				{
					return false;
				}

				LoadMetaData();

				// Map the constant buffer to update it with material data
				D3D12_RANGE readRange = { 0, 0 };  // We don't need to read, so set to zero
				void* mappedData;
				m_pResource->Map(0, &readRange, &mappedData);
				memcpy(mappedData, &m_MaterialData, sizeof(m_MaterialData));  // Copy data into the buffer
				m_pResource->Unmap(0, nullptr);

				m_AssetType = resources::AssetType::Material;
				return true;
			}
#endif // _LOAD_BY_PATH

			//---------------------------------------------------------------------
			void Material::LoadMetaData()
			{
				if (m_Path.empty())
				{
					return;
				}

				core::Data data;
				file::LoadFile(m_Path.generic_string(), data);
				resources::SrcData srcData(data);

				if (!srcData.IsValid())
				{
					return;
				}

				DeserializeEditorExposable(this, srcData);
			}

			//---------------------------------------------------------------------
			void Material::Bind(std::shared_ptr<CommandList> a_CommandList)
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

				a_CommandList->GetCommandList()->SetGraphicsRootConstantBufferView(RootParameters::MATERIAL, m_pResource->GetGPUVirtualAddress());
			}
		}
	}
}