#pragma once

#include "graphics/dx12/DX12PCH.h"
#include "graphics/dx12/DX12Resource.h"

// external
#include <wrl.h>
#include <string>
#include <memory>
#include <cstdint>

#include "utils/math.h"

namespace gallus::graphics::dx12
{
	class DX12System;
	class CommandQueue;

	enum class TextureType
	{
		Texture2D,
		TextureSheet
	};

	/// <summary>
	/// Converts a texture type enumeration value to its corresponding string representation.
	/// </summary>
	/// <param name="a_TextureType">The texture type to convert.</param>
	/// <returns>A string representing the specified texture type.</returns>
	inline std::string TextureTypeToString(TextureType a_TextureType)
	{
		switch (a_TextureType)
		{
			case TextureType::Texture2D:
			{
				return "Texture";
			}
			case TextureType::TextureSheet:
			{
				return "Texture Sheet";
			}
			default:
			{
				return "";
			}
		}
	}

	class TextureRect : public ISerializableObject
	{
	public:
		TextureRect() : ISerializableObject()
		{}

		uint32_t x = 0;
		uint32_t y = 0;
		uint32_t width = 0;
		uint32_t height = 0;

		BEGIN_SERIALIZE(TextureRect)
			SERIALIZE_FIELD(x, "x", "", .type = FieldSerializationType::FieldSerializationType_Int32)
			SERIALIZE_FIELD(y, "y", "", .type = FieldSerializationType::FieldSerializationType_Int32)
			SERIALIZE_FIELD(width, "width", "", .type = FieldSerializationType::FieldSerializationType_Int32)
			SERIALIZE_FIELD(height, "height", "", .type = FieldSerializationType::FieldSerializationType_Int32)
		END_SERIALIZE(TextureRect)
	};

	class TextureUVCPU
	{
	public:
		DirectX::XMFLOAT2 uv0 = { 0.0f, 0.0f }; // top-left
		DirectX::XMFLOAT2 uv1 = { 1.0f, 1.0f }; // bottom-right
	};

	//---------------------------------------------------------------------
	// Texture
	//---------------------------------------------------------------------
	/// <summary>
	/// Represents a GPU texture.
	/// </summary>
	class Texture : public DX12Resource
	{
	public:
		~Texture();

		/// <summary>
		/// Loads the texture data.
		/// </summary>
		/// <param name="a_Data">A vector containing data in bytes.</param>
		/// <param name="a_pCommandList">The command list used for updating resources.</param>
		/// <returns>True when loading was successful, false otherwise.</returns>
		bool LoadData(const core::Data& a_Data, std::shared_ptr<CommandQueue> a_pCommandQueue);

		/// <summary>
		/// Loads the texture data.
		/// </summary>
		/// <param name="a_Data">A vector containing data in bytes.</param>
		/// <returns>True when loading was successful, false otherwise.</returns>
		bool LoadData(const core::Data& a_Data, D3D12_RESOURCE_DESC a_Description);

		/// <summary>
		/// Destroys the texture and sets it to invalid (it still exists in the texture atlas, however).
		/// </summary>
		bool Destroy() override;

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
		DirectX::XMFLOAT2 GetSize() const;

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
		/// <param name="a_iTextureIndex">The sprite index that is used in the sprite sheet (optional).</param>
		void Bind(std::shared_ptr<CommandList> a_pCommandList, int8_t a_iTextureIndex = 0);

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

		/// <summary>
		/// Adds a sprite rect to the rects array.
		/// </summary>
		/// <param name="a_Rect">The sprite rect that should be added.</param>
		size_t AddTextureRect(const TextureRect& a_Rect);

		/// <summary>
		/// Retrieves the texture type.
		/// </summary>
		/// <returns>The texture type.</returns>
		TextureType GetTextureType() const
		{
			return m_TextureType;
		}

		/// <summary>
		/// Retrieves the amount of sprite rects.
		/// </summary>
		/// <returns>The size of sprite rects that are registered.</returns>
		size_t GetTextureRectsSize() const
		{
			return m_aTextureRects.size();
		}

		/// <summary>
		/// Retrieves a sprite rect by index.
		/// </summary>
		/// <param name="a_iIndex">The sprite index.</param>
		/// <returns>The sprite rect that was requested.</returns>
		TextureRect* GetTextureRect(int8_t a_iIndex)
		{
			if (a_iIndex >= m_aTextureRects.size())
			{
				return nullptr;
			}

			return &m_aTextureRects[a_iIndex];
		}

		std::vector<TextureRect>& GetTextureRects()
		{
			return m_aTextureRects;
		}

		/// <summary>
		/// Retrieves a sprite rect's UV.
		/// </summary>
		/// <param name="a_iIndex">The sprite index.</param>
		/// <returns>The sprite uvs calculated from the rect.</returns>
		TextureUVCPU GetTextureUV(uint16_t a_iIndex) const;

		IVector2 GetSpriteSheetCellSize()
		{
			return { static_cast<int>(m_iSpriteSheetCellSizeX), static_cast<int>(m_iSpriteSheetCellSizeY) };
		}

		void SetSpriteSheetCellSize(const IVector2& a_vSpriteSheetCellSize)
		{
			m_iSpriteSheetCellSizeX = static_cast<uint32_t>(a_vSpriteSheetCellSize.x);
			m_iSpriteSheetCellSizeY = static_cast<uint32_t>(a_vSpriteSheetCellSize.y);
		}
	private:
		Microsoft::WRL::ComPtr<ID3D12Resource> m_pResourceUploadHeap = nullptr;
		int32_t m_iSRVIndex = -1;
		D3D12_SHADER_RESOURCE_VIEW_DESC m_SrvDesc;

		uint32_t m_iSpriteSheetCellSizeX = 0;
		uint32_t m_iSpriteSheetCellSizeY = 0;

		TextureType m_TextureType = TextureType::Texture2D;
		std::vector<TextureRect> m_aTextureRects;

		BEGIN_SERIALIZE_PARENT(Texture, DX12Resource)
			SERIALIZE_FIELD(m_TextureType, "Texture Type", "The type of texture. Texture sheet for multiple sprites, or texture for only 1.",
				.type = FieldSerializationType::FieldSerializationType_Enum,
				.enumToStringFunc = MakeEnumToStringFunc<TextureType>(TextureTypeToString))
			SERIALIZE_FIELD_OPTIONS(m_aTextureRects, "Texture Rects", "Enables multiple sprites from one image.", MakeArrayFieldSerializationOptions(TextureRect, FieldSerializationType::FieldSerializationType_Object))
			SERIALIZE_FIELD(m_iSpriteSheetCellSizeX, "Sprite Sheet Size X", "", .type = FieldSerializationType::FieldSerializationType_Int32)
			SERIALIZE_FIELD(m_iSpriteSheetCellSizeY, "Sprite Sheet Size Y", "", .type = FieldSerializationType::FieldSerializationType_Int32)
		END_SERIALIZE(Texture)
	};
}