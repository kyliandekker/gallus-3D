#pragma once

#include "DX12PCH.h"
#include "resources/EngineResource.h"

// external
#include <string>
#include <filesystem>

namespace gallus::graphics::dx12
{
	inline std::string D3D12_RESOURCE_STATESToString(D3D12_RESOURCE_STATES state)
	{
		return "";
	}

	class CommandList;
	//---------------------------------------------------------------------
	// DX12Resource
	//---------------------------------------------------------------------
	/// <summary>
	/// Wrapper for a DX12 resource, providing utility functions.
	/// </summary>
	class DX12Resource : public resources::EngineResource
	{
	public:
		/// <summary>
		/// Constructs a dx12 resource without any details.
		/// </summary>
		DX12Resource() : resources::EngineResource()
		{}

		/// <summary>
		/// Destroys the dx12 resource.
		/// </summary>
		bool Destroy() override;

		/// <summary>
		/// Loads a resource by name.
		/// </summary>
		/// <param name="a_sName">Name of the resource.</param>
		/// <param name="a_ResourceDesc">Resource description.</param>
		/// <param name="a_Heap">Heap property options.</param>
		/// <param name="a_ResourceState">Resource state it will transition into.</param>
		/// <param name="a_pOptimizedClearValue">Clear value of the resource.</param>
		/// <returns></returns>
		virtual bool SetResourceData(const std::string& a_sName, const D3D12_RESOURCE_DESC& a_ResourceDesc, const D3D12_HEAP_PROPERTIES& a_Heap = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT), const D3D12_RESOURCE_STATES a_ResourceState = D3D12_RESOURCE_STATE_COMMON, const D3D12_CLEAR_VALUE* a_pOptimizedClearValue = nullptr);

		/// <summary>
		/// Returns whether the resource is a valid resource.
		/// </summary>
		/// <returns>True if the resource was valid, false otherwise.</returns>
		bool IsValid() const override;

		/// <summary>
		/// Returns the DX12 resource.
		/// </summary>
		/// <returns>Pointer to the DX12 Resource.</returns>
		Microsoft::WRL::ComPtr<ID3D12Resource>& GetResource();

		/// <summary>
		/// Returns the DX12 resource description.
		/// </summary>
		/// <returns>The resource DX12 description.</returns>
		D3D12_RESOURCE_DESC GetResourceDesc() const;

		/// <summary>
		/// Checks the resource if a format is supported.
		/// </summary>
		/// <param name="a_FormatSupport">Format to check.</param>
		/// <returns>True if the resource supports format, false otherwise.</returns>
		bool CheckFormatSupport(D3D12_FORMAT_SUPPORT1 a_FormatSupport) const;

		/// <summary>
		/// Checks the resource if a format is supported.
		/// </summary>
		/// <param name="a_FormatSupport">Format to check.</param>
		/// <returns>True if the resource supports format, false otherwise.</returns>
		bool CheckFormatSupport(D3D12_FORMAT_SUPPORT2 a_FormatSupport) const;

		/// <summary>
		/// Sets the format support.
		/// </summary>
		void CheckFeatureSupport();

		/// <summary>
		/// Transitions the resource into a new state.
		/// </summary>
		/// <param name="a_pCommandList">The command list.</param>
		/// <param name="a_NewState">The new state.</param>
		void Transition(std::shared_ptr<CommandList> a_pCommandList, D3D12_RESOURCE_STATES a_NewState);
		
		/// <summary>
		/// Loads the data.
		/// </summary>
		/// <param name="a_Data">A vector containing meta data in bytes.</param>
		/// <returns>True when loading was successful, false otherwise.</returns>
		bool LoadData(const core::Data& a_Data) override
		{
			return true;
		}
	protected:
		friend class DX12System;

		// ONLY USED INTERNALLY!!! SUPER DANGEROUS OUTSIDE OF DX12SYSTEM.
		/// <summary>
		/// Sets a DX12 resource.
		/// </summary>
		/// <param name="a_pResource">Resource that will be wrapped.</param>
		void SetResource(Microsoft::WRL::ComPtr<ID3D12Resource> a_pResource);

		Microsoft::WRL::ComPtr<ID3D12Resource> m_pResource = nullptr;
		D3D12_FEATURE_DATA_FORMAT_SUPPORT m_FormatSupport{};
		D3D12_RESOURCE_STATES m_CurrentState = D3D12_RESOURCE_STATE_COMMON;

		BEGIN_SERIALIZE_PARENT(DX12Resource, resources::EngineResource)
		END_SERIALIZE(DX12Resource)
	};
}