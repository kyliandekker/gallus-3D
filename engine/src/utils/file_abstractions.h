#pragma once

#include <string>
#include <vector>
#include <shtypes.h>
#include <filesystem>

#if defined(CreateDirectory)
#undef CreateDirectory
#undef CreateDirectoryA
#undef CreateDirectoryW
#endif

namespace fs = std::filesystem;

namespace gallus
{
	namespace core
	{
		class Data;
		class DataStream;
	}
	namespace file
	{
		/// <summary>
		/// Opens a folder open dialog.
		/// </summary>
		/// <param name="a_sPath">The directory that was selected.</param>
		/// <returns>True if folder was selected, false otherwise.</returns>
		bool PickDirectory(fs::path& a_sPath);

		/// <summary>
		/// Opens a file open dialog.
		/// </summary>
		/// <param name="a_sPath">The file that was selected.</param>
		/// <param name="a_aFilters">List of file extensions to filter.</param>
		/// <returns>True if file was selected, false otherwise.</returns>
		bool PickFile(fs::path& a_sPath, const std::vector<COMDLG_FILTERSPEC>& a_aFilters = {}, const fs::path& a_InitialPath = {});

		/// <summary>
		/// Opens a file save dialog.
		/// </summary>
		/// <param name="a_sPath">The file that was saved.</param>
		/// <param name="a_aFilters">List of file extensions to filter.</param>
		/// <returns>True if file was saved, false otherwise.</returns>
		bool SaveFile(fs::path& a_sPath, const std::vector<COMDLG_FILTERSPEC>& a_aFilters = {}, const fs::path& a_InitialPath = {});

		/// <summary>
		/// Retrieves the app data path.
		/// </summary>
		/// <returns>Path to the app data folder.</returns>
		const fs::path GetAppDataPath();

		/// <summary>
		/// Opens the explorer in a specified path.
		/// </summary>
		/// <param name="a_sPath">The directory to create.</param>
		/// <returns>True if successful, false otherwise.</returns>
		bool CreateDirectory(const fs::path& a_Path);

		/// <summary>
		/// Opens the explorer in a specified path.
		/// </summary>
		/// <param name="a_sPath">The directory to open.</param>
		/// <returns>True if successful, false otherwise.</returns>
		bool OpenInExplorer(const fs::path& a_Path);

		/// <summary>
		/// Loads a file and puts it in a data stream.
		/// </summary>
		/// <param name="a_Path">The file to open.</param>
		/// <param name="a_Data">The data container to save file info into.</param>
		/// <returns>True if operation was successful, otherwise false.</returns>
		bool LoadFile(const fs::path& a_Path, core::Data& a_Data);

		/// Saves a data container to a file.
		/// </summary>
		/// <param name="a_Path">The file path to save to.</param>
		/// <param name="a_Data">The data container containing the data to save.</param>
		/// <returns>True if operation was successful, otherwise false.</returns>
		bool SaveFile(const fs::path& a_Path, const core::DataStream& a_Data);
	}
}