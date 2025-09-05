#ifdef _EDITOR

#include "FileResource.h"

#include <rapidjson/document.h>
#include <unordered_map>

// core includes
#include "core/DataStream.h"

// logger includes
#include "logger/Logger.h"

// utils includes
#include "utils/file_abstractions.h"

// editor includes
#include "editor/AssetType.h"
#include "editor/AssetDatabase.h"
#include "editor/metadata/MetaData.h"
#include "editor/metadata/TextureMetaData.h"

namespace gallus
{
	namespace editor
	{
		//---------------------------------------------------------------------
		// FileResource
		//---------------------------------------------------------------------
		FileResource::FileResource()
		{}

		//---------------------------------------------------------------------
		FileResource::~FileResource()
		{
			delete m_MetaData;
		}

		//---------------------------------------------------------------------
		const fs::path& FileResource::GetPath() const
		{
			return m_Path;
		}

		//---------------------------------------------------------------------
		FileResource* FileResource::GetParent() const
		{
			return m_Parent;
		}

		const std::unordered_map<std::string, std::vector<AssetType>> FILE_ATLAS =
		{
			{ ".scene", { AssetType::Scene } },
			{ ".png", { AssetType::Texture } },
			{ ".bmp", { AssetType::Texture } },
			{ ".wav", { AssetType::Sound, AssetType::Song, AssetType::VO } },
			{ ".anim", { AssetType::Animation } },
			{ ".hlsl", { AssetType::PixelShader, AssetType::VertexShader } },
		};

		//---------------------------------------------------------------------
		bool FileResource::Scan()
		{
			if (!fs::exists(m_Path))
			{
				return false;
			}

			m_aChildren.clear();
			m_aChildren.reserve(20);

			if (fs::is_directory(m_Path))
			{
				m_MetaData = new MetaData(*this);
				m_MetaData->SetAssetType(AssetType::Folder);

				// Go through each file/folder and check their status.
				fs::directory_iterator ds = fs::directory_iterator(m_Path, std::filesystem::directory_options::skip_permission_denied);
				for (const auto& dirEntry : ds)
				{
					// Check for hidden or system files. These should not be added.
					DWORD attributes = GetFileAttributes(dirEntry.path().generic_string().c_str());
					if (attributes & FILE_ATTRIBUTE_HIDDEN || attributes & FILE_ATTRIBUTE_SYSTEM)
					{
						continue;
					}

					if (!fs::is_directory(dirEntry.path()))
					{
						std::string extension = dirEntry.path().extension().generic_string();

						// Get the extension. If the extension is not recognized, it will just be ignored.
						if (FILE_ATLAS.find(extension) == FILE_ATLAS.end())
						{
							continue;
						}
					}

					FileResource resource = FileResource();
					resource.m_Path = dirEntry.path().lexically_normal();
					resource.m_Parent = this;
					m_aChildren.push_back(resource);
				}
			}
			else
			{
				m_MetaData = new MetaData(*this);
				if (!m_MetaData->Exists())
				{
					std::string extension = m_Path.extension().generic_string();

					// Get the extension. If the extension is not recognized, it will just be ignored.
					if (FILE_ATLAS.find(extension) != FILE_ATLAS.end())
					{
						auto it = FILE_ATLAS.find(extension);

						m_MetaData->SetAssetType(it->second[0]);
						m_MetaData->Save();
					}
				}
				else
				{
					// TODO: This is awful.

					rapidjson::Document doc = m_MetaData->Load();
					m_MetaData->LoadMetaData(doc);
					AssetType assetType = m_MetaData->GetAssetType();

					switch (assetType)
					{
						case AssetType::Texture:
						{
							delete m_MetaData;
							m_MetaData = new TextureMetaData(*this);
							break;
						}
						default:
						{

							break;
						}
					}
					doc = m_MetaData->Load();
					m_MetaData->LoadMetaData(doc);
				}
			}

			for (auto& resource : m_aChildren)
			{
				resource.Scan();
			}

			return true;
		}

		//---------------------------------------------------------------------
		void FileResource::Rename(const std::string& a_sName)
		{
			fs::path newPath = m_Path.parent_path().generic_string() + a_sName + m_Path.extension().generic_string();

			fs::rename(m_Path, newPath);
			m_Path = newPath;
		}

		//---------------------------------------------------------------------
		void FileResource::Delete()
		{
			fs::remove(m_Path.c_str());
		}

		//---------------------------------------------------------------------
		bool FileResource::GetFileData(core::Data& a_Data) const
		{
			return file::LoadFile(m_Path, a_Data);
		}

		//---------------------------------------------------------------------
		bool FileResource::SearchForPath(const fs::path& a_Path, FileResource*& a_pFileResource)
		{
			if (m_Path == a_Path)
			{
				a_pFileResource = this;
				return true;
			}
			for (FileResource& view : m_aChildren)
			{
				if (view.SearchForPath(a_Path, a_pFileResource))
				{
					return true;
				}
			}
			return false;
		}
	}
}

#endif // _EDITOR