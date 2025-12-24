#include "DirectionalLight.h"

// graphics
#include "graphics/dx12/CommandList.h"
#include "graphics/dx12/DX12System.h"

namespace gallus
{
	namespace graphics
	{
		namespace dx12
		{
			//---------------------------------------------------------------------
			// DirectionalLight
			//---------------------------------------------------------------------
			void DirectionalLight::Bind(std::shared_ptr<CommandList> a_CommandList)
			{
				if (!m_pResource)
				{
					CreateResource(CD3DX12_RESOURCE_DESC::Buffer(sizeof(DirectionalLightData)), "Directional Light", CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD), D3D12_RESOURCE_STATE_GENERIC_READ, nullptr);
					return;
				}

				// Map the constant buffer to update it with material data
				void* mappedData;
				CD3DX12_RANGE readRange(0, 0);
				m_pResource->Map(0, &readRange, &mappedData);
				memcpy(mappedData, &m_DirectionalLightData, sizeof(DirectionalLightData));
				m_pResource->Unmap(0, nullptr);

				a_CommandList->GetCommandList()->SetGraphicsRoot32BitConstants(
					RootParameters::DIRECTIONAL_LIGHT,
					sizeof(DirectionalLightData) / 4,
					&m_DirectionalLightData,
					0
				);
			}
		}
	}
}