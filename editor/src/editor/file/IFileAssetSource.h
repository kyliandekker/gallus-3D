#pragma once

#include "resources/IAssetResource.h"

namespace gallus::file
{
	class IFileAssetSource : public resources::IAssetSource
	{
	public:
		/// <summary>
		/// Loads binary data from an asset.
		/// </summary>
		/// <param name="a_sID">The id of the asset (name or path).</param>
		/// <param name="a_OutData">List of asset extensions to filter.</param>
		/// <returns>True if loading was successful, false otherwise.</returns>
		bool LoadBinary(const std::string& a_sID, core::Data& a_OutData) override;

		/// <summary>
		/// Loads meta binary data from an asset.
		/// </summary>
		/// <param name="a_sID">The id of the asset (name or path).</param>
		/// <param name="a_OutData">List of asset extensions to filter.</param>
		/// <returns>True if loading was successful, false otherwise.</returns>
		bool LoadMetaData(const std::string& a_sID, bool a_bSeparate, core::Data& a_OutData) override;
	};
}