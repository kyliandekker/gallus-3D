#pragma once

#include "graphics/dx12/DX12PCH.h"
#include "graphics/dx12/DX12Resource.h"

#include <wrl.h>
#include <string>
#include <memory>
#include <cstdint>
#include <glm/vec2.hpp>

#include "utils/file_abstractions.h"

namespace gallus
{
	namespace graphics
	{
		namespace dx12
		{
			class DX12System2D;
			class CommandList;

			//---------------------------------------------------------------------
			// Texture
			//---------------------------------------------------------------------
			/// <summary>
			/// Represents a GPU texture.
			/// </summary>
			class Texture : public DX12Resource
			{
			public:
				/// <summary>
				/// Destroys the texture and sets it to invalid (it still exists in the texture atlas, however).
				/// </summary>
				void Destroy() override;

				/// <summary>
				/// Checks whether the texture supports SRV.
				/// </summary>
				/// <returns>True if the texture supports SRV, otherwise false.</returns>
				bool CheckSRVSupport() const;

				/// <summary>
				/// Checks whether the texture supports RTV.
				/// </summary>
				/// <returns>True if the texture supports RTV, otherwise false.</returns>
				bool CheckRTVSupport() const;

				/// <summary>
				/// Checks whether the texture supports UAV.
				/// </summary>
				/// <returns>True if the texture supports UAV, otherwise false.</returns>
				bool CheckUAVSupport() const;

				/// <summary>
				/// Checks whether the texture supports DSV.
				/// </summary>
				/// <returns>True if the texture supports DSV, otherwise false.</returns>
				bool CheckDSVSupport() const;

				/// <summary>
				/// Retrieves the size of the texture.
				/// </summary>
				/// <returns>Returns a vector representing the width and the height.</returns>
				glm::ivec2 GetSize() const;

				/// <summary>
				/// Creates the SRV for the texture.
				/// </summary>
				/// <param name="a_pCommandList">The command list that will be used.</param>
				/// <returns>True if the srv was successfully created, otherwise false.</returns>
				bool CreateSRV(std::shared_ptr<CommandList> a_pCommandList);

				/// <summary>
				/// Binds the texture to the pipeline (causing it to be rendered).
				/// </summary>
				/// <param name="a_pCommandList">The command list that will be used.</param>
				void Bind(std::shared_ptr<CommandList> a_pCommandList);

				/// <summary>
				/// Unbinds the texture from the pipeline.
				/// </summary>
				/// <param name="a_pCommandList">The command list that will be used.</param>
				void Unbind(std::shared_ptr<CommandList> a_pCommandList);

				/// <summary>
				/// Retrieves the GPU handle of the SRV.
				/// </summary>
				/// <returns>GPU Descriptor handle of the SRV.</returns>
				CD3DX12_GPU_DESCRIPTOR_HANDLE GetGPUHandle();

				/// <summary>
				/// Retrieves the CPU handle of the SRV.
				/// </summary>
				/// <returns>CPU Descriptor handle of the SRV.</returns>
				CD3DX12_CPU_DESCRIPTOR_HANDLE GetCPUHandle();

				/// <summary>
				/// Creates a texture by name but does not fill it with texture data. Useful for render textures.
				/// </summary>
				/// <param name="a_sName">Name of the texture.</param>
				/// <param name="a_Description">Description of the texture.</param>
				/// <param name="a_Heap">Heap properties.</param>
				/// <param name="a_ResourceState">The resource state the texture will be transitioned into after successful creation.</param>
				/// <returns></returns>
				bool LoadByName(const std::string& a_sName, const D3D12_RESOURCE_DESC& a_Description, const D3D12_HEAP_PROPERTIES& a_Heap = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT), const D3D12_RESOURCE_STATES a_ResourceState = D3D12_RESOURCE_STATE_COMMON);

				/// <summary>
				/// Loads a texture by name.
				/// </summary>
				/// <param name="a_sName">Name of the texture.</param>
				/// <param name="a_pCommandList">The command list used for updating resources.</param>
				/// <param name="a_Heap">Heap properties.</param>
				/// <param name="a_ResourceState">The resource state the texture will be transitioned into after successful creation.</param>
				/// <returns></returns>
				bool LoadByName(const std::string& a_sName, std::shared_ptr<CommandList> a_pCommandList, const D3D12_HEAP_PROPERTIES& a_Heap = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT), const D3D12_RESOURCE_STATES a_ResourceState = D3D12_RESOURCE_STATE_COMMON);

				/// <summary>
				/// Loads a texture by path.
				/// </summary>
				/// <param name="a_Path">The path to the texture.</param>
				/// <param name="a_pCommandList">The command list used for updating resources.</param>
				/// <returns></returns>
				bool LoadByPath(const fs::path& a_Path, std::shared_ptr<CommandList> a_pCommandList);

				/// <summary>
				/// Uploads a texture to the heap.
				/// </summary>
				/// <param name="a_UploadHeapProperties">Properties for the heap.</param>
				/// <param name="a_BufferDescription">Properties for the resource.</param>
				/// <returns></returns>
				bool UploadTexture(const D3D12_HEAP_PROPERTIES& a_UploadHeapProperties, const D3D12_RESOURCE_DESC& a_BufferDescription);

				/// <summary>
				/// Retrieves the upload heap resource used for this texture.
				/// </summary>
				/// <returns>Shared pointer to the upload resource.</returns>
				Microsoft::WRL::ComPtr<ID3D12Resource>& GetUploadResource();

				/// <summary>
				/// Sets the SRV description for the texture.
				/// </summary>
				/// <param name="a_SrvDesc">Shader Resource View description.</param>
				void SetSRVDesc(const D3D12_SHADER_RESOURCE_VIEW_DESC& a_SrvDesc);

				/// <summary>
				/// Checks whether the SRV index is valid.
				/// </summary>
				/// <returns>True if valid, false otherwise.</returns>
				bool IsSrvIndexValid() const;

				/// <summary>
				/// Checks whether the texture can be drawn/rendered.
				/// </summary>
				/// <returns>True if drawable, false otherwise.</returns>
				bool CanBeDrawn() const;

				~Texture();
			private:
				friend DX12System2D;

				Microsoft::WRL::ComPtr<ID3D12Resource> m_pResourceUploadHeap = nullptr;
				int32_t m_iSRVIndex = -1;
				D3D12_SHADER_RESOURCE_VIEW_DESC m_SrvDesc;
			};
		}
	}
}
