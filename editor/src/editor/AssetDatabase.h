#pragma once

#ifdef _EDITOR

#include <string>

#include "editor/FileResource.h"
#include "core/System.h"
#include "core/Event.h"

namespace gallus
{
	namespace editor
	{
		/// <summary>
		/// A system that displays and manages the asset database of the project.
		/// </summary>
		//---------------------------------------------------------------------
		// AssetDatabase
		//---------------------------------------------------------------------
		class AssetDatabase : public core::System
		{
		public:
			/// <summary>
			/// Initializes the imgui system.
			/// </summary>
			bool Initialize() override;

			/// <summary>
			/// Cleans up resources and destroys the asset database resources.
			/// </summary>
			/// <returns>True if destruction succeeds, otherwise false.</returns>
			bool Destroy() override;

			/// <summary>
			/// Rescans the asset database of a project.
			/// </summary>
			void Rescan();

			/// <summary>
			/// Checks whether the database needs a rescan.
			/// </summary>
			void CheckAssetDatabase();

			/// <summary>
			/// Retrieves the root folder of the project.
			/// </summary>
			/// <returns>A reference to the root folder.</returns>
			FileResource& GetRoot();

			const SimpleEvent<>& GetOnScanCompleted() const
			{
				return m_eOnScanCompleted;
			}

			bool Scan(); /// Function that scans the database.
		private:
			std::recursive_mutex m_AssetMutex;
			SimpleEvent<> m_eOnScanCompleted; /// Simple event that gets called when the asset database has completed a scan.

			bool m_bRescan; /// Whether the asset database needs to be scanned again.
			FileResource m_AssetsRoot; /// The root of the asset database.
		};
	}
}

#endif // _EDITOR