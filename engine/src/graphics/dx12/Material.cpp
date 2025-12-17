#include "Material.h"

// core
#include "core/Data.h"

// graphics
#include "graphics/dx12/CommandList.h"
#include "graphics/dx12/DX12System.h"

// utils
#include "utils/file_abstractions.h"


namespace gallus
{
	namespace graphics
	{
		namespace dx12
		{
			//---------------------------------------------------------------------
			// Material
			//---------------------------------------------------------------------
			bool Material::LoadByName(const std::string& a_sName)
			{
				if (!DX12Resource::LoadByName(a_sName))
				{
					return false;
				}

				m_AssetType = resources::AssetType::Material;
				return false;
			}

#ifdef _LOAD_BY_PATH
			//---------------------------------------------------------------------
			bool Material::LoadByPath(const fs::path& a_MaterialPath)
			{
				return false;
			}
#endif // _LOAD_BY_PATH

			//---------------------------------------------------------------------
			void Material::Bind(std::shared_ptr<CommandList> a_CommandList)
			{
				// Map the constant buffer to update it with material data
				D3D12_RANGE readRange = { 0, 0 };  // We don't need to read, so set to zero
				void* mappedData;
				m_pResource->Map(0, &readRange, &mappedData);
				memcpy(mappedData, &m_MaterialData, sizeof(m_MaterialData));  // Copy data into the buffer
				m_pResource->Unmap(0, nullptr);

				a_CommandList->GetCommandList()->SetGraphicsRootConstantBufferView(RootParameters::MATERIAL, m_pResource->GetGPUVirtualAddress());
			}
		}
	}
}