#pragma once

#include "MetaData.h"

#include <vector>

#include "graphics/dx12/Texture.h"

namespace gallus
{
	namespace editor
	{
		class FileResource;

		class TextureMetaData : public MetaData
		{
		public:
			TextureMetaData(FileResource& a_FileResource);

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
				{
					3,
					0,
					200,
					300
				}
			};
		};
	}
}