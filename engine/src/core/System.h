#pragma once

#include <thread>
#include <mutex>
#include <atomic>
#include <condition_variable>
#include <future>

namespace gallus
{
	namespace core
	{
		//---------------------------------------------------------------------
		// System
		//---------------------------------------------------------------------
		/// <summary>
		/// Base class for all systems within the engine, providing a common interface 
		/// for initialization, destruction, and readiness checking.
		/// </summary>
		class System
		{
		public:
			/// <summary>
			/// Initializes the system, setting up necessary resources.
			/// </summary>
			/// <returns>True if the initialization was successful, otherwise false.</returns>
			virtual bool Initialize();

			/// <summary>
			/// Destroys the system, releasing resources and performing necessary cleanup.
			/// </summary>
			/// <returns>True if the destruction was successful, otherwise false.</returns>
			virtual bool Destroy();

			/// <summary>
			/// Checks whether the system is ready for use.
			/// </summary>
			/// <returns>True if the system is ready, otherwise false.</returns>
			bool Running() const;

		protected:
			std::atomic<bool> m_bRunning{ false }; /// Flag indicating whether the system is running.
		};

		//---------------------------------------------------------------------
		// ThreadedSystem
		//---------------------------------------------------------------------
		/// <summary>
		/// Base class for all threaded systems within the engine, providing a common interface 
		/// for initialization, destruction, and readiness checking.
		/// </summary>
		class ThreadedSystem : public System
		{
		public:
			/// <summary>
			/// Initializes the system and its thread.
			/// </summary>
			/// <param name="a_Wait">If true, waits for initialization to complete before returning.</param>
			/// <returns>True if initialization was successful, false otherwise.</returns>
			virtual bool Initialize(bool a_Wait);

			/// <summary>
			/// Signals the thread to stop and joins it.
			/// </summary>
			/// <returns>True if the destruction was successful, otherwise false.</returns>
			bool Destroy() override;

			/// <summary>
			/// The main loop method run by the thread; must be implemented by subclasses.
			/// </summary>
			virtual void Loop() = 0;

			/// <summary>
			/// Wakes up the thread if it is sleeping.
			/// </summary>
			void WakeUp()
			{
				m_RunningCondVar.notify_one();
			}

		protected:
			/// <summary>
			/// Indicates whether the thread should sleep or wake to run work.
			/// Must be implemented by subclasses.
			/// </summary>
			/// <returns>True if thread should sleep (wait), false to proceed immediately.</returns>
			virtual bool Sleep() const = 0;

			/// <summary>
			/// Called once on the thread to perform initialization steps.
			/// Must be implemented by subclasses.
			/// </summary>
			/// <returns>True if the initialization was successful, otherwise false.</returns>
			virtual bool InitThreadWorker() = 0;

			/// <summary>
			/// Called once the thread is stopping, to release resources.
			/// Must be implemented by subclasses.
			/// </summary>
			virtual void Finalize() = 0;

			/// <summary>
			/// Thread entry point. Internal, runs init and loop.
			/// </summary>
			void ThreadEntry(std::promise<bool> promise);

			std::thread m_Thread; /// The thread.

			std::mutex m_ReadyMutex; /// Mutex for initialization synchronization.
			std::condition_variable m_ReadyCondVar; /// Condition variable to notify init complete.

			std::mutex m_RunningMutex; /// Mutex for controlling running condition.
			std::condition_variable m_RunningCondVar; /// Condition variable for thread wakeup.

			std::atomic<bool> m_bInitialized{ false }; /// Flag indicating thread init done.
		};
	}
}