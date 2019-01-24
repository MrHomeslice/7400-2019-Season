#include "RobotControl.h"
#include <frc/GenericHID.h>
#include "..\MeeseeksProperties.h"
#include <unistd.h>

extern MeeseeksProperties g_mp;

RobotControl::RobotControl()
	: m_driveJoyStick(JOYSTICK_1),
	m_acquiredSwitch(0)
{
	m_x = -10.0;
	m_y = -10.0;
	m_z = -10.0;
	m_slider = 0;

	m_lastX = 0.0;
	m_lastY = 0.0;
	m_lastZ = 0.0;

	m_bXYZChanged = false;
	m_bFlipState = true;

	m_ejectCounter = 0;

	m_cargoState = eCargoStateNull;
	m_lastCargoState = eCargoStateNull;
}

void RobotControl::Initialize()
{
	m_ladder.Initialize();
}

double RobotControl::Deadband(double input, double deadbandHalfWidth)
{
	double yInercept;

	if (input > deadbandHalfWidth)
		yInercept = deadbandHalfWidth / (deadbandHalfWidth - 1.0);
	else if (input < -deadbandHalfWidth)
		yInercept = deadbandHalfWidth / (1.0 - deadbandHalfWidth);
	else
		return 0.0;

	double slope = 1.0 / (1.0 - deadbandHalfWidth);

	return input * slope + yInercept;
}

bool RobotControl::Periodic()
{
	m_driveJoyStick.Periodic();

	m_x = Deadband(m_driveJoyStick.GetX(), g_mp.m_deadbandX);
	m_y = -Deadband(m_driveJoyStick.GetY(), g_mp.m_deadbandY);
	m_z = Deadband(m_driveJoyStick.GetZ(), g_mp.m_deadbandZ);
	m_slider = (m_driveJoyStick.GetThrottle() + 1) / 2;
	m_pov = m_driveJoyStick.GetPOV();

	m_bRobotCentric = m_driveJoyStick.CentricityToggle()->Value() == 1 ? true : false;

	m_bCargo = m_driveJoyStick.GetThrottle() > 0.0 ? true : false;

	ProcessCargoState();

	m_ladder.Periodic();

	if ((m_lastX != m_x) || (m_lastY != m_y) || (m_lastZ != m_z))
	{
		m_bXYZChanged = true;
		m_lastX = m_x;
		m_lastY = m_y;
		m_lastZ = m_z;
	}
	else
	{
		m_bXYZChanged = false;
	}

	if (m_driveJoyStick.GetPOVState() == -1) //Left
	{

	}

	else if (m_driveJoyStick.GetPOVState() == 1) //Right
	{

	}

	else //Center
	{

	}

	if (m_driveJoyStick.ElevatorFlip()->Changed() && m_driveJoyStick.ElevatorFlip()->Pressed())
	{
		if (!m_bFlipState)
			m_bFlipState = true;
		else
			m_bFlipState = false;
	}

	return m_bXYZChanged;
}

bool RobotControl::XYZChanged()
{
	return m_bXYZChanged;
}

double RobotControl::X()
{
	return m_x;
}

double RobotControl::Y()
{
	return m_y;
}

double RobotControl::Z()
{
	return m_z / 2;
}

bool RobotControl::Cargo()
{
	return m_bCargo;
}

void RobotControl::TestButtons()
{
	if (m_driveJoyStick.Allign()->Changed() && m_driveJoyStick.Allign()->Pressed())
		printf("Allignment Changed...\n");

	if (m_driveJoyStick.Action()->Changed() && m_driveJoyStick.Action()->Pressed())
		printf("Eject Changed...\n");

	//if(m_driveJoyStick.ElevatorFlip()->Changed() && m_driveJoyStick.ElevatorFlip()->Pressed())
	//	printf("Flip Changed...\n");

	if (m_bCargo)
	{
		if (m_driveJoyStick.TopHeight()->Changed() && m_driveJoyStick.TopHeight()->Pressed())
			printf("Cargo Top Height Changed...\n");

		if (m_driveJoyStick.MidHeight()->Changed() && m_driveJoyStick.MidHeight()->Pressed())
			printf("Cargo Mid Height Changed...\n");

		if (m_driveJoyStick.BottomHeight()->Changed() && m_driveJoyStick.BottomHeight()->Pressed())
			printf("Cargo Bottom Height Changed...\n");
	}

	else
	{
		if (m_driveJoyStick.TopHeight()->Changed() && m_driveJoyStick.TopHeight()->Pressed())
			printf("Hatch Top Height Changed...\n");

		if (m_driveJoyStick.MidHeight()->Changed() && m_driveJoyStick.MidHeight()->Pressed())
			printf("Hatch Mid Height Changed...\n");

		if (m_driveJoyStick.BottomHeight()->Changed() && m_driveJoyStick.BottomHeight()->Pressed())
			printf("Hatch Bottom Height Changed...\n");
	}

	if (m_driveJoyStick.CentricityToggle()->Changed() && m_driveJoyStick.CentricityToggle()->Pressed())
	{
		printf("Centricity Value: %d\n", m_driveJoyStick.CentricityToggle()->Value());
	}
}
bool RobotControl::FlipState()
{
	return m_bFlipState;
}

bool RobotControl::RobotCentric()
{
	return m_driveJoyStick.CentricityToggle()->Value();
}

void RobotControl::ProcessCargoState()
{
	NewStateCheck();


	switch (m_cargoState)
	{
	case eCargoStateNull:
	{
		if (m_driveJoyStick.Action()->Changed() && m_driveJoyStick.Action()->Pressed())
		{
			m_cargoState = eCargoStateAquiring;
		}

		break;
	}
	case eCargoStateAquiring:
	{
		if (m_acquiredSwitch.Get())
		{
			m_cargoState = eCargoStateAquired;
		}

		if (m_driveJoyStick.Action()->Changed() && m_driveJoyStick.Action()->Pressed())
		{
			m_cargoState = eCargoStateNull;
		}

		break;
	}
	case eCargoStateAquired:
	{
		if (m_driveJoyStick.ElevatorFlip()->Changed() && m_driveJoyStick.ElevatorFlip()->Pressed())
		{
			m_cargoState = eCargoStateFlipped;
		}

		break;
	}
	case eCargoStateFlipped:
	{
		if (m_driveJoyStick.Action()->Changed() && m_driveJoyStick.Action()->Pressed())
		{
			m_pneumatics.Flip(FlipState());

			m_cargoState = eCargoStateEjecting;
		}

		break;
	}
	case eCargoStateEjecting:
	{
		m_ejectCounter++;
		if (m_ejectCounter % 50 == 0)
		{
			m_cargoState = eCargoStateEjected;
		}

		break;
	}

	case eCargoStateEjected:
	{
		m_pneumatics.Flip(FlipState());


		m_cargoState = eCargoStateNull;


		break;
	}
	}
}

void RobotControl::NewStateCheck()
{
	if (m_lastCargoState != m_cargoState)
	{
		printf("State: %d\n", m_cargoState);
		m_lastCargoState = m_cargoState;
	}
}