#include "TextureMetaData.h"

#include <rapidjson/utils.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/prettywriter.h>

// core includes
#include "core/DataStream.h"

// utils includes
#include "utils/file_abstractions.h"

// logger includes
#include "logger/Logger.h"

// editor includes
#include "editor/FileResource.h"

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
	namespace editor
	{
		//---------------------------------------------------------------------
		// TextureMetaData
		//---------------------------------------------------------------------
		TextureMetaData::TextureMetaData(FileResource& a_FileResource) : MetaData(a_FileResource)
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

			return true;
		}
	}
}