#include "graphics/dx12/Texture.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

// core includes
#include "core/Engine.h"
#include "core/Data.h"

// logger includes
#include "logger/Logger.h"

// graphics includes
#include "graphics/dx12/CommandList.h"
#include "graphics/dx12/CommandQueue.h"

#include "resources/metadata/TextureMetaData.h"

namespace gallus
{
	namespace graphics
	{
		namespace dx12
		{
			//---------------------------------------------------------------------
			// Texture
			//---------------------------------------------------------------------

			//---------------------------------------------------------------------
			bool Texture::LoadByName(const std::string& a_sName, const D3D12_RESOURCE_DESC& a_Description, const D3D12_HEAP_PROPERTIES& a_Heap, const D3D12_RESOURCE_STATES a_ResourceState)
			{
				if (!DX12Resource::LoadByName(a_sName, a_Description, a_Heap, a_ResourceState))
				{
					return false;
				}

				D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
				srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
				srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
				srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
				srvDesc.Texture2D.MostDetailedMip = 0;
				srvDesc.Texture2D.MipLevels = -1;
				srvDesc.Texture2D.ResourceMinLODClamp = 0.0f;
				SetSRVDesc(srvDesc);

				auto dCommandQueue = core::ENGINE->GetDX12().GetCommandQueue();
				auto dCommandList = dCommandQueue->GetCommandList();
				if (!CreateSRV(dCommandList))
				{
					return false;
				}

				m_AssetType = resources::AssetType::Sprite;

				return true;
			}

			// TODO: This is for loading from the tex file (library of all textures that would get compiled). For now it is unused since we just load by path.
			//---------------------------------------------------------------------
			bool Texture::LoadByName(const std::string& a_sName, std::shared_ptr<CommandList> a_pCommandList, const D3D12_HEAP_PROPERTIES& a_Heap, const D3D12_RESOURCE_STATES a_ResourceState)
			{
				if (!DX12Resource::LoadByName(a_sName))
				{
					return false;
				}

				m_AssetType = resources::AssetType::Sprite;
				return false;
			}

#ifdef _LOAD_BY_PATH
			//---------------------------------------------------------------------
			bool Texture::LoadByPath(const fs::path& a_Path, std::shared_ptr<CommandQueue> a_CommandQueue)
			{
				int width, height, channels;
				stbi_uc* imageData = stbi_load(a_Path.generic_string().c_str(), &width, &height, &channels, STBI_rgb_alpha);
				if (!imageData)
				{
					LOGF(LOGSEVERITY_ERROR, LOG_CATEGORY_DX12, "Failed to load texture: \"%s\".", a_Path.generic_string().c_str());
					return false;
				}

				// Pre-multiply alpha: p.rgb = (p.rgb * p.a) / 255
				size_t pixelCount = static_cast<size_t>(width) * static_cast<size_t>(height);
				for (size_t i = 0; i < pixelCount; ++i)
				{
					uint8_t* px = imageData + i * 4;
					uint8_t a = px[3];
					if (a == 255)
					{
						continue;
					}
					if (a == 0)
					{
						px[0] = 0;
						px[1] = 0;
						px[2] = 0;
						continue;
					}
					// integer premultiply with rounding
					px[0] = static_cast<uint8_t>((px[0] * a + 127) / 255);
					px[1] = static_cast<uint8_t>((px[1] * a + 127) / 255);
					px[2] = static_cast<uint8_t>((px[2] * a + 127) / 255);
				}

				size_t memorySize = static_cast<size_t>(width) * height * 4;
				core::Data data = core::Data(imageData, memorySize);

				stbi_image_free(imageData);

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

				if (!DX12Resource::LoadByPath(m_sName, textureDesc))
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

				auto cCommandList = a_CommandQueue->GetCommandList();

				UploadTexture(uploadHeapProperties, bufferResource);
				UpdateSubresources(cCommandList->GetCommandList().Get(), m_pResource.Get(), m_pResourceUploadHeap.Get(), 0, 0, 1, &textureData);

				uint64_t fenceValue = a_CommandQueue->ExecuteCommandList(cCommandList);
				a_CommandQueue->WaitForFenceValue(fenceValue);

				D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
				srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
				srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
				srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
				srvDesc.Texture2D.MostDetailedMip = 0;
				srvDesc.Texture2D.MipLevels = 1;
				srvDesc.Texture2D.ResourceMinLODClamp = 0.0f;
				SetSRVDesc(srvDesc);

				m_AssetType = resources::AssetType::Sprite;

				LoadMetaData();

				auto dCommandQueue = core::ENGINE->GetDX12().GetCommandQueue();
				auto dCommandList = dCommandQueue->GetCommandList();
				CreateSRV(dCommandList);

				LOGF(LOGSEVERITY_INFO_SUCCESS, LOG_CATEGORY_DX12, "Successfully loaded texture: \"%s\".", a_Path.generic_string().c_str());
				return true;
			}
#endif

			//---------------------------------------------------------------------
			bool Texture::Destroy()
			{
				std::shared_ptr<CommandQueue> commandQueue = core::ENGINE->GetDX12().GetCommandQueue(D3D12_COMMAND_LIST_TYPE_DIRECT);

				uint64_t fenceVal = core::ENGINE->GetDX12().GetCurrentFenceValue();
				commandQueue->WaitForFenceValue(core::ENGINE->GetDX12().GetCurrentFenceValue());

				if (!DX12Resource::Destroy())
				{
					return;
				}

				if (IsSrvIndexValid())
				{
					core::ENGINE->GetDX12().GetSRV().Deallocate(m_iSRVIndex);
					m_iSRVIndex = -1;
				}
				if (m_pResourceUploadHeap)
				{
					m_pResourceUploadHeap.Reset();
				}
			}

