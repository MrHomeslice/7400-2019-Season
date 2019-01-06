#include "MeeseeksJoystick.h"
#include "JoystickButton.h"

MeeseeksJoystick::MeeseeksJoystick(int port)
	            : Joystick(port)
{
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
}

void MeeseeksJoystick::Initialize()
{
	for (unsigned int buttonIndex=0; buttonIndex<m_buttons.size(); buttonIndex++)
	{
		JSButton *pButton = m_buttons[buttonIndex];

		pButton->Initialize();
	}
}
