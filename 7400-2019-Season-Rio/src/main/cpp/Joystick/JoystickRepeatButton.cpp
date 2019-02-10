#include "JoystickRepeatButton.h"
#include <stdio.h>

JoystickRepeatButton::JoystickRepeatButton(int index, int valueCount, int initialValue, bool bLoop, int delta, double dwellTime, double waitTime)
	: JSButton(index)
{
	m_dwellTime = dwellTime;
	m_waitTime = waitTime;
	m_bWaitingForDwell = true;
    m_bTimerRunning = false;
	m_lastUpdateTime = 0.0;
}

bool JoystickRepeatButton::Update(bool bPressed)
{
    if (bPressed)
    {
        if(!m_bTimerRunning)
        {
            m_timer.Reset();
            m_timer.Start();
            m_bTimerRunning = true;
        }
    }
    else
    {
        m_timer.Stop();
        m_timer.Reset();
        m_bTimerRunning = false;
    }
    
    if (m_timer.Get() > m_waitTime) 
	    return JSButton::Update(true);

    return JSButton::Update(false);
}

