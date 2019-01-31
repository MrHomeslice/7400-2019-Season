#include "RobotControl.h"
#include <frc/GenericHID.h>
#include "..\MeeseeksProperties.h"

extern MeeseeksProperties g_mp;

RobotControl::RobotControl()
			 : m_driveJoyStick(JOYSTICK_1),
			   m_cargoControl(0, 1, 2),
			   m_hatchControl(0, 1, 2)
						  
{
	m_x 	 = -10.0;
	m_y 	 = -10.0;
	m_z      = -10.0;
	m_slider = 0;

	m_lastX  = 0.0;
	m_lastY  = 0.0;
	m_lastZ  = 0.0;

	m_bXYZChanged = false;
	m_bChangedHeight = false;
}

void RobotControl::Initialize()
{
	m_ladder.Initialize();
}

double RobotControl::Deadband(double input, double deadbandHalfWidth)
{
	double yInercept;

	if (input > deadbandHalfWidth)
		yInercept =  deadbandHalfWidth / (deadbandHalfWidth - 1.0);
	else if (input < -deadbandHalfWidth)
		yInercept = deadbandHalfWidth / (1.0 - deadbandHalfWidth);
	else
		return 0.0;

	double slope = 1.0/(1.0 - deadbandHalfWidth);

	return input * slope + yInercept;
}

bool RobotControl::Periodic(bool bTeleop)
{
	m_driveJoyStick.Periodic();

	m_x 	 =  Deadband(m_driveJoyStick.GetX(), g_mp.m_deadbandX);
	m_y	     = -Deadband(m_driveJoyStick.GetY(), g_mp.m_deadbandY);
	m_z 	 =  Deadband(m_driveJoyStick.GetZ(), g_mp.m_deadbandZ);
	m_slider = (m_driveJoyStick.GetThrottle() + 1) / 2;
	m_pov    = m_driveJoyStick.GetPOV();

	m_bRobotCentric = m_driveJoyStick.CentricityToggle()->Value() == 1 ? true : false;

	m_hatchControl.Periodic();
	m_cargoControl.ProcessCargoState();
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

	m_bCargo = m_driveJoyStick.GetThrottle() > 0.0 ? true : false;

	if(m_driveJoyStick.GetPOVState() == -1) //Left
	{

	}

	else if(m_driveJoyStick.GetPOVState() == 1) //Right
	{

	}	

	else //Center
	{
		
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

void RobotControl::ReadButtons()
{

	if(m_driveJoyStick.Allign()->Changed() && m_driveJoyStick.Allign()->Pressed())
		printf("Allignment Changed...\n");

	m_bAction = m_driveJoyStick.Action()->Changed() && m_driveJoyStick.Action()->Pressed();
		
	m_bFlipped = m_driveJoyStick.ElevatorFlip()->Changed() && m_driveJoyStick.ElevatorFlip()->Pressed();

	m_bAbort = m_driveJoyStick.Abort()->Changed() && m_driveJoyStick.Abort()->Pressed();
	
	if(m_bFlipped)
	{
		m_flippedStateValue = m_driveJoyStick.ElevatorFlip()->Value();
	}
	
	if(m_bCargo)
	{
		if(m_driveJoyStick.TopHeight()->Changed() && m_driveJoyStick.TopHeight()->Pressed())
		{
			m_bChangedHeight = true;
			m_ladderTargetHeight = eLadderHeightCargoTop;
		}
	
		if(m_driveJoyStick.MidHeight()->Changed() && m_driveJoyStick.MidHeight()->Pressed())
		{
			m_bChangedHeight = true;
			m_ladderTargetHeight = eLadderHeightCargoMid;
		}

		if(m_driveJoyStick.BottomHeight()->Changed() && m_driveJoyStick.BottomHeight()->Pressed())
		{
			m_bChangedHeight = true;
			m_ladderTargetHeight = eLadderHeightCargoBottom;
		}
	}

	else
	{
		if(m_driveJoyStick.TopHeight()->Changed() && m_driveJoyStick.TopHeight()->Pressed())
		{
			m_bChangedHeight = true;
			m_ladderTargetHeight = eLadderHeightHatchTop;
		}
		
		if(m_driveJoyStick.MidHeight()->Changed() && m_driveJoyStick.MidHeight()->Pressed())
		{
			m_bChangedHeight = true;
			m_ladderTargetHeight = eLadderHeightHatchMid;
		}

		if(m_driveJoyStick.BottomHeight()->Changed() && m_driveJoyStick.BottomHeight()->Pressed())
		{
			m_bChangedHeight = true;
			m_ladderTargetHeight = eLadderHeightHatchBottom;
		}
	}

	if(m_driveJoyStick.CentricityToggle()->Changed() && m_driveJoyStick.CentricityToggle()->Pressed())
	{
		printf("Centricity Value: %d\n", m_driveJoyStick.CentricityToggle()->Value());
	}
}

bool RobotControl::RobotCentric()
{
	return m_driveJoyStick.CentricityToggle()->Value();
}

void RobotControl::CargoEjected()
{
	m_bChangedHeight = true;
	m_ladderTargetHeight = eLadderHeightGround;
}