#include "JoystickUpDownButton.h"

JoystickUpDownButton::JoystickUpDownButton(JoystickValueButton *pUpButton, JoystickValueButton *pDownButton)
	: m_pUpButton(pUpButton), m_pDownButton(pDownButton)
{
	m_value = pUpButton->Value();
	m_lastValue = m_value;
	m_bChanged = false;
}

bool JoystickUpDownButton::Update()
{
	m_lastValue = m_value;
	m_bChanged = false;

	if (m_pUpButton->Changed())
	{
		m_value = m_pUpButton->Value();
		m_pDownButton->Value(m_value);
		m_bChanged = true;
		return true;
	}

	if (m_pDownButton->Changed())
	{
		m_value = m_pDownButton->Value();
		m_pUpButton->Value(m_value);
		m_bChanged = true;
		return true;
	}

	return false;
}