			Texture::~Texture()
			{
				Destroy();
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
				if (!m_pResource)
				{
					return false;
				}

				// Only create SRV once
				if (m_iSRVIndex == -1)
				{
					m_iSRVIndex = static_cast<int32_t>(core::ENGINE->GetDX12().GetSRV().Allocate());
					core::ENGINE->GetDX12().GetDevice()->CreateShaderResourceView(
						m_pResource.Get(),
						&m_SrvDesc,
						core::ENGINE->GetDX12().GetSRV().GetCPUHandle(m_iSRVIndex)
					);
				}

				// Only transition if not already in PIXEL_SHADER_RESOURCE
				if (m_CurrentState != D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE)
				{
					Transition(a_pCommandList, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
				}
				return true;
			}

			//---------------------------------------------------------------------
			void Texture::Bind(std::shared_ptr<CommandList> a_pCommandList, int8_t a_iSpriteIndex)
			{
				a_pCommandList->GetCommandList()->SetDescriptorHeaps(1, core::ENGINE->GetDX12().GetSRV().GetHeap().GetAddressOf());

				CD3DX12_GPU_DESCRIPTOR_HANDLE gpuHandle = core::ENGINE->GetDX12().GetSRV().GetGPUHandle(m_iSRVIndex);

				a_pCommandList->GetCommandList()->SetGraphicsRootDescriptorTable(RootParameters::TEX_SRV, gpuHandle);

				if (m_TextureType == TextureType::SpriteSheet && a_iSpriteIndex < m_aSpriteRects.size())
				{
					SpriteUV uv = GetSpriteUV(a_iSpriteIndex);
					float uvData[4] = { uv.uv0.x, uv.uv0.y, uv.uv1.x, uv.uv1.y };
					a_pCommandList->GetCommandList()->SetGraphicsRoot32BitConstants(RootParameters::SPRITE_UV, 4, uvData, 0);
				}
				else
				{
					SpriteUV uv;
					uv.uv0 = { 0.0f, 0.0f }; 
					uv.uv1 = { 1.0f, 1.0f }; 
					float uvData[4] = { uv.uv0.x, uv.uv0.y, uv.uv1.x, uv.uv1.y };
					a_pCommandList->GetCommandList()->SetGraphicsRoot32BitConstants(RootParameters::SPRITE_UV, 4, uvData, 0);
				}
			}

			//---------------------------------------------------------------------
			void Texture::Unbind(std::shared_ptr<CommandList> a_pCommandList)
			{}

			//---------------------------------------------------------------------
			CD3DX12_GPU_DESCRIPTOR_HANDLE Texture::GetGPUHandle()
			{
				return core::ENGINE->GetDX12().GetSRV().GetGPUHandle(m_iSRVIndex);
			}

			//---------------------------------------------------------------------
			CD3DX12_CPU_DESCRIPTOR_HANDLE Texture::GetCPUHandle()
			{
				return core::ENGINE->GetDX12().GetSRV().GetCPUHandle(m_iSRVIndex);
			}

			//---------------------------------------------------------------------
			bool Texture::UploadTexture(const D3D12_HEAP_PROPERTIES& a_UploadHeapProperties, const D3D12_RESOURCE_DESC& a_BufferDescription)
			{
				UINT64 uploadBufferSize = GetRequiredIntermediateSize(m_pResource.Get(), 0, 1);
				if (FAILED(core::ENGINE->GetDX12().GetDevice()->CreateCommittedResource(
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

			//---------------------------------------------------------------------
			void Texture::AddSpriteRect(const SpriteRect& a_Rect)
			{
				m_TextureType = TextureType::SpriteSheet;
				m_aSpriteRects.push_back(a_Rect);
			}

			//---------------------------------------------------------------------
			void Texture::LoadMetaData()
			{
				// TODO: This will fail when _LOAD_BY_PATH is disabled (aka, in game mode later)
				resources::TextureMetaData metaData;
				rapidjson::Document doc = metaData.Load(m_Path);
				metaData.LoadMetaData(doc);

				m_aSpriteRects = metaData.GetSprites();
				m_TextureType = metaData.GetTextureType();
			}

			//---------------------------------------------------------------------
			SpriteUV Texture::GetSpriteUV(int a_iIndex) const
			{
				SpriteUV uv;

				if (a_iIndex < 0 || a_iIndex >= static_cast<int>(m_aSpriteRects.size()))
				{
					return uv;
				}

				const auto& r = m_aSpriteRects[a_iIndex];
				glm::ivec2 size = GetSize(); // returns {width, height} of texture

				float u0 = (static_cast<float>(r.x) + 0.5f) / static_cast<float>(size.x);
				float v0 = (static_cast<float>(r.y) + 0.5f) / static_cast<float>(size.y);
				float u1 = (static_cast<float>(r.x + r.width) - 0.5f) / static_cast<float>(size.x);
				float v1 = (static_cast<float>(r.y + r.height) - 0.5f) / static_cast<float>(size.y);

				uv.uv0 = { u0, v0 };
				uv.uv1 = { u1, v1 };
				return uv;
			}
		}
	}
}