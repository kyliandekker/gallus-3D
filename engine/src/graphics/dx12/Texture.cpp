#include "graphics/dx12/Texture.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

// core includes
#include "core/Tool.h"
#include "core/Data.h"

// logger includes
#include "logger/Logger.h"

// graphics includes
#include "graphics/dx12/CommandList.h"
#include "graphics/dx12/CommandQueue.h"

namespace gallus
{
	namespace graphics
	{
		namespace dx12
		{
			//---------------------------------------------------------------------
			// Texture
			//---------------------------------------------------------------------
			Texture::~Texture()
			{
				Destroy();
			}

			//---------------------------------------------------------------------
			void Texture::Destroy()
			{
				std::shared_ptr<CommandQueue> commandQueue = core::TOOL->GetDX12().GetCommandQueue(D3D12_COMMAND_LIST_TYPE_DIRECT);

				uint64_t fenceVal = core::TOOL->GetDX12().GetCurrentFenceValue();
				commandQueue->WaitForFenceValue(core::TOOL->GetDX12().GetCurrentFenceValue());

				if (m_pResource)
				{
					core::TOOL->GetDX12().GetSRV().Deallocate(m_iSRVIndex);
					m_pResource.Reset();
					m_pResourceUploadHeap.Reset();
					m_iSRVIndex = -1;
				}
				DX12Resource::Destroy();
			}

			//---------------------------------------------------------------------
			bool Texture::CheckSRVSupport() const
			{
				return CheckFormatSupport(D3D12_FORMAT_SUPPORT1_SHADER_SAMPLE);
			}

			//---------------------------------------------------------------------
			bool Texture::CheckRTVSupport() const
			{
				return CheckFormatSupport(D3D12_FORMAT_SUPPORT1_RENDER_TARGET);
			}

			//---------------------------------------------------------------------
			bool Texture::CheckUAVSupport() const
			{
				return CheckFormatSupport(D3D12_FORMAT_SUPPORT1_TYPED_UNORDERED_ACCESS_VIEW) &&
					CheckFormatSupport(D3D12_FORMAT_SUPPORT2_UAV_TYPED_LOAD) &&
					CheckFormatSupport(D3D12_FORMAT_SUPPORT2_UAV_TYPED_STORE);
			}

			//---------------------------------------------------------------------
			bool Texture::CheckDSVSupport() const
			{
				return CheckFormatSupport(D3D12_FORMAT_SUPPORT1_DEPTH_STENCIL);
			}

			//---------------------------------------------------------------------
			glm::ivec2 Texture::GetSize() const
			{
				return glm::ivec2(GetResourceDesc().Width, GetResourceDesc().Height);
			}

			//---------------------------------------------------------------------
			bool Texture::CreateSRV(std::shared_ptr<CommandList> a_pCommandList)
			{
				if (m_iSRVIndex != -1 || !m_pResource)
				{
					return false;
				}

				auto desc = m_pResource->GetDesc();
				if (desc.Width == 0 || desc.Height == 0)
				{
					return false;
				}

				CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
					m_pResource.Get(),
					D3D12_RESOURCE_STATE_COPY_DEST,
					D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
				a_pCommandList->GetCommandList()->ResourceBarrier(1, &barrier);

				m_iSRVIndex = static_cast<int32_t>(core::TOOL->GetDX12().GetSRV().Allocate());
				core::TOOL->GetDX12().GetDevice()->CreateShaderResourceView(m_pResource.Get(), &m_SrvDesc, core::TOOL->GetDX12().GetSRV().GetCPUHandle(m_iSRVIndex));

				return true;
			}

			//---------------------------------------------------------------------
			void Texture::Bind(std::shared_ptr<CommandList> a_pCommandList)
			{
				a_pCommandList->GetCommandList()->SetDescriptorHeaps(1, core::TOOL->GetDX12().GetSRV().GetHeap().GetAddressOf());

				CD3DX12_GPU_DESCRIPTOR_HANDLE gpuHandle = core::TOOL->GetDX12().GetSRV().GetGPUHandle(m_iSRVIndex);

				a_pCommandList->GetCommandList()->SetGraphicsRootDescriptorTable(1, gpuHandle);
			}

			//---------------------------------------------------------------------
			void Texture::Unbind(std::shared_ptr<CommandList> a_pCommandList)
			{}

			//---------------------------------------------------------------------
			CD3DX12_GPU_DESCRIPTOR_HANDLE Texture::GetGPUHandle()
			{
				return core::TOOL->GetDX12().GetSRV().GetGPUHandle(m_iSRVIndex);
			}

			//---------------------------------------------------------------------
			CD3DX12_CPU_DESCRIPTOR_HANDLE Texture::GetCPUHandle()
			{
				return core::TOOL->GetDX12().GetSRV().GetCPUHandle(m_iSRVIndex);
			}

			//---------------------------------------------------------------------
			bool Texture::LoadByName(const std::string& a_sName, const D3D12_RESOURCE_DESC& a_Description, const D3D12_HEAP_PROPERTIES& a_Heap, const D3D12_RESOURCE_STATES a_ResourceState)
			{
				const bool success = CreateResource(a_Description, a_sName, a_Heap, a_ResourceState);
				if (success)
				{
					D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
					srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
					srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
					srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
					srvDesc.Texture2D.MostDetailedMip = 0;
					srvDesc.Texture2D.MipLevels = -1;
					srvDesc.Texture2D.ResourceMinLODClamp = 0.0f;
					SetSRVDesc(srvDesc);
				}

				m_ResourceType = core::ResourceType::ResourceType_Texture;
				return success;
			}

