#include "Texture.h"

// external
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

// core
#include "core/Engine.h"
#include "core/Data.h"

// logger
#include "logger/Logger.h"

// graphics
#include "graphics/dx12/DX12System.h"
#include "graphics/dx12/HeapAllocation.h"
#include "graphics/dx12/CommandList.h"
#include "graphics/dx12/CommandQueue.h"

namespace gallus::graphics::dx12
{
	//---------------------------------------------------------------------
	// Texture
	//---------------------------------------------------------------------
	Texture::~Texture()
	{
		Destroy();
	}

	//---------------------------------------------------------------------
	bool Texture::LoadData(const core::Data& a_Data, D3D12_RESOURCE_DESC a_Description)
	{
		if (!SetResourceData("", a_Description))
		{
			LOGF(LOGSEVERITY_ERROR, LOG_CATEGORY_DX12, "Failed to load empty texture \"%s\": Could not create DX12 resource.", m_sName.c_str());
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

		DX12System& dx12System = GetDX12System();

		std::shared_ptr<CommandQueue> dCommandQueue = dx12System.GetCommandQueue();
		std::shared_ptr<CommandList> dCommandList = dCommandQueue->GetCommandList();
		CreateSRV(dCommandList);

		m_AssetType = resources::AssetType::Texture;

		LOGF(LOGSEVERITY_INFO_SUCCESS, LOG_CATEGORY_DX12, "Successfully loaded empty texture \"%s\".", m_sName.c_str());
		return true;
	}

	//---------------------------------------------------------------------
	bool Texture::LoadData(const core::Data& a_Data, std::shared_ptr<CommandQueue> a_pCommandQueue)
    {
		int width, height, channels;
		stbi_uc* imageData = stbi_load_from_memory(a_Data.dataAs<stbi_uc>(), a_Data .size(), &width, &height, &channels, STBI_rgb_alpha);
		if (!imageData)
		{
			LOGF(LOGSEVERITY_ERROR, LOG_CATEGORY_DX12, "Failed to load texture \"%s\": Something was wrong with data.", m_sName.c_str());
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
			// integer pre-multiply with rounding
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

		if (!SetResourceData("", textureDesc))
		{
			LOGF(LOGSEVERITY_ERROR, LOG_CATEGORY_DX12, "Failed to load texture \"%s\": Could not create DX12 resource.", m_sName.c_str());
			return false;
		}

		const UINT64 uploadBufferSize = GetRequiredIntermediateSize(m_pResource.Get(), 0, 1);
		const CD3DX12_HEAP_PROPERTIES uploadHeapProperties(D3D12_HEAP_TYPE_UPLOAD);
		const CD3DX12_RESOURCE_DESC bufferResource = CD3DX12_RESOURCE_DESC::Buffer(uploadBufferSize);
		D3D12_SUBRESOURCE_DATA textureData = {};
		textureData.pData = data.data();
		textureData.RowPitch = width * channels;
		textureData.SlicePitch = textureData.RowPitch * height;

		std::shared_ptr<CommandList> cCommandList = a_pCommandQueue->GetCommandList();

		UploadTexture(uploadHeapProperties, bufferResource);
		UpdateSubresources(cCommandList->GetCommandList().Get(), m_pResource.Get(), m_pResourceUploadHeap.Get(), 0, 0, 1, &textureData);

		uint64_t fenceValue = a_pCommandQueue->ExecuteCommandList(cCommandList);
		a_pCommandQueue->WaitForFenceValue(fenceValue);

		D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MostDetailedMip = 0;
		srvDesc.Texture2D.MipLevels = 1;
		srvDesc.Texture2D.ResourceMinLODClamp = 0.0f;
		SetSRVDesc(srvDesc);

		m_AssetType = resources::AssetType::Texture;

		DX12System& dx12System = GetDX12System();

		std::shared_ptr<CommandQueue> dCommandQueue = dx12System.GetCommandQueue();
		std::shared_ptr<CommandList> dCommandList = dCommandQueue->GetCommandList();
		CreateSRV(dCommandList);

		LOGF(LOGSEVERITY_INFO_SUCCESS, LOG_CATEGORY_DX12, "Successfully loaded texture \"%s\".", m_sName.c_str());
		return true;
    }

	//---------------------------------------------------------------------
	bool Texture::Destroy()
	{
		DX12System& dx12System = GetDX12System();

		std::shared_ptr<CommandQueue> commandQueue = dx12System.GetCommandQueue(D3D12_COMMAND_LIST_TYPE_DIRECT);

		uint64_t fenceVal = dx12System.GetCurrentFenceValue();
		commandQueue->WaitForFenceValue(dx12System.GetCurrentFenceValue());

		if (!DX12Resource::Destroy())
		{
			return false;
		}

		if (IsSrvIndexValid())
		{
			dx12System.GetSRV().Deallocate(m_iSRVIndex);
			m_iSRVIndex = -1;
		}
		if (m_pResourceUploadHeap)
		{
			m_pResourceUploadHeap.Reset();
		}

		return true;
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
	DirectX::XMFLOAT2 Texture::GetSize() const
	{
		return { static_cast<float>(GetResourceDesc().Width), static_cast<float>(GetResourceDesc().Height) };
	}

	//---------------------------------------------------------------------
	bool Texture::CreateSRV(std::shared_ptr<CommandList> a_pCommandList)
	{
		if (!m_pResource)
		{
			return false;
		}

		DX12System& dx12System = GetDX12System();

		// Only create SRV once
		if (m_iSRVIndex == -1)
		{
			m_iSRVIndex = static_cast<int32_t>(dx12System.GetSRV().Allocate());
			dx12System.GetDevice()->CreateShaderResourceView(
				m_pResource.Get(),
				&m_SrvDesc,
				dx12System.GetSRV().GetCPUHandle(m_iSRVIndex)
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
	void Texture::Bind(std::shared_ptr<CommandList> a_pCommandList, int8_t a_iTextureIndex)
	{
		DX12System& dx12System = GetDX12System();

		a_pCommandList->GetCommandList()->SetDescriptorHeaps(1, dx12System.GetSRV().GetHeap().GetAddressOf());

		CD3DX12_GPU_DESCRIPTOR_HANDLE gpuHandle = dx12System.GetSRV().GetGPUHandle(m_iSRVIndex);

		a_pCommandList->GetCommandList()->SetGraphicsRootDescriptorTable(graphics::dx12::RootParameters::TEX_SRV, gpuHandle);

		if (m_TextureType == TextureType::TextureSheet && a_iTextureIndex < m_aTextureRects.size())
		{
			TextureUVCPU uv = GetTextureUV(a_iTextureIndex);
			float uvData[4] = { uv.uv0.x, uv.uv0.y, uv.uv1.x, uv.uv1.y };
			a_pCommandList->GetCommandList()->SetGraphicsRoot32BitConstants(graphics::dx12::RootParameters::SPRITE_UV, 4, uvData, 0);
		}
		else
		{
			TextureUVCPU uv;
			uv.uv0 = { 0.0f, 0.0f }; 
			uv.uv1 = { 1.0f, 1.0f }; 
			float uvData[4] = { uv.uv0.x, uv.uv0.y, uv.uv1.x, uv.uv1.y };
			a_pCommandList->GetCommandList()->SetGraphicsRoot32BitConstants(graphics::dx12::RootParameters::SPRITE_UV, 4, uvData, 0);
		}
	}

	//---------------------------------------------------------------------
	CD3DX12_GPU_DESCRIPTOR_HANDLE Texture::GetGPUHandle()
	{
		DX12System& dx12System = GetDX12System();

		return dx12System.GetSRV().GetGPUHandle(m_iSRVIndex);
	}

	//---------------------------------------------------------------------
	CD3DX12_CPU_DESCRIPTOR_HANDLE Texture::GetCPUHandle()
	{
		DX12System& dx12System = GetDX12System();

		return dx12System.GetSRV().GetCPUHandle(m_iSRVIndex);
	}

	//---------------------------------------------------------------------
	bool Texture::UploadTexture(const D3D12_HEAP_PROPERTIES& a_UploadHeapProperties, const D3D12_RESOURCE_DESC& a_BufferDescription)
	{
		DX12System& dx12System = GetDX12System();

		UINT64 uploadBufferSize = GetRequiredIntermediateSize(m_pResource.Get(), 0, 1);
		if (FAILED(dx12System.GetDevice()->CreateCommittedResource(
			&a_UploadHeapProperties,
			D3D12_HEAP_FLAG_NONE,
			&a_BufferDescription,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&m_pResourceUploadHeap))))
		{
			LOG(LOGSEVERITY_ERROR, LOG_CATEGORY_DX12, "Failed uploading texture: Failed to create upload heap.");
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
	size_t Texture::AddTextureRect(const TextureRect& a_Rect)
	{
		m_TextureType = TextureType::TextureSheet;
		m_aTextureRects.push_back(a_Rect);
		return m_aTextureRects.size() - 1;
	}

	//---------------------------------------------------------------------
	TextureUVCPU Texture::GetTextureUV(uint16_t a_iIndex) const
	{
		TextureUVCPU uv;

		if (a_iIndex < 0 || a_iIndex >= static_cast<uint16_t>(m_aTextureRects.size()))
		{
			return uv;
		}

		const TextureRect& r = m_aTextureRects[a_iIndex];
		DirectX::XMFLOAT2 size = GetSize(); // returns {width, height} of texture

		float u0 = (static_cast<float>(r.x) + 0.5f) / static_cast<float>(size.x);
		float v0 = (static_cast<float>(r.y) + 0.5f) / static_cast<float>(size.y);
		float u1 = (static_cast<float>(r.x + r.width) - 0.5f) / static_cast<float>(size.x);
		float v1 = (static_cast<float>(r.y + r.height) - 0.5f) / static_cast<float>(size.y);

		uv.uv0 = { u0, v0 };
		uv.uv1 = { u1, v1 };
		return uv;
	}
}

