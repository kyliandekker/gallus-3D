#pragma once

#include <string>

namespace gallus::core
{
	class Data;
}
namespace gallus::resources
{
	class IAssetSource
	{
	public:
		virtual ~IAssetSource() = default;

		/// <summary>
		/// Loads binary data from an asset.
		/// </summary>
		/// <param name="a_sID">The id of the asset (name or path).</param>
		/// <param name="a_OutData">List of asset extensions to filter.</param>
		/// <returns>True if loading was successful, false otherwise.</returns>
		virtual bool LoadBinary(const std::string& a_sID, core::Data& a_OutData) = 0;

		/// <summary>
		/// Loads meta binary data from an asset.
		/// </summary>
		/// <param name="a_sID">The id of the asset (name or path).</param>
		/// <param name="a_OutData">List of asset extensions to filter.</param>
		/// <returns>True if loading was successful, false otherwise.</returns>
		virtual bool LoadMetaData(const std::string& a_sID, bool a_bSeparate, core::Data& a_OutData) = 0;
	};
}