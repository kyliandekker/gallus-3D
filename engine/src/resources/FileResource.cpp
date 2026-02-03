#ifdef _LOAD_BY_PATH

// header
#include "FileResource.h"

// external
#include <rapidjson/document.h>
#include <unordered_map>

// core
#include "core/DataStream.h"

// logger
#include "logger/Logger.h"

// utils
#include "utils/file_abstractions.h"

// resources
#include "resources/AssetType.h"
#include "resources/SrcData.h"

namespace gallus
{
	namespace resources
	{
		//---------------------------------------------------------------------
		// FileResource
		//---------------------------------------------------------------------
		FileResource::FileResource()
		{}

		//---------------------------------------------------------------------
		FileResource::~FileResource()
		{
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
				m_AssetType = resources::AssetType::Folder;

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
				std::string extension = m_Path.extension().generic_string();
				core::Data data;
				if (file::LoadFile(m_Path.generic_string() + ".meta", data))
				{
					resources::SrcData srcData(data);
					if (!srcData.IsValid() || !srcData.GetEnum(m_AssetType, "assetType"))
					{
						LOGF(LOGSEVERITY_WARNING, "%s has no asset type in its meta data.", m_Path.generic_string().c_str());
					}
				}
				else if (FILE_ATLAS.find(extension) != FILE_ATLAS.end())
				{
					auto it = FILE_ATLAS.find(extension);

					m_AssetType = it->second[0];

					resources::SrcData srcData;
					srcData.SetObject();

					srcData.SetEnum("assetType", m_AssetType);

					srcData.GetData(data);
					file::SaveFile(m_Path.generic_string() + ".meta", data);
				}
			}

			for (auto& resource : m_aChildren)
			{
				resource.Scan();
			}

			return true;
		}

		//---------------------------------------------------------------------
		resources::AssetType FileResource::GetAssetType() const
		{
			return m_AssetType;
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
			fs::remove_all(m_Path.c_str());
		}

		//---------------------------------------------------------------------
		bool FileResource::GetFileData(core::Data& a_Data) const
		{
			return file::LoadFile(m_Path, a_Data);
		}

		//---------------------------------------------------------------------
		bool FileResource::Find(const std::string& a_sName, FileResource*& a_pFileResource)
		{
			if (m_Path.filename() == a_sName)
			{
				a_pFileResource = this;
				return true;
			}
			for (FileResource& resource : m_aChildren)
			{
				if (resource.Find(a_sName, a_pFileResource))
				{
					return true;
				}
			}
			return false;
		}

		//---------------------------------------------------------------------
		bool FileResource::Find(const fs::path& a_Path, FileResource*& a_pFileResource)
		{
			if (m_Path == a_Path)
			{
				a_pFileResource = this;
				return true;
			}
			for (FileResource& resource : m_aChildren)
			{
				if (resource.Find(a_Path, a_pFileResource))
				{
					return true;
				}
			}
			return false;
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

#endif // _LOAD_BY_PATH