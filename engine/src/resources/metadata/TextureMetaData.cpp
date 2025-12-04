#include "TextureMetaData.h"

// external
#include <rapidjson/utils.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/prettywriter.h>

// core
#include "core/DataStream.h"

// logger
#include "logger/Logger.h"

// utils
#include "utils/file_abstractions.h"

#define JSON_META_DATA_FILE_RESOURCE_TEXTURE_TYPE_VAR "textureType"
#define JSON_TEXTURE_META_DATA_FILE_RESOURCE_SPRITES_VAR "sprites"
#define JSON_TEXTURE_META_DATA_FILE_RESOURCE_SPRITE_RECT_VAR "rect"
#define JSON_TEXTURE_META_DATA_FILE_RESOURCE_SPRITE_POSITION_VAR "position"
#define JSON_TEXTURE_META_DATA_FILE_RESOURCE_SPRITE_POSITION_X_VAR "x"
#define JSON_TEXTURE_META_DATA_FILE_RESOURCE_SPRITE_POSITION_Y_VAR "y"
#define JSON_TEXTURE_META_DATA_FILE_RESOURCE_SPRITE_SIZE_VAR "size"
#define JSON_TEXTURE_META_DATA_FILE_RESOURCE_SPRITE_SIZE_WIDTH_VAR "width"
#define JSON_TEXTURE_META_DATA_FILE_RESOURCE_SPRITE_SIZE_HEIGHT_VAR "height"

namespace gallus
{
	namespace resources
	{
		//---------------------------------------------------------------------
		// TextureMetaData
		//---------------------------------------------------------------------
		TextureMetaData::TextureMetaData() : MetaData()
		{}

		//---------------------------------------------------------------------
		graphics::dx12::TextureType TextureMetaData::GetTextureType() const
		{
			return m_TextureType;
		}

		//---------------------------------------------------------------------
		void TextureMetaData::SetTextureType(graphics::dx12::TextureType a_TextureType)
		{
			m_TextureType = a_TextureType;
		}

		//---------------------------------------------------------------------
		rapidjson::Document TextureMetaData::GenerateMetaData() const
		{
			rapidjson::Document doc = MetaData::GenerateMetaData();

			int textureType = (int) m_TextureType;
			rapidjson::SetOrAddMember(doc, JSON_META_DATA_FILE_RESOURCE_TEXTURE_TYPE_VAR, textureType, doc.GetAllocator());

			if (m_TextureType == graphics::dx12::TextureType::SpriteSheet)
			{
				rapidjson::Document spritesDoc;
				spritesDoc.SetArray();

				for (const graphics::dx12::SpriteRect& sprite : m_aSprites)
				{
					rapidjson::Document spriteDoc;
					spriteDoc.SetObject();
					rapidjson::Document rectDoc;
					rectDoc.SetObject();
					rapidjson::Document positionDoc;
					positionDoc.SetObject();
					rapidjson::Document sizeDoc;
					sizeDoc.SetObject();
					positionDoc.AddMember(JSON_TEXTURE_META_DATA_FILE_RESOURCE_SPRITE_POSITION_X_VAR, sprite.x, doc.GetAllocator());
					positionDoc.AddMember(JSON_TEXTURE_META_DATA_FILE_RESOURCE_SPRITE_POSITION_Y_VAR, sprite.y, doc.GetAllocator());
					sizeDoc.AddMember(JSON_TEXTURE_META_DATA_FILE_RESOURCE_SPRITE_SIZE_WIDTH_VAR, sprite.width, doc.GetAllocator());
					sizeDoc.AddMember(JSON_TEXTURE_META_DATA_FILE_RESOURCE_SPRITE_SIZE_HEIGHT_VAR, sprite.height, doc.GetAllocator());
					rectDoc.AddMember(JSON_TEXTURE_META_DATA_FILE_RESOURCE_SPRITE_POSITION_VAR, positionDoc, doc.GetAllocator());
					rectDoc.AddMember(JSON_TEXTURE_META_DATA_FILE_RESOURCE_SPRITE_SIZE_VAR, sizeDoc, doc.GetAllocator());
					spriteDoc.AddMember(JSON_TEXTURE_META_DATA_FILE_RESOURCE_SPRITE_RECT_VAR, rectDoc, doc.GetAllocator());
					spritesDoc.PushBack(spriteDoc, doc.GetAllocator());
				}

				rapidjson::SetOrAddArrayMember(doc, JSON_TEXTURE_META_DATA_FILE_RESOURCE_SPRITES_VAR, spritesDoc, doc.GetAllocator());
			}

			return doc;
		}

