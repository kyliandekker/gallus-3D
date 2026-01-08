#include "file_abstractions.h"

// external
#include <vector>
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <ShlObj_core.h>

// core
#include "core/DataStream.h"

namespace gallus
{
	namespace file
	{
		//---------------------------------------------------------------------
		bool genericFileOpen(fs::path& a_sPath, const IID a_Rclsid, FILEOPENDIALOGOPTIONS a_Options, const std::vector<COMDLG_FILTERSPEC>& a_aFilters = {}, const fs::path& a_InitialPath = {})
		{
			CoInitialize(nullptr);

			IFileDialog* pfd;
			HRESULT hr = CoCreateInstance(a_Rclsid, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pfd));
			if (FAILED(hr))
				return false;

			// Set file type filters
			if (!a_aFilters.empty())
				pfd->SetFileTypes(static_cast<UINT>(a_aFilters.size()), a_aFilters.data());

			// Set options
			DWORD dwOptions;
			if (SUCCEEDED(pfd->GetOptions(&dwOptions)))
			{
				pfd->SetOptions(dwOptions | a_Options);
			}

			// Set initial folder if provided
			if (!a_InitialPath.empty())
			{
				IShellItem* psiFolder = nullptr;
				hr = SHCreateItemFromParsingName(a_InitialPath.c_str(), NULL, IID_PPV_ARGS(&psiFolder));
				if (SUCCEEDED(hr))
				{
					pfd->SetFolder(psiFolder);
					psiFolder->Release();
				}
			}

			// Show dialog
			if (SUCCEEDED(pfd->Show(NULL)))
			{
				IShellItem* psi;
				if (SUCCEEDED(pfd->GetResult(&psi)))
				{
					LPWSTR pszPath;
					psi->GetDisplayName(SIGDN_DESKTOPABSOLUTEPARSING, &pszPath);

					std::wstring wide(pszPath);
					fs::path resultPath(wide);

					// If user didn�t specify extension, apply default one from filter
					if (resultPath.extension().empty() && !a_aFilters.empty())
					{
						UINT fileTypeIndex = 0;
						if (SUCCEEDED(pfd->GetFileTypeIndex(&fileTypeIndex)) && fileTypeIndex > 0 && fileTypeIndex <= a_aFilters.size())
						{
							std::wstring defExt = a_aFilters[fileTypeIndex - 1].pszSpec;
							// pszSpec can be "*.ext", so strip the "*."
							if (defExt.rfind(L"*.", 0) == 0)
								defExt = defExt.substr(1); // remove '*'

							resultPath += defExt;
						}
					}

					a_sPath = resultPath;

					psi->Release();
					CoTaskMemFree(pszPath);
					pfd->Release();
					return true;
				}
			}

			pfd->Release();
			return false;
		}

		//---------------------------------------------------------------------
		bool PickContainer(fs::path& a_sPath)
		{
			return genericFileOpen(a_sPath, CLSID_FileOpenDialog, FOS_PICKFOLDERS);
		}

		//---------------------------------------------------------------------
		bool PickFile(fs::path& a_sPath, const std::vector<COMDLG_FILTERSPEC>& a_aFilters, const fs::path& a_InitialPath)
		{
			return genericFileOpen(a_sPath, CLSID_FileOpenDialog, 0, a_aFilters, a_InitialPath);
		}

		//---------------------------------------------------------------------
		bool SaveFile(fs::path& a_sPath, const std::vector<COMDLG_FILTERSPEC>& a_aFilters, const fs::path& a_InitialPath)
		{
			return genericFileOpen(a_sPath, CLSID_FileSaveDialog, 0, a_aFilters, a_InitialPath);
		}

		//---------------------------------------------------------------------
		const fs::path GetAppDataPath()
		{
			PWSTR path_tmp;
			fs::path path;
			SHGetKnownFolderPath(FOLDERID_RoamingAppData, 0, nullptr, &path_tmp);
			path = path_tmp;

			fs::create_directories(path);

			return path;
		}

		//---------------------------------------------------------------------
		bool CreateDirectory(const fs::path& a_Path)
		{
			return fs::create_directories(a_Path);
		}

		//---------------------------------------------------------------------
		bool OpenInExplorer(const fs::path& a_Path)
		{
			ShellExecuteA(NULL, "open", fs::absolute(a_Path).string().c_str(), NULL, NULL, SW_SHOWDEFAULT);
			return true;
		}

		//---------------------------------------------------------------------
		bool LoadFile(const fs::path& a_Path, core::Data& a_Data)
		{
			if (!fs::exists(a_Path))
			{
				return false;
			}

			FILE* file = nullptr;
			fopen_s(&file, a_Path.generic_string().c_str(), "rb");
			if (!file)
			{
				return false;
			}

			fseek(file, 0, SEEK_END);
			size_t fileSize = ftell(file);
			rewind(file);

			a_Data = core::DataStream(fileSize);
			fread(a_Data.data(), fileSize, 1, file);

			fclose(file);

			return true;
		}

		//---------------------------------------------------------------------
		bool SaveFile(const fs::path& a_Path, const core::DataStream& a_Data)
		{
			FILE* file = nullptr;
			fopen_s(&file, a_Path.generic_string().c_str(), "wb");
			if (!file)
			{
				return false;
			}

			fwrite(a_Data.data(), a_Data.size(), 1, file);

			fclose(file);

			return true;
		}
	}
}