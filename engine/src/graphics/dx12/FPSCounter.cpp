#include "FPSCounter.h"
#include <thread>

namespace gallus
{
	namespace graphics
	{
	void FPSCounter::Update()
        {
			clock::time_point current = clock::now();
			std::chrono::duration<double> elapsed = current - m_tPrevious;
			m_tPrevious = current;

			m_fLag += elapsed.count();
			m_fFPSTimer += elapsed.count();

			m_iFrameUpdates = 0;

			m_fDeltaTime = m_fFrameDurationTarget;
			while (m_fLag >= m_fFrameDurationTarget)
			{
				m_fLag -= m_fFrameDurationTarget;
				m_iFrameUpdates++;
				m_fFPS = static_cast<double>(m_iFrames) / m_fFPSTimer;

				m_eOnNewFrame(m_fFrameDurationTarget);
			}

			m_iFrames += m_iFrameUpdates;

			std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
	}
}