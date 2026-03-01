#pragma once

#include "core/System.h"

// core
#include "core/Event.h"

namespace gallus::file
{
	class FileResource;
}
namespace gallus::editor
{
	/// <summary>
	/// A system that displays and manages the asset database of the project.
	/// </summary>
	//---------------------------------------------------------------------
	// AssetDatabase
	//---------------------------------------------------------------------
	class AssetDatabase : public core::ThreadedSystem
	{
	public:
		/// <summary>
		/// Initializes the system, setting up necessary resources.
		/// </summary>
		/// <param name="a_bWait">Determines whether the application waits until the system has been fully initialized.</param>
		/// <returns>True if the initialization was successful, otherwise false.</returns>
		bool Initialize(bool a_bWait) override;

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
		/// Returns the on scan completed event.
		/// </summary>
		/// <returns>Reference to the on scan completed event.</returns>
		const SimpleEvent<>& GetOnScanCompleted() const
		{
			return m_eOnScanCompleted;
		}

		/// <summary>
		/// Scans the database.
		/// </summary>
		/// <returns>True if scan succeeds, otherwise false.</returns>
		bool Scan();

		/// <summary>
		/// Retrieves the resource folder.
		/// </summary>
		/// <returns>Pointer to the resource folder.</returns>
		file::FileResource* GetResourceFolder()
		{
			return m_pResourceFolder;
		}
	private:
		/// <summary>
		/// Loop method for the thread.
		/// </summary>
		void Loop() override;

		/// <summary>
		/// Indicates whether the thread should sleep or wake to run work.
		/// Must be implemented by subclasses.
		/// </summary>
		/// <returns>True if thread should sleep (wait), false to proceed immediately.</returns>
		bool Sleep() const override;

		/// <summary>
		/// Called once on the thread to perform initialization steps.
		/// </summary>
		/// <returns>True if the initialization was successful, otherwise false.</returns>
		bool InitThreadWorker() override;

		/// <summary>
		/// Destroys the system, releasing resources and performing necessary cleanup.
		/// </summary>
		/// <returns>True if the destruction was successful, otherwise false.</returns>
		void Finalize() override;

		std::recursive_mutex m_AssetMutex;
		SimpleEvent<> m_eOnScanCompleted; /// Simple event that gets called when the asset database has completed a scan.

		bool m_bRescan; /// Whether the asset database needs to be scanned again.

		file::FileResource* m_pResourceFolder = nullptr;
	};
}