#include "MeeseeksJoystick.h"
#include "JoystickButton.h"

MeeseeksJoystick::MeeseeksJoystick(int port)
	            : Joystick(port)
{
	m_oldPOV = -1;
	m_newPOV = -1;

	m_bPOVChanged = false;
}

void MeeseeksJoystick::Periodic()
{
	for (unsigned int buttonIndex=0; buttonIndex<m_buttons.size(); buttonIndex++)
	{
		JSButton *pButton = m_buttons[buttonIndex];

		pButton->Changed(false);

		bool buttonState = GetRawButton(pButton->Index() + 1);

		pButton->Update(buttonState);
	}

	m_bPOVChanged = false;

	m_POVState = 0;

	m_newPOV = GetPOV();

	if(m_newPOV != m_oldPOV)
	{
		if((m_oldPOV <= 180 && m_oldPOV >= 0 && m_newPOV >= 180) || (m_oldPOV >= 180 && m_newPOV <= 180 && m_newPOV >= 0) || (m_oldPOV == -1 && m_newPOV > -1))
			m_bPOVChanged = true;
		
		m_oldPOV = m_newPOV;
	}

	if(m_bPOVChanged)
	{
		if(m_newPOV > 180)
			m_POVState = -1;
		else if(m_newPOV > 0 && m_newPOV < 180)
			m_POVState = 1;
	}
}

void MeeseeksJoystick::Initialize()
{
	for (unsigned int buttonIndex=0; buttonIndex<m_buttons.size(); buttonIndex++)
	{
		JSButton *pButton = m_buttons[buttonIndex];

		pButton->Initialize();
	}
}

int MeeseeksJoystick::GetPOVState()
{
	return m_POVState;
}