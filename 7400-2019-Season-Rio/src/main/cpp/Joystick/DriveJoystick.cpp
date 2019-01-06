#include "DriveJoystick.h"

DriveJoystick::DriveJoystick(int joyStickPort)
			 : MeeseeksJoystick(joyStickPort),
			   m_grab(0, 2, 0, true, 1),
			   m_frontLeftCenter(4), m_frontRightCenter(5), m_backLeftCenter(2), m_backRightCenter(3), m_centerCenter(1),
			   m_disableSwerve(10, 2, 0, true, 1), m_writeSteerOffsets(11), m_zeroSwerve(9), m_onlyX(6), m_onlyY(7), m_onlyZ(8)
{
	m_buttons.push_back(&m_grab);
	m_buttons.push_back(&m_frontLeftCenter);
	m_buttons.push_back(&m_frontRightCenter);
	m_buttons.push_back(&m_backLeftCenter);
	m_buttons.push_back(&m_backRightCenter);
	m_buttons.push_back(&m_centerCenter);
	m_buttons.push_back(&m_disableSwerve);
	m_buttons.push_back(&m_writeSteerOffsets);
	m_buttons.push_back(&m_onlyX);
	m_buttons.push_back(&m_onlyY);
	m_buttons.push_back(&m_onlyZ);
	m_buttons.push_back(&m_zeroSwerve);
}

void DriveJoystick::Periodic()
{
	MeeseeksJoystick::Periodic();
}
