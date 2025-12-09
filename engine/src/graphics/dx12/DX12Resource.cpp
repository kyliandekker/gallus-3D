#include "DX12Resource.h"

// core
#include "core/Engine.h"

// graphics
#include "graphics/dx12/DX12System2D.h"
#include "graphics/dx12/CommandList.h"

// logger
#include "logger/Logger.h"

namespace gallus
{
	namespace graphics
	{
		namespace dx12
		{
			//---------------------------------------------------------------------
			// DX12Resource
			//---------------------------------------------------------------------
			bool DX12Resource::Destroy()
			{
				if (!EngineResource::Destroy())
				{
					return false;
				}
				
				if (m_pResource)
				{
					m_pResource.Reset();
				}
				m_FormatSupport = {};
				m_wsName = L"";
				m_CurrentState = D3D12_RESOURCE_STATE_COMMON;
				return true;
			}

			//---------------------------------------------------------------------
			bool DX12Resource::LoadByName(const std::string& a_sName)
			{
				if (!EngineResource::LoadByName(a_sName))
				{
					return false;
				}
				m_wsName = std::wstring(m_sName.begin(), m_sName.end());

				return true;
			}

			//---------------------------------------------------------------------
			bool DX12Resource::LoadByName(const std::string& a_sName, const D3D12_RESOURCE_DESC& a_ResourceDesc, const D3D12_HEAP_PROPERTIES& a_Heap, const D3D12_RESOURCE_STATES a_ResourceState, const D3D12_CLEAR_VALUE* a_pOptimizedClearValue)
			{
				if (!LoadByName(a_sName))
				{
					return false;
				}

				Microsoft::WRL::ComPtr<ID3D12Device2>& device = core::ENGINE->GetDX12().GetDevice();
				if (device->CreateCommittedResource(
					&a_Heap,
					D3D12_HEAP_FLAG_NONE,
					&a_ResourceDesc,
					a_ResourceState,
					a_pOptimizedClearValue,
					IID_PPV_ARGS(&m_pResource)
					))
				{
					LOGF(LOGSEVERITY_ERROR, LOG_CATEGORY_DX12, "Failed creating committed resource: \"%s\".", a_sName.c_str());
					return false;
				}

				if (!m_pResource)
				{
					LOGF(LOGSEVERITY_ERROR, LOG_CATEGORY_DX12, "Resource is null: \"%s\".", a_sName.c_str());
					return false;
				}

				m_pResource->SetName(m_wsName.c_str());

				CheckFeatureSupport();

				return true;
			}

#ifdef _LOAD_BY_PATH
			//---------------------------------------------------------------------
			bool DX12Resource::LoadByPath(const fs::path& a_Path)
			{
				if (!EngineResource::LoadByPath(a_Path))
				{
					return false;
				}
				m_wsName = std::wstring(m_sName.begin(), m_sName.end());

				return true;
			}

			//---------------------------------------------------------------------
			bool DX12Resource::LoadByPath(const fs::path& a_Path, const D3D12_RESOURCE_DESC& a_ResourceDesc, const D3D12_HEAP_PROPERTIES& a_Heap, const D3D12_RESOURCE_STATES a_ResourceState, const D3D12_CLEAR_VALUE* a_pOptimizedClearValue)
			{
				if (!EngineResource::LoadByPath(a_Path))
				{
					return false;
				}

				Microsoft::WRL::ComPtr<ID3D12Device2>& device = core::ENGINE->GetDX12().GetDevice();
				if (device->CreateCommittedResource(
					&a_Heap,
					D3D12_HEAP_FLAG_NONE,
					&a_ResourceDesc,
					a_ResourceState,
					a_pOptimizedClearValue,
					IID_PPV_ARGS(&m_pResource)
				))
				{
					LOGF(LOGSEVERITY_ERROR, LOG_CATEGORY_DX12, "Failed creating committed resource: \"%s\".", a_Path.generic_string().c_str());
					return false;
				}

				if (!m_pResource)
				{
					LOGF(LOGSEVERITY_ERROR, LOG_CATEGORY_DX12, "Resource is null: \"%s\".", a_Path.generic_string().c_str());
					return false;
				}

				m_pResource->SetName(m_wsName.c_str());

				CheckFeatureSupport();

				return true;
			}
#endif

