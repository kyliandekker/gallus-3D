#pragma once

#include "graphics/dx12/DX12PCH.h"
#include "graphics/dx12/DX12Resource.h"

// external
#include <wrl.h>
#include <string>
#include <memory>
#include <cstdint>

// utils
#include "utils/FilePCH.h"

namespace gallus
{
	namespace graphics
	{
		namespace dx12
		{
			class DX12System;
			class CommandQueue;

			enum class TextureType
			{
				Texture2D,
				SpriteSheet
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
						return "Sprite";
					}
					case TextureType::SpriteSheet:
					{
						return "Sprite Sheet";
					}
					default:
					{
						return "";
					}
				}
			}

			class SpriteRect : public ISerializableObject
			{
			public:
				SpriteRect() : ISerializableObject()
				{}

				uint32_t x = 0;
				uint32_t y = 0;
				uint32_t width = 0;
				uint32_t height = 0;

				BEGIN_SERIALIZE(SpriteRect)
					SERIALIZE_FIELD(x, "x", "", .type = gallus::FieldSerializationType::FieldSerializationType_Int64)
					SERIALIZE_FIELD(y, "y", "", .type = gallus::FieldSerializationType::FieldSerializationType_Int64)
					SERIALIZE_FIELD(width, "width", "", .type = gallus::FieldSerializationType::FieldSerializationType_Int64)
					SERIALIZE_FIELD(height, "height", "", .type = gallus::FieldSerializationType::FieldSerializationType_Int64)
				END_SERIALIZE(SpriteRect)
			};

			class SpriteUV
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
				/// <summary>
				/// Constructs an empty texture.
				/// </summary>
				Texture() : DX12Resource()
				{
					m_AssetType = resources::AssetType::Sprite;
				}

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

#ifdef _LOAD_BY_PATH
				/// <summary>
				/// Loads a texture by path.
				/// </summary>
				/// <param name="a_Path">The path to the texture.</param>
				/// <param name="a_pCommandList">The command list used for updating resources.</param>
				/// <returns></returns>
				bool LoadByPath(const fs::path& a_Path, std::shared_ptr<CommandQueue> a_pCommandQueue);
#endif // _LOAD_BY_PATH

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
				/// <param name="a_iSpriteIndex">The sprite index that is used in the sprite sheet (optional).</param>
				void Bind(std::shared_ptr<CommandList> a_pCommandList, int8_t a_iSpriteIndex = 0);

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
				void AddSpriteRect(const SpriteRect& a_Rect);

				/// <summary>
				/// Loads texture metadata.
				/// </summary>
				void LoadMetaData();

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
				size_t GetSpriteRectsSize() const
				{
					return m_aSpriteRects.size();
				}

				/// <summary>
				/// Retrieves a sprite rect by index.
				/// </summary>
				/// <param name="a_iIndex">The sprite index.</param>
				/// <returns>The sprite rect that was requested.</returns>
				SpriteRect GetSpriteRect(int8_t a_iIndex)
				{
					if (a_iIndex >= m_aSpriteRects.size())
					{
						return SpriteRect();
					}

					return m_aSpriteRects[a_iIndex];
				}

				/// <summary>
				/// Retrieves a sprite rect's UV.
				/// </summary>
				/// <param name="a_iIndex">The sprite index.</param>
				/// <returns>The sprite uvs calculated from the rect.</returns>
				SpriteUV GetSpriteUV(int a_iIndex) const;

				~Texture();
			private:
				Microsoft::WRL::ComPtr<ID3D12Resource> m_pResourceUploadHeap = nullptr;
				int32_t m_iSRVIndex = -1;
				D3D12_SHADER_RESOURCE_VIEW_DESC m_SrvDesc;

				TextureType m_TextureType = TextureType::Texture2D;
				std::vector<SpriteRect> m_aSpriteRects;

				BEGIN_SERIALIZE_PARENT(Texture, DX12Resource)
					SERIALIZE_FIELD(m_TextureType, "Texture Type", "The type of texture. Sprite sheet for multiple sprites, or texture for only 1.",
						.type = FieldSerializationType::FieldSerializationType_Enum,
						.enumToStringFunc = MakeEnumToStringFunc<TextureType>(TextureTypeToString))
					SERIALIZE_FIELD_OPTIONS(m_aSpriteRects, "Sprite Rects", "Enables multiple sprites from one image.", MakeArrayFieldSerializationOptions<SpriteRect>())
				END_SERIALIZE(Texture)
			};
		}
	}
}
