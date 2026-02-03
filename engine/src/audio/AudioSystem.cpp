#include "./AudioSystem.h"

#include "logger/Logger.h"

namespace gallus
{
	namespace audio
	{
		//---------------------------------------------------------------------
		// AudioSystem
		//--------------------------------------------------------------------- 
		bool AudioSystem::Initialize(bool a_bWait)
		{
			LOG(LOGSEVERITY_INFO, LOG_CATEGORY_DX12, "Initializing audio system.");
			return ThreadedSystem::Initialize(a_bWait);
		}

		//---------------------------------------------------------------------
		bool AudioSystem::Destroy()
		{
			LOG(LOGSEVERITY_INFO, LOG_CATEGORY_DX12, "Destroying audio system.");
			return ThreadedSystem::Destroy();
		}

		//---------------------------------------------------------------------
		bool AudioSystem::InitThreadWorker()
		{
			LOG(LOGSEVERITY_SUCCESS, LOG_CATEGORY_DX12, "Initialized audio system.");

			return true;
		}

		//---------------------------------------------------------------------
		void AudioSystem::Finalize()
		{
			LOG(LOGSEVERITY_SUCCESS, LOG_CATEGORY_DX12, "Destroyed audio system.");
		}

		//---------------------------------------------------------------------
		void AudioSystem::Loop()
		{}
	}
}