			//---------------------------------------------------------------------
			bool DX12Resource::IsValid() const
			{
				return m_pResource;
			}

			//---------------------------------------------------------------------
			Microsoft::WRL::ComPtr<ID3D12Resource>& DX12Resource::GetResource()
			{
				return m_pResource;
			}

			//---------------------------------------------------------------------
			D3D12_RESOURCE_DESC DX12Resource::GetResourceDesc() const
			{
				D3D12_RESOURCE_DESC resDesc = {};
				if (m_pResource)
				{
					resDesc = m_pResource->GetDesc();
				}

				return resDesc;
			}

			//---------------------------------------------------------------------
			bool DX12Resource::CheckFormatSupport(D3D12_FORMAT_SUPPORT1 a_FormatSupport) const
			{
				return (m_FormatSupport.Support1 & a_FormatSupport) != 0;
			}

			//---------------------------------------------------------------------
			bool DX12Resource::CheckFormatSupport(D3D12_FORMAT_SUPPORT2 a_FormatSupport) const
			{
				return (m_FormatSupport.Support2 & a_FormatSupport) != 0;
			}

			//---------------------------------------------------------------------
			bool DX12Resource::CreateResource(const D3D12_RESOURCE_DESC& a_ResourceDesc, const std::string& a_sName, const D3D12_HEAP_PROPERTIES& a_Heap, const D3D12_RESOURCE_STATES a_ResourceState, const D3D12_CLEAR_VALUE* a_pOptimizedClearValue)
			{
				if (m_pResource)
				{
					// Resources that cannot be destroyed cannot be overridden.
					if (!m_bIsDestroyable)
					{
						return false;
					}
					m_pResource.Reset();
				}

				Microsoft::WRL::ComPtr<ID3D12Device2>& device = core::ENGINE->GetDX12().GetDevice();

				m_sName = a_sName;
				if (device->CreateCommittedResource(
					&a_Heap,
					D3D12_HEAP_FLAG_NONE,
					&a_ResourceDesc,
					a_ResourceState,
					a_pOptimizedClearValue,
					IID_PPV_ARGS(&m_pResource)
					))
				{
					LOGF(LOGSEVERITY_ERROR, LOG_CATEGORY_DX12, "Failed creating committed resource: \"%s\".", a_sName.c_str());
					return false;
				}

				m_wsName = std::wstring(m_sName.begin(), m_sName.end());
				m_pResource->SetName(m_wsName.c_str());

				CheckFeatureSupport();

				return true;
			}

			//---------------------------------------------------------------------
			void DX12Resource::SetResource(Microsoft::WRL::ComPtr<ID3D12Resource> a_pResource)
			{
				// TODO: Fix. This is not safe.

				// Resources that cannot be destroyed cannot be overridden.
				if (!m_bIsDestroyable)
				{
					return;
				}

				if (m_pResource)
				{
					m_pResource.Reset();
					m_pResource = nullptr;
				}

				m_pResource = a_pResource;
			}

			//---------------------------------------------------------------------
			void DX12Resource::CheckFeatureSupport()
			{
				Microsoft::WRL::ComPtr<ID3D12Device2>& device = core::ENGINE->GetDX12().GetDevice();

				const D3D12_RESOURCE_DESC desc = m_pResource->GetDesc();
				m_FormatSupport.Format = desc.Format;
				if (FAILED(device->CheckFeatureSupport(D3D12_FEATURE_FORMAT_SUPPORT, &m_FormatSupport,
					sizeof(D3D12_FEATURE_DATA_FORMAT_SUPPORT))))
				{
					LOGF(LOGSEVERITY_ERROR, LOG_CATEGORY_DX12, "Failed checking feature support.");
					return;
				}
			}

			//---------------------------------------------------------------------
			void DX12Resource::Transition(std::shared_ptr<CommandList> a_pCommandList, D3D12_RESOURCE_STATES a_NewState)
			{
				if (m_CurrentState != a_NewState)
				{
					CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
						m_pResource.Get(),
						m_CurrentState, 
						a_NewState);
					a_pCommandList->GetCommandList()->ResourceBarrier(1, &barrier);

					m_CurrentState = a_NewState;
				}
			}
		}
	}
}