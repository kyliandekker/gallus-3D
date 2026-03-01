#include "InputSystem.h"

// external
#include <ShlObj_core.h>

namespace gallus::input
{
	bool InputKey::IsKeyDown() const
	{
		if (!IsKey())
		{
			m_bPressed = false;
			return false;
		}

		if (!m_bPressed)
		{
			m_bPressed = true;
			return true;
		}

		return false;
	}
	
	bool InputKey::IsDoubleDown() const
	{
		if (IsKeyDown())
		{
			auto now = std::chrono::steady_clock::now();
			std::chrono::duration<double> delta = now - m_LastClickTime;

			m_LastClickTime = now;

			if (delta.count() <= DOUBLE_CLICK_THRESHOLD)
			{
				return true;
			}
		}

		return false;
	}

	bool InputKey::IsKey() const
	{
		return GetKeyState(m_cKey) & 0x8000;
	}

	void InputSystem::RegisterKey(char a_cKey)
	{
		if (!m_mKeys.contains(a_cKey))
		{
			m_mKeys.insert(std::make_pair(a_cKey, InputKey(a_cKey)));
		}
	}

	bool InputSystem::IsKeyDown(char a_cKey)
	{
		RegisterKey(a_cKey);
		return m_mKeys[a_cKey].IsKeyDown();
	}

	bool InputSystem::IsDoubleDown(char a_cKey)
	{
		RegisterKey(a_cKey);
		return m_mKeys[a_cKey].IsDoubleDown();
	}

	bool InputSystem::IsKey(char a_cKey)
	{
		RegisterKey(a_cKey);
		return m_mKeys[a_cKey].IsKey();
	}

	void InputSystem::Loop()
	{
	}

	bool InputSystem::Sleep() const
	{
		return true;
	}

	bool InputSystem::InitThreadWorker()
	{
		return true;
	}

	void InputSystem::Finalize()
	{}
}