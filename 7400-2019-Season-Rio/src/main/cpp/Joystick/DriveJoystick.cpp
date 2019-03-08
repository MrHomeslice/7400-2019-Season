#include "DriveJoystick.h"

DriveJoystick::DriveJoystick(int joyStickPort)
			 : MeeseeksJoystick(joyStickPort),
			   m_allign(0), m_action(1), m_stationIntake(2), m_centricityToggle(3, 2, 0, true, 1),
			   m_flip(4), m_topHeight(6), m_cargoShipHeight(7), m_midHeight(8),
			   m_zeroGyro(9), m_bottomHeight(10), m_abort(11)
			   
{
	m_buttons.push_back(&m_allign);
	m_buttons.push_back(&m_action);
	m_buttons.push_back(&m_stationIntake);
	m_buttons.push_back(&m_centricityToggle);
	m_buttons.push_back(&m_topHeight);
	m_buttons.push_back(&m_cargoShipHeight);
	m_buttons.push_back(&m_midHeight);
	m_buttons.push_back(&m_zeroGyro);
	m_buttons.push_back(&m_bottomHeight);
	m_buttons.push_back(&m_abort);
	m_buttons.push_back(&m_flip);
	
}

void DriveJoystick::Periodic()
{
	MeeseeksJoystick::Periodic();
}