		//---------------------------------------------------------------------
		bool TextureMetaData::LoadMetaData(rapidjson::Document& a_Document)
		{
			MetaData::LoadMetaData(a_Document);

			int textureType = 0;
			rapidjson::GetInt(a_Document, JSON_META_DATA_FILE_RESOURCE_TEXTURE_TYPE_VAR, textureType);
			m_TextureType = static_cast<graphics::dx12::TextureType>(textureType);

			if (m_TextureType == graphics::dx12::TextureType::SpriteSheet)
			{
				m_aSprites.clear();

				if (a_Document.HasMember(JSON_TEXTURE_META_DATA_FILE_RESOURCE_SPRITES_VAR) &&
					a_Document[JSON_TEXTURE_META_DATA_FILE_RESOURCE_SPRITES_VAR].IsArray())
				{
					const auto& spritesArray = a_Document[JSON_TEXTURE_META_DATA_FILE_RESOURCE_SPRITES_VAR].GetArray();

					for (const auto& spriteDoc : spritesArray)
					{
						graphics::dx12::SpriteRect spriteRect{};

						if (spriteDoc.HasMember(JSON_TEXTURE_META_DATA_FILE_RESOURCE_SPRITE_RECT_VAR) &&
							spriteDoc[JSON_TEXTURE_META_DATA_FILE_RESOURCE_SPRITE_RECT_VAR].IsObject())
						{
							const auto& rectDoc = spriteDoc[JSON_TEXTURE_META_DATA_FILE_RESOURCE_SPRITE_RECT_VAR];

							if (rectDoc.HasMember(JSON_TEXTURE_META_DATA_FILE_RESOURCE_SPRITE_POSITION_VAR) &&
								rectDoc[JSON_TEXTURE_META_DATA_FILE_RESOURCE_SPRITE_POSITION_VAR].IsObject())
							{
								const auto& positionDoc = rectDoc[JSON_TEXTURE_META_DATA_FILE_RESOURCE_SPRITE_POSITION_VAR];
								if (positionDoc.HasMember(JSON_TEXTURE_META_DATA_FILE_RESOURCE_SPRITE_POSITION_X_VAR) &&
									positionDoc[JSON_TEXTURE_META_DATA_FILE_RESOURCE_SPRITE_POSITION_X_VAR].IsInt())
								{
									spriteRect.x = positionDoc[JSON_TEXTURE_META_DATA_FILE_RESOURCE_SPRITE_POSITION_X_VAR].GetInt();
								}
								if (positionDoc.HasMember(JSON_TEXTURE_META_DATA_FILE_RESOURCE_SPRITE_POSITION_Y_VAR) &&
									positionDoc[JSON_TEXTURE_META_DATA_FILE_RESOURCE_SPRITE_POSITION_Y_VAR].IsInt())
								{
									spriteRect.y = positionDoc[JSON_TEXTURE_META_DATA_FILE_RESOURCE_SPRITE_POSITION_Y_VAR].GetInt();
								}
							}

							if (rectDoc.HasMember(JSON_TEXTURE_META_DATA_FILE_RESOURCE_SPRITE_SIZE_VAR) &&
								rectDoc[JSON_TEXTURE_META_DATA_FILE_RESOURCE_SPRITE_SIZE_VAR].IsObject())
							{
								const auto& sizeDoc = rectDoc[JSON_TEXTURE_META_DATA_FILE_RESOURCE_SPRITE_SIZE_VAR];
								if (sizeDoc.HasMember(JSON_TEXTURE_META_DATA_FILE_RESOURCE_SPRITE_SIZE_WIDTH_VAR) &&
									sizeDoc[JSON_TEXTURE_META_DATA_FILE_RESOURCE_SPRITE_SIZE_WIDTH_VAR].IsInt())
								{
									spriteRect.width = sizeDoc[JSON_TEXTURE_META_DATA_FILE_RESOURCE_SPRITE_SIZE_WIDTH_VAR].GetInt();
								}
								if (sizeDoc.HasMember(JSON_TEXTURE_META_DATA_FILE_RESOURCE_SPRITE_SIZE_HEIGHT_VAR) &&
									sizeDoc[JSON_TEXTURE_META_DATA_FILE_RESOURCE_SPRITE_SIZE_HEIGHT_VAR].IsInt())
								{
									spriteRect.height = sizeDoc[JSON_TEXTURE_META_DATA_FILE_RESOURCE_SPRITE_SIZE_HEIGHT_VAR].GetInt();
								}
							}
						}

						m_aSprites.push_back(spriteRect);
					}
				}
			}

			return true;
		}
	}
}