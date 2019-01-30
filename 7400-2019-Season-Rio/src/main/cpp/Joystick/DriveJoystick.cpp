#include "DriveJoystick.h"

DriveJoystick::DriveJoystick(int joyStickPort)
			 : MeeseeksJoystick(joyStickPort),
			   m_allign(0), m_action(1), m_elevatorFlip(2, 2, 0, true, 1), 
			   m_centricityToggle(3, 2, 0, true, 1), m_topHeight(6), 
			   m_midHeight(8), m_bottomHeight(10), m_abort(11)
			   
{
	m_buttons.push_back(&m_allign);
	m_buttons.push_back(&m_action);
	m_buttons.push_back(&m_elevatorFlip);
	m_buttons.push_back(&m_centricityToggle);
	m_buttons.push_back(&m_topHeight);
	m_buttons.push_back(&m_midHeight);
	m_buttons.push_back(&m_bottomHeight);
	m_buttons.push_back(&m_abort);
}

void DriveJoystick::Periodic()
{
	MeeseeksJoystick::Periodic();
}
