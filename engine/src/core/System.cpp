#include "core/System.h"

#include "logger/Logger.h"

namespace gallus
{
	namespace core
	{
		//---------------------------------------------------------------------
		// System
		//---------------------------------------------------------------------
		bool System::Initialize()
		{
			m_bRunning.store(true);
			return true;
		}

		//---------------------------------------------------------------------
		bool System::Destroy()
		{
			m_bRunning.store(false);
			return true;
		}

		//---------------------------------------------------------------------
		bool System::Running() const
		{
			return m_bRunning.load();
		}

		//---------------------------------------------------------------------
		// ThreadedSystem
		//---------------------------------------------------------------------
		bool ThreadedSystem::Initialize(bool a_Wait)
		{
			if (m_Thread.joinable())
			{
				// Already running
				return false;
			}

			m_bInitialized.store(false);
			m_bRunning.store(false);

			std::promise<bool> initPromise;
			std::future<bool> initFuture = initPromise.get_future();

			// Pass promise by move to ThreadEntry
			m_Thread = std::thread(&ThreadedSystem::ThreadEntry, this, std::move(initPromise));

			if (a_Wait)
			{
				bool success = initFuture.get();
				return success;
			}
			else
			{
				return true;
			}
		}

		//---------------------------------------------------------------------
		void ThreadedSystem::ThreadEntry(std::promise<bool> promise)
		{
			bool success = false;
			success = InitThreadWorker();

			{
				std::unique_lock lock(m_ReadyMutex);
				m_bInitialized.store(success);
				m_ReadyCondVar.notify_one();
			}

			promise.set_value(success);

			if (!success)
			{
				return;
			}

			m_bRunning.store(true);
			std::unique_lock lock(m_RunningMutex);

			while (m_bRunning.load())
			{
				m_RunningCondVar.wait(lock, [this]()
					{
						return Sleep() || !m_bRunning.load();
					});

				if (!m_bRunning.load())
				{
					break;
				}

				Loop();
			}

			Finalize();
		}

		//---------------------------------------------------------------------
		bool ThreadedSystem::Destroy()
		{
			if (!m_Thread.joinable())
			{
				// Already running
				return false;
			}

			// NOTE: Called from main thread.

			m_bRunning.store(false);
			m_RunningCondVar.notify_all();

			if (m_Thread.joinable())
			{
				m_Thread.join();
			}

			return true;
		}
	}
}