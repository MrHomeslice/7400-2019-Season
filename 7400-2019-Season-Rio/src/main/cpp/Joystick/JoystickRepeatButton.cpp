#include "JoystickRepeatButton.h"
#include <stdio.h>

JoystickRepeatButton::JoystickRepeatButton(int index, int valueCount, int initialValue, bool bLoop, int delta, double dwellTime, double waitTime)
	: JoystickValueButton(index, valueCount, initialValue, bLoop, delta)
{
	m_dwellTime = dwellTime;
	m_waitTime = waitTime;
	m_bWaitingForDwell = true;
	m_lastUpdateTime = 0.0;
}

bool JoystickRepeatButton::Update(bool bPressed)
{
	if (m_bPressed)
	{
		if (bPressed)
		{
			double elapsedTime = m_timer.Get();

			if (m_bWaitingForDwell)
			{
				if (elapsedTime >= m_dwellTime)
				{
					m_bWaitingForDwell = false;
					m_lastUpdateTime = elapsedTime;
					m_bPressed = false;
				}
			}
			else
			{
				if (elapsedTime - m_lastUpdateTime >= m_waitTime)
				{
					m_lastUpdateTime = elapsedTime;
					m_bPressed = false;
				}
			}
		}
		else
			m_timer.Stop();
	}
	else
	{
		if (bPressed)
		{
			m_timer.Reset();
			m_timer.Start();
			m_bWaitingForDwell = true;
		}
	}

	return JoystickValueButton::Update(bPressed);
}