			// TODO: This is for loading from the tex file (library of all textures that would get compiled). For now it is unused since we just use the editor.
			//---------------------------------------------------------------------
			bool Texture::LoadByName(const std::string& a_sName, std::shared_ptr<CommandList> a_pCommandList, const D3D12_HEAP_PROPERTIES& a_Heap, const D3D12_RESOURCE_STATES a_ResourceState)
			{
				m_ResourceType = core::ResourceType::ResourceType_Texture;
				return false;
			}

			//---------------------------------------------------------------------
			bool Texture::LoadByPath(const fs::path& a_Path, std::shared_ptr<CommandList> a_CommandList)
			{
				if (m_pResource && !m_bIsDestroyable)
				{
					return false;
				}

				if (m_iSRVIndex != -1)
				{
					core::TOOL->GetDX12().GetSRV().Deallocate(m_iSRVIndex);
					m_iSRVIndex = -1;
				}

				int width, height, channels;
				stbi_uc* imageData = stbi_load(a_Path.generic_string().c_str(), &width, &height, &channels, STBI_rgb_alpha);
				if (!imageData)
				{
					LOGF(LOGSEVERITY_ERROR, LOG_CATEGORY_DX12, "Failed to load texture: \"%s\".", a_Path.generic_string().c_str());
					return false;
				}

				size_t memorySize = static_cast<size_t>(width) * height * 4;
				core::Data data = core::Data(imageData, memorySize);
				 
				// OPTION: Do the alpha fix here.
				//for (int i = 0; i < width * height; i++)
				//{
				//	unsigned char* pixel = &data[i * 4]; // RGBA
				//	float a = pixel[3] / 255.0f;
				//	pixel[0] = (unsigned char) (pixel[0] * a);
				//	pixel[1] = (unsigned char) (pixel[1] * a);
				//	pixel[2] = (unsigned char) (pixel[2] * a);
				//}

				stbi_image_free(imageData);

				m_sName = a_Path.filename().generic_string();
				m_Path = a_Path;

				D3D12_RESOURCE_DESC textureDesc = {};
				textureDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
				textureDesc.Width = width;
				textureDesc.Height = height;
				textureDesc.DepthOrArraySize = 1;
				textureDesc.MipLevels = 1;
				textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
				textureDesc.SampleDesc.Count = 1;
				textureDesc.Flags = D3D12_RESOURCE_FLAG_NONE;
				textureDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;

				if (!CreateResource(textureDesc, m_sName))
				{
					LOG(LOGSEVERITY_ERROR, LOG_CATEGORY_DX12, "Failed creating resource.");
					return false;
				}

				const UINT64 uploadBufferSize = GetRequiredIntermediateSize(m_pResource.Get(), 0, 1);
				const CD3DX12_HEAP_PROPERTIES uploadHeapProperties(D3D12_HEAP_TYPE_UPLOAD);
				const CD3DX12_RESOURCE_DESC bufferResource = CD3DX12_RESOURCE_DESC::Buffer(uploadBufferSize);
				D3D12_SUBRESOURCE_DATA textureData = {};
				textureData.pData = data.data();
				textureData.RowPitch = width * channels;
				textureData.SlicePitch = textureData.RowPitch * height;

				UploadTexture(uploadHeapProperties, bufferResource);
				UpdateSubresources(a_CommandList->GetCommandList().Get(), m_pResource.Get(), m_pResourceUploadHeap.Get(), 0, 0, 1, &textureData);

				D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
				srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
				srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
				srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
				srvDesc.Texture2D.MostDetailedMip = 0;
				srvDesc.Texture2D.MipLevels = 1;
				srvDesc.Texture2D.ResourceMinLODClamp = 0.0f;
				SetSRVDesc(srvDesc);

				m_ResourceType = core::ResourceType::ResourceType_Texture;

				LOGF(LOGSEVERITY_INFO_SUCCESS, LOG_CATEGORY_DX12, "Successfully loaded texture: \"%s\".", a_Path.generic_string().c_str());
				return true;
			}

			//---------------------------------------------------------------------
			bool Texture::UploadTexture(const D3D12_HEAP_PROPERTIES& a_UploadHeapProperties, const D3D12_RESOURCE_DESC& a_BufferDescription)
			{
				UINT64 uploadBufferSize = GetRequiredIntermediateSize(m_pResource.Get(), 0, 1);
				if (FAILED(core::TOOL->GetDX12().GetDevice()->CreateCommittedResource(
					&a_UploadHeapProperties,
					D3D12_HEAP_FLAG_NONE,
					&a_BufferDescription,
					D3D12_RESOURCE_STATE_GENERIC_READ,
					nullptr,
					IID_PPV_ARGS(&m_pResourceUploadHeap))))
				{
					LOG(LOGSEVERITY_ERROR, LOG_CATEGORY_DX12, "Failed to create upload heap.");
					return false;
				}

				return true;
			}

			//---------------------------------------------------------------------
			Microsoft::WRL::ComPtr<ID3D12Resource>& Texture::GetUploadResource()
			{
				return m_pResourceUploadHeap;
			}

			//---------------------------------------------------------------------
			void Texture::SetSRVDesc(const D3D12_SHADER_RESOURCE_VIEW_DESC& a_SrvDesc)
			{
				m_SrvDesc = a_SrvDesc;
			}

			//---------------------------------------------------------------------
			bool Texture::IsSrvIndexValid() const
			{
				return m_iSRVIndex != -1;
			}

			//---------------------------------------------------------------------
			bool Texture::CanBeDrawn() const
			{
				return IsSrvIndexValid() && IsValid();
			}
		}
	}
}