#include "FPSCounter.h"

// external
#include <thread>

namespace gallus::graphics
{
	void FPSCounter::Update()
	{
		clock::time_point current = clock::now();
		std::chrono::duration<double> elapsed = current - m_tPrevious;
		m_tPrevious = current;

		double deltaSeconds = elapsed.count();

		m_fFPSTimer += deltaSeconds;

		if (m_bLimitFPS)
		{
			m_fLag += deltaSeconds;
			m_iFrameUpdates = 0;

			while (m_fLag >= m_fFrameDurationTarget)
			{
				m_fLag -= m_fFrameDurationTarget;
				m_iFrameUpdates++;

				m_eOnNewFrame(m_fFrameDurationTarget);
			}

			m_fDeltaTime = m_fFrameDurationTarget;
			m_iFrames += m_iFrameUpdates;
		}
		else
		{
			m_fDeltaTime = deltaSeconds;
			m_iFrames++;

			m_eOnNewFrame(m_fDeltaTime);
		}

		if (m_fFPSTimer >= 1.0)
		{
			m_fFPS = static_cast<double>(m_iFrames) / m_fFPSTimer;
			m_iFrames = 0;
			m_fFPSTimer = 0.0;
		}

		if (m_bLimitFPS)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
		}
	}
}
