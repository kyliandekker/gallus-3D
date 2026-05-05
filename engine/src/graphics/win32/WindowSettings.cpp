#include "WindowSettings.h"

namespace gallus::graphics::win32
{
	//---------------------------------------------------------------------
	// WindowSettings
	//---------------------------------------------------------------------
	WindowSettings::WindowSettings(const std::string& a_sFileName) : Settings(a_sFileName)
	{}

	//---------------------------------------------------------------------
	void WindowSettings::SetSize(const IVector2& a_vSize)
	{
		m_vSize = a_vSize;
	}

	//---------------------------------------------------------------------
	const IVector2& WindowSettings::GetSize() const
	{
		return m_vSize;
	}

	//---------------------------------------------------------------------
	void WindowSettings::SetPosition(const IVector2& a_vPosition)
	{
		m_vPosition = a_vPosition;
	}

	//---------------------------------------------------------------------
	const IVector2& WindowSettings::GetPosition() const
	{
		return m_vPosition;
	}
}