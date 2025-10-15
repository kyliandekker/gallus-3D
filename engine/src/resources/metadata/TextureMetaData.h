#pragma once

#include "MetaData.h"

#include <vector>

#include "graphics/dx12/Texture.h"

namespace gallus
{
	namespace resources
	{
		class TextureMetaData : public MetaData
		{
		public:
			/// <summary>
			/// Constructs meta data for a texture.
			/// </summary>
			TextureMetaData();

			/// <summary>
			/// Retrieves the type of texture resource.
			/// </summary>
			/// <returns>AssetType containing information about what type of texture resource it is.</returns>
			graphics::dx12::TextureType GetTextureType() const;

			/// <summary>
			/// Sets the texture type.
			/// </summary>
			/// <param name="a_TextureType">The texture type it will be set to.</param>
			void SetTextureType(graphics::dx12::TextureType a_TextureType);

			/// <summary>
			/// Retrieves the sprite rects stored in the file's meta data.
			/// </summary>
			/// <returns>The sprite rects stored in this file's meta data.</returns>
			std::vector<graphics::dx12::SpriteRect>& GetSprites()
			{
				return m_aSprites;
			}

			/// <summary>
			/// Generates all the metadata.
			/// </summary>
			rapidjson::Document GenerateMetaData() const override;

			/// <summary>
			/// Saves all the metadata.
			/// </summary>
			bool LoadMetaData(rapidjson::Document& a_Document) override;
		private:
			graphics::dx12::TextureType m_TextureType;

			std::vector<graphics::dx12::SpriteRect> m_aSprites = {
			};
		};
	}
}