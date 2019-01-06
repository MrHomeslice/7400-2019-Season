#include "JoystickButton.h"

JSButton::JSButton(int index)
{
	m_bPressed = false;
	m_index = index;
	m_bChanged = false;
}

bool JSButton::Update(bool bPressed)
{
	if (bPressed != m_bPressed)
	{
		m_bPressed = bPressed;
		m_bChanged = true;

		return true;
	}
	return false;
}

void JSButton::Initialize()
{
	m_bPressed = false;
}
