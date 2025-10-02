#pragma once

// base class
#include "MetaData.h"

// external
#include <vector>

namespace gallus
{
	namespace graphics
	{
		namespace dx12
		{
			enum class TextureType;
			class SpriteRect;
		}
	}
	namespace resources
	{
		class TextureMetaData : public MetaData
		{
		public:
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

			std::vector<graphics::dx12::SpriteRect> m_aSprites;
		};
	}
}