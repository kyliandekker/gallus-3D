#pragma once

// external
#include <cstdint>
#include <chrono>

// core
#include "core/Event.h"

namespace gallus::graphics
{
	/// <summary>
	/// Tracks and calculates frames per second (FPS).
	/// </summary>
	class FPSCounter
	{
	public:
		FPSCounter()
		{
			Initialize();
		}

		void Initialize()
		{
			m_tPrevious = clock::now();

			m_iFrameUpdates = 0;
			m_iFrames = 0;
			m_bLimitFPS = false;
			m_fFrameDurationTarget = 0.0;

			m_fLag = 0.0;
			m_iFrameCounter = 0;
			m_fFPSTimer = 0.0;
			m_fDeltaTime = 0.0;
			m_fFPS = 0.0;
		}

		void Update();

		void SetTargetFPS(int targetFPS)
		{
			m_fTargetFPS = targetFPS;
			if (m_fTargetFPS > 0)
			{
				m_bLimitFPS = true;
				m_fFrameDurationTarget = 1.0 / static_cast<double>(m_fTargetFPS);
			}
		}

		void DisableFPSLimit()
		{
			m_bLimitFPS = false;
		}

		bool IsFPSLimited() const
		{
			return m_bLimitFPS;
		}

		float GetFPS() const
		{
			return m_fFPS;
		}

		float GetDeltaTime() const
		{
			return m_fDeltaTime;
		}

		float GetTargetFPS() const
		{
			if (!m_bLimitFPS)
			{
				return 1000;
			}
			return m_fTargetFPS;
		}

		Event<float> m_eOnNewFrame;
	private:
		using clock = std::chrono::high_resolution_clock;

		clock::time_point m_tPrevious;
		int m_iFrameUpdates = 0;
		int m_iFrames = 0;
		bool m_bLimitFPS = false;
		double m_fFrameDurationTarget = 0.0f;
		float m_fTargetFPS = 60.0f;

		double m_fLag = 0.0f;
		int m_iFrameCounter = 0;
		double m_fFPSTimer = 0.0f;
		double m_fDeltaTime = 0.0f;
		double m_fFPS = 0.0f;
	};
}