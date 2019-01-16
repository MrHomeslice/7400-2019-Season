#include "DriveJoystick.h"

DriveJoystick::DriveJoystick(int joyStickPort)
			 : MeeseeksJoystick(joyStickPort),
			   m_allign(0), m_eject(1), m_elevatorFlip(2), m_intake(3),
			   m_topHeight(6), m_midHeight(8), m_bottomHeight(10)
{
	m_buttons.push_back(&m_allign);
	m_buttons.push_back(&m_eject);
	m_buttons.push_back(&m_elevatorFlip);
	m_buttons.push_back(&m_intake);
	m_buttons.push_back(&m_topHeight);
	m_buttons.push_back(&m_midHeight);
	m_buttons.push_back(&m_bottomHeight);
}

void DriveJoystick::Periodic()
{
	MeeseeksJoystick::Periodic();
}
