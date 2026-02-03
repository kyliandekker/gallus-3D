#pragma once

#include "core/System.h"

namespace gallus
{
	namespace audio
	{
		//---------------------------------------------------------------------
		// AudioSystem
		//---------------------------------------------------------------------
		/// <summary>
		/// Represents a midi audio system.
		/// </summary>
		class AudioSystem : public core::ThreadedSystem
		{
		public:
			/// <summary>
			/// Initializes the system, setting up necessary resources.
			/// </summary>
			/// <param name="a_bWait">Determines whether the application waits until the system has been fully initialized.</param>
			/// <returns>True if the initialization was successful, otherwise false.</returns>
			bool Initialize(bool a_bWait) override;

			/// <summary>
			/// Cleans up resources and destroys the dx12 window.
			/// </summary>
			/// <returns>True if destruction succeeds, otherwise false.</returns>
			bool Destroy() override;
		protected:
			/// <summary>
			/// Indicates whether the thread should sleep or wake to run work.
			/// Must be implemented by subclasses.
			/// </summary>
			/// <returns>True if thread should sleep (wait), false to proceed immediately.</returns>
			bool Sleep() const override
			{
				return true;
			}

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
		public:
			/// <summary>
			/// Loop method for the thread.
			/// </summary>
			void Loop() override;
		};
	}
}