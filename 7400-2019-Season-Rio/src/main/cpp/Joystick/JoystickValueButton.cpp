#include "JoystickValueButton.h"

JoystickValueButton::JoystickValueButton(int index, int valueCount, int initialValue, bool bLoop, int delta)
	: JSButton(index)
{
	m_value = initialValue;
	m_lastValue= initialValue;
	m_valueCount = valueCount;
	m_initialValue = initialValue;
	m_bLoop = bLoop;
	m_delta = delta;
}

bool JoystickValueButton::Update(bool bPressed)
{
	if (JSButton::Update(bPressed))
	{
		if (m_bPressed)
		{
			m_lastValue = m_value;

			m_value += m_delta;

			if (m_delta > 0)
			{
				if (m_value >= m_valueCount)
					m_value = m_bLoop ? 0 : m_valueCount - 1;
			}
			else
			{
				if (m_value < 0)
					m_value = m_bLoop ? m_valueCount - 1 : 0;
			}

			m_bChanged = m_value != m_lastValue ? true : false;

			return true;
		}
		else
			m_bChanged = false;
	}

	return false;
}

void JoystickValueButton::Initialize()
{
	JSButton::Initialize();

	m_bChanged = (m_value != m_initialValue) ? true : false;

	m_lastValue = m_value;
	m_value = m_initialValue;
}